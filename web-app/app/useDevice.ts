import { useCallback, useMemo, useRef, useState } from 'react'

const PRODUCT_NAME = 'Macrolev'
const VENDOR_ID = 0xcafe
// const LAYER_COUNT = 2
// const MATRIX_ROWS = 5
// const MATRIX_COLS = 15

export enum Status {
  MIGHT_BE_TAP,
  TAP,
  TRIGGERED,
  RESET,
  RAPID_TRIGGER_RESET,
}

const vendorRequests = {
  VENDOR_REQUEST_KEYS: 0xfe,
  VENDOR_REQUEST_CONFIG: 0xff,
  VENDOR_REQUEST_RESET_CONFIG: 0xfd,
  VENDOR_REQUEST_DFU_MODE: 0xfc,
  VENDOR_REQUEST_CYCLE_DURATION: 0xfb,
}

const vendorValues = {
  VENDOR_VALUE_GET_LENGTH: 0x00,
  VENDOR_VALUE_GET: 0x01,
  VENDOR_VALUE_SET: 0x02,
}

// struct user_config {
//   uint8_t reverse_magnet_pole;
//   uint8_t trigger_offset;
//   uint8_t reset_threshold;
//   uint8_t rapid_trigger_offset;
//   uint16_t keymaps[LAYERS_COUNT][MATRIX_ROWS][MATRIX_COLS];
// };
type UserConfig = {
  reverseMagnetPole: number
  triggerOffset: number
  resetThreshold: number
  rapidTriggerOffset: number
  screamingVelocityTrigger: number
  tapTimeout: number
  keymaps: number[]
}

function parseKeymaps(config: DataView, byteOffset: number) {
  const test: number[] = []
  let i = byteOffset
  while (i < config.byteLength) {
    const value = config.getUint16(i, true)
    // if (value > 0xff) {
    //   console.log(value & 0b0111111111111111)
    // }
    test.push(value)
    i = i + 2
  }

  return test
}

function parseUserConfig(buffer: DataView): UserConfig {
  const config = {
    reverseMagnetPole: buffer.getUint8(0),
    triggerOffset: buffer.getUint8(1),
    resetThreshold: buffer.getUint8(2),
    rapidTriggerOffset: buffer.getUint8(3),
    screamingVelocityTrigger: buffer.getUint8(4),
    tapTimeout: buffer.getUint16(6, true),
    keymaps: parseKeymaps(buffer, 8),
  }

  return config
}

function formatUserConfig(config: UserConfig): BufferSource {
  const buffer = new DataView(new ArrayBuffer(308))
  buffer.setUint8(0, config.reverseMagnetPole)
  buffer.setUint8(1, config.triggerOffset)
  buffer.setUint8(2, config.resetThreshold)
  buffer.setUint8(3, config.rapidTriggerOffset)
  buffer.setUint8(4, config.screamingVelocityTrigger)
  buffer.setUint16(6, config.tapTimeout, true)
  config.keymaps.forEach((keycode, index) => {
    buffer.setUint16(8 + index * 2, keycode, true)
  })

  return buffer
}

const layerTypes = [
  'KEY_TYPE_EMPTY',
  'KEY_TYPE_NORMAL',
  'KEY_TYPE_MODIFIER',
  'KEY_TYPE_CONSUMER_CONTROL',
] as const

const actuationDirections = ['GOING_UP', 'GOING_DOWN'] as const

const actuationStatus = [
  'STATUS_MIGHT_BE_TAP',
  'STATUS_TAP',
  'STATUS_TRIGGERED',
  'STATUS_RESET',
  'STATUS_RAPID_TRIGGER_RESET',
] as const

export type Key = {
  isEnabled: boolean // uint8_t
  row: number // uint8_t
  column: number // uint8_t
  idleCounter: number // uint8_t
  isIdle: boolean // uint8_t
  layers: {
    type: (typeof layerTypes)[number] // uint8_t
    value: number // uint16_t
  }[]
  calibration: {
    cyclesCount: number // uint16_t
    idleValue: number // uint16_t
    maxDistance: number // uint16_t
  }
  state: {
    value: number // uint16_t
    distance: number // uint16_t
    distance8bits: number // uint8_t
    filteredDistance: number // float int32_t ?
    velocity: number // int8_t
    filteredDistance8bits: number // uint8_t
  }
  actuation: {
    direction: (typeof actuationDirections)[number] // uint8_t
    directionChangedPoint: number // uint8_t
    status: (typeof actuationStatus)[number] // uint8_t
    resetOffset: number // uint8_t
    triggerOffset: number // uint8_t
    rapidTriggerOffset: number // uint8_t
    isContinuousRapidTriggerEnabled: boolean // uint8_t
    triggeredAt: number // uint32_t
  }
}

function parseKeys(data: DataView): Key[] {
  let offset = 0
  function getValue(data: DataView, bytes: 8 | 16 | 32 | 'float', unsigned: boolean = true) {
    const currentOffset = offset
    if (bytes === 8) {
      offset = offset + 1
      return unsigned ? data.getUint8(currentOffset) : data.getInt8(currentOffset)
    } else if (bytes === 16) {
      offset = offset + 2
      return unsigned ? data.getUint16(currentOffset, true) : data.getInt16(currentOffset, true)
    } else if (bytes === 32) {
      offset = offset + 4
      return unsigned ? data.getUint32(currentOffset, true) : data.getInt32(currentOffset, true)
    } else {
      offset = offset + 4
      return data.getFloat32(currentOffset)
    }
  }

  const keys: Key[] = []

  try {
    while (true) {
      keys.push({
        isEnabled: Boolean(getValue(data, 8)),
        row: getValue(data, 8),
        column: getValue(data, 8),
        idleCounter: getValue(data, 8),
        isIdle: Boolean(getValue(data, 8)),
        layers: [
          {
            type: layerTypes[getValue(data, 8)],
            value: getValue(data, 16),
          },
          {
            type: layerTypes[getValue(data, 8)],
            value: getValue(data, 16),
          },
        ],
        calibration: {
          cyclesCount: getValue(data, 16),
          idleValue: getValue(data, 16),
          maxDistance: getValue(data, 16),
        },
        state: {
          value: getValue(data, 16),
          distance: getValue(data, 16),
          distance8bits: getValue(data, 8),
          filteredDistance: getValue(data, 'float', false),
          velocity: getValue(data, 8, false),
          filteredDistance8bits: getValue(data, 8),
        },
        actuation: {
          direction: actuationDirections[getValue(data, 8)],
          directionChangedPoint: getValue(data, 8),
          status: actuationStatus[getValue(data, 8)],
          resetOffset: getValue(data, 8),
          triggerOffset: getValue(data, 8),
          rapidTriggerOffset: getValue(data, 8),
          isContinuousRapidTriggerEnabled: Boolean(getValue(data, 8)),
          triggeredAt: getValue(data, 32),
        },
      })
    }
  } catch (err) {
    if (!(err instanceof Error && err.name === 'RangeError')) {
      throw err
    }
  }

  return keys
}

type DeviceConfig = {
  endpointInNumber: number
  endpointOutNumber: number
  alternateNumber: number
  interfaceNumber: number
}

function getDeviceConfig(device: USBDevice): DeviceConfig {
  for (const itf of device.configuration?.interfaces || []) {
    for (let alternateNumber = 0; alternateNumber < itf.alternates.length; alternateNumber++) {
      const alternate = itf.alternates[alternateNumber]
      if (alternate.interfaceClass == 0xff) {
        const endpointInNumber = alternate.endpoints.find(
          ({ direction }) => direction === 'in',
        )?.endpointNumber
        const endpointOutNumber = alternate.endpoints.find(
          ({ direction }) => direction === 'out',
        )?.endpointNumber

        if (endpointInNumber != undefined && endpointOutNumber != undefined) {
          return {
            endpointInNumber,
            endpointOutNumber,
            alternateNumber,
            interfaceNumber: itf.interfaceNumber,
          }
        }
      }
    }
  }

  throw new Error(`${PRODUCT_NAME} HID configuration looks bad to me 😑`)
}

async function openDevice(device: USBDevice): Promise<DeviceConfig> {
  if (!device.opened) {
    await device.open()
  }

  if (device.configuration === null) {
    await device.selectConfiguration(1)
  }

  const config = getDeviceConfig(device)

  await device.claimInterface(config.interfaceNumber)

  await device.selectAlternateInterface(config.interfaceNumber, config.alternateNumber)

  return config
}

export function useDevice() {
  const isConnecting = useRef<boolean>(false)
  const [isLoading, setIsLoading] = useState<boolean>(true)
  const [device, setDevice] = useState<USBDevice | null>(null)
  const [deviceConfig, setDeviceConfig] = useState<DeviceConfig | null>(null)
  const [userConfig, setUserConfig] = useState<UserConfig | null>(null)
  const [keys, setKeys] = useState<Key[]>([])

  const isConnected = useMemo(() => {
    return Boolean(!isConnecting.current && device?.opened && deviceConfig)
  }, [isConnecting, device, deviceConfig])

  const autoReconnect = useCallback(async () => {
    if (isConnecting.current || isConnected) return
    isConnecting.current = true

    try {
      const devices = await navigator.usb.getDevices()
      let _device = devices.find((device) => device.productName === PRODUCT_NAME)

      if (_device) {
        const config = await openDevice(_device)

        setDevice(_device)
        setDeviceConfig(config)
      }
    } catch (error) {
      console.error(error)
    }

    isConnecting.current = false
    setIsLoading(false)
  }, [isConnected])

  const connect = useCallback(async () => {
    if (isConnecting.current || isConnected) return
    isConnecting.current = true
    setIsLoading(true)

    try {
      const devices = await navigator.usb.getDevices()
      let _device = devices.find((device) => device.productName === PRODUCT_NAME)

      if (!_device) {
        _device = await navigator.usb.requestDevice({ filters: [{ vendorId: VENDOR_ID }] })

        if (_device.productName !== PRODUCT_NAME) {
          throw new Error(`This is not a ${PRODUCT_NAME} 😑`)
        }
      }

      const config = await openDevice(_device)

      setDevice(_device)
      setDeviceConfig(config)
    } catch (error) {
      console.error(error)
    }

    isConnecting.current = false
    setIsLoading(false)
  }, [isConnected])

  const disconnect = useCallback(async () => {
    if (!device || !deviceConfig?.interfaceNumber) return

    await device.releaseInterface(deviceConfig.interfaceNumber)
    await device.close()
    await device.forget()

    /**
     * @Hack if i try to connect, then disconnect and connect again, a wild error appears
     * DOMException: Failed to execute 'open' on 'USBDevice': The device was disconnected.
     */
    window.location.reload()

    setDevice(null)
    setDeviceConfig(null)
  }, [device, deviceConfig])

  const write = useCallback(
    async (request: keyof typeof vendorRequests, data?: BufferSource) => {
      if (!device || !deviceConfig?.endpointOutNumber || !isConnected)
        throw new Error('Not connected')

      const result = await device.controlTransferOut(
        {
          requestType: 'vendor',
          recipient: 'endpoint',
          request: vendorRequests[request],
          value: vendorValues.VENDOR_VALUE_SET,
          index: deviceConfig.endpointOutNumber,
        },
        data,
      )

      if (result.status !== 'ok') {
        throw new Error(`controlTransferIn errored with status ${result.status}`)
      }
    },
    [device, deviceConfig, isConnected],
  )

  const read = useCallback(
    async (request: keyof typeof vendorRequests) => {
      if (!device || !deviceConfig?.endpointInNumber || !isConnected)
        throw new Error('Not connected')

      const lenghtResult = await device.controlTransferIn(
        {
          requestType: 'vendor',
          recipient: 'endpoint',
          request: vendorRequests[request],
          value: vendorValues.VENDOR_VALUE_GET_LENGTH,
          index: deviceConfig.endpointInNumber,
        },
        16,
      )

      if (lenghtResult.status !== 'ok') {
        throw new Error(`controlTransferIn errored with status ${lenghtResult.status}`)
      }

      if (!lenghtResult.data) {
        throw new Error(`controlTransferIn errored with empty data`)
      }
      const length = lenghtResult.data.getUint16(0, true)

      const result = await device.controlTransferIn(
        {
          requestType: 'vendor',
          recipient: 'endpoint',
          request: vendorRequests[request],
          value: vendorValues.VENDOR_VALUE_GET,
          index: deviceConfig.endpointInNumber,
        },
        length,
      )

      if (result.status !== 'ok') {
        throw new Error(`controlTransferIn errored with status ${result.status}`)
      }

      if (result.data) {
        return result.data
      }

      throw new Error(`controlTransferIn errored empty data`)
    },
    [device, deviceConfig, isConnected],
  )

  const loadUserConfig = useCallback(async () => {
    const config = await read('VENDOR_REQUEST_CONFIG')
    setUserConfig(parseUserConfig(config))
  }, [read])

  const persistUserConfig = useCallback(async () => {
    if (!userConfig) throw new Error('cannot persist an empty config')

    await write('VENDOR_REQUEST_CONFIG', formatUserConfig(userConfig))
  }, [userConfig, write])

  const loadKeys = useCallback(async () => {
    const keys = await read('VENDOR_REQUEST_KEYS')
    setKeys(parseKeys(keys))
  }, [read])

  return {
    autoReconnect,
    connect,
    disconnect,
    read,
    write,
    device,
    deviceConfig,
    isLoading,
    isConnected,
    loadUserConfig,
    persistUserConfig,
    userConfig,
    setUserConfig,
    loadKeys,
    keys,
  }
}
