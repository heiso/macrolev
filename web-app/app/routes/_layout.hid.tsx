import { CategoryScale, Chart, LineElement, LinearScale, PointElement } from 'chart.js'
import Annotation from 'chartjs-plugin-annotation'
import { useCallback, useEffect, useMemo, useState } from 'react'
import { Line } from 'react-chartjs-2'
import layout from '../parsed-layout.json'
import { Button } from '../ui/button.tsx'
import { Icon } from '../ui/icon.tsx'
import { Link } from '../ui/link.tsx'

const MAX_VALUES = 100
const status = [
  'STATUS_MIGHT_BE_TAP',
  'STATUS_TAP',
  'STATUS_TRIGGERED',
  'STATUS_RESET',
  'STATUS_RAPID_TRIGGER_RESET',
] as const
type Status = (typeof status)[number]
const visualizationModes = ['NORMAL', 'DISTANCE_HEATMAP', 'IDLE_HEATMAP'] as const
type VisualizationMode = (typeof visualizationModes)[number]

type Calibration = {
  idleValue: number
  maxDistance: number
}

type CalibrationRange = {
  idleValue: {
    min: number
    max: number
  }
  maxDistance: {
    min: number
    max: number
  }
}

type State = {
  value: number
  distance8bits: number
  status: Status
}

type Key = {
  row: number
  col: number

  calibration: Calibration
  state: State

  calibrations: (Calibration & { time: Date })[]
  states: (State & { time: Date })[]
}

type KeysByRowCol = Record<string, Key | undefined>

function dataViewToHexs(data: DataView) {
  let hexs = ''

  for (let i = 0; i < data.byteLength; i++) {
    if (i % 10 == 0 && i !== 0) {
      hexs += '\n'
    }
    hexs += data.getUint8(i).toString(16).padStart(2, '0').toUpperCase() + ' '
  }

  return hexs
}

function isBufferEmpty(data: DataView, offset: number, length: number) {
  for (let i = offset; i < offset + length; i++) {
    if (data.getUint8(i) !== 0) {
      return false
    }
  }
  return true
}

function parseRawOffset(value: number) {
  return Number(((value * 4) / 255).toFixed(1))
}

function formatRawOffset(value: number) {
  return (value * 255) / 4
}

export default function Index() {
  const [devices, setDevices] = useState<HIDDevice[]>([])
  const [rawReports, setRawReports] = useState<string[]>([
    '00 00 00 00 00 00 00 00 00 00 \n00 00 00 00 00 00 00 00 00 00 \n00 00 00 00 00 00 00 00 00 00 \n00 00 00 00 00 00 00 00 00 00 \n00 00 00 00 00 00 00 00 00 00 \n00 00 00 00 00 00 00 00 00 00 \n00 00 00 00',
  ])
  const [duration, setDuration] = useState(0)
  const [keys, setKeys] = useState<KeysByRowCol>({})
  const [keyIndex, setKeyIndex] = useState<string | null>(null)
  const [mode, setMode] = useState<VisualizationMode>('NORMAL')
  const [triggerOffset, setTriggerOffset] = useState(0)
  const [rapidTriggerOffset, setRapidTriggerOffset] = useState(0)
  const [resetOffset, setResetOffset] = useState(0)

  const handleKeyDown = (event: KeyboardEvent) => {
    event.preventDefault()
  }

  useEffect(() => {
    window.addEventListener('keydown', handleKeyDown)

    return () => {
      window.removeEventListener('keydown', handleKeyDown)
    }
  }, [])

  const parseReport = useCallback((data: DataView) => {
    const time = new Date()
    new Array(6).fill(null).forEach((_, index) => {
      if (isBufferEmpty(data, index * 10, 10)) return null

      const offset = index * 10

      const row = data.getUint8(offset)
      const col = data.getUint8(offset + 1)

      const state: State = {
        value: data.getUint16(offset + 6, true),
        distance8bits: data.getUint8(offset + 8),
        status: status[data.getUint8(offset + 9)],
      }

      const calibration: Calibration = {
        idleValue: data.getUint16(offset + 2, true),
        maxDistance: data.getUint16(offset + 4, true),
      }

      setKeys((keys) => ({
        ...keys,
        [`${row}-${col}`]: {
          row,
          col,
          calibration,
          state,
          calibrations: [
            ...(keys[`${row}-${col}`]?.calibrations || []),
            { ...calibration, time },
          ].slice(-MAX_VALUES),
          states: [...(keys[`${row}-${col}`]?.states || []), { ...state, time }].slice(-MAX_VALUES),
        },
      }))
    })
    setDuration((duration) => data.getUint8(60) * (1 - 0.8) + duration * 0.8)
    setTriggerOffset(() => parseRawOffset(Number(data.getUint8(61))))
    setResetOffset(() => Number(data.getUint8(62)))
    setRapidTriggerOffset(() => parseRawOffset(Number(data.getUint8(63))))
    setRawReports((rawReports) => [dataViewToHexs(data), ...rawReports].slice(0, MAX_VALUES))
  }, [])

  useEffect(() => {
    if (devices.length == 0) {
      navigator.hid.getDevices().then((devices) => {
        setDevices(devices)
      })
    } else {
      devices
        .filter((device) => device.collections.length == 1)
        .forEach((device) => {
          if (!device.opened) {
            device.open().then(() => (device.oninputreport = (event) => parseReport(event.data)))
          } else {
            device.oninputreport = (event) => parseReport(event.data)
          }
        })
    }
  }, [devices])

  return (
    <div className="space-y-8">
      <div className="relative space-y-2">
        {devices.length === 0 && (
          <div className="absolute z-10 top-0 bottom-0 left-0 right-0 flex justify-center items-center">
            <Button
              primary
              onClick={async () => {
                const devices = await navigator.hid.requestDevice({
                  filters: [
                    {
                      vendorId: 0xcafe,
                    },
                  ],
                })
                setDevices(devices)
              }}
            >
              Connect your macrolev
            </Button>
          </div>
        )}
        <div className={devices.length == 0 ? 'opacity-80' : ''}>
          <Keyboard keys={keys} mode={mode} onClick={setKeyIndex} />
        </div>
        <div className={`w-full flex justify-between ${devices.length == 0 ? 'hidden' : ''}`}>
          <div className="flex gap-4">
            <Link preventScrollReset to="" onClick={() => setMode('IDLE_HEATMAP')}>
              Show idle value heatmap
            </Link>
            <Link preventScrollReset to="" onClick={() => setMode('DISTANCE_HEATMAP')}>
              Show max distance heatmap
            </Link>
            <Link preventScrollReset to="" onClick={() => setMode('NORMAL')}>
              Show analog values
            </Link>
            <Link
              className="inline-flex gap-2"
              to=""
              onClick={() => {
                const json = JSON.stringify(keys, null, 2)
                navigator.clipboard.writeText(json)
              }}
              title="Copy dataset to clipboard"
            >
              <Icon id="clipboard" className="self-center fill-pink-500" />
            </Link>
            <div className="flex gap-2 items-center">
              <input
                className="cursor-pointer"
                min={0.1}
                max={4}
                step={0.1}
                type="range"
                onChange={(event) => {
                  setTriggerOffset(Number(event.target.value))
                }}
                value={triggerOffset}
              />
              <span className="text-slate-400">{triggerOffset} mm</span>
              <input
                className="cursor-pointer"
                min={0.1}
                max={4}
                step={0.1}
                type="range"
                onChange={(event) => {
                  setRapidTriggerOffset(Number(event.target.value))
                }}
                value={rapidTriggerOffset}
              />
              <span className="text-slate-400">{rapidTriggerOffset} mm</span>
              <Link
                preventScrollReset
                to=""
                onClick={() =>
                  devices
                    .filter((device) => device.collections.length == 1)
                    .forEach((device) =>
                      device.sendReport(
                        0,
                        new Uint8Array([
                          formatRawOffset(triggerOffset),
                          resetOffset,
                          formatRawOffset(rapidTriggerOffset),
                        ]),
                      ),
                    )
                }
              >
                Save
              </Link>
            </div>
          </div>
          <div className="text-right text-slate-400">
            <span>Avg cycle duration </span>
            <span className="text-pink-500">{duration.toFixed(0)} ms</span>
          </div>
        </div>
      </div>

      {devices.length !== 0 && (
        <>
          <div className="flex gap-4 justify-start">
            <div className="space-y-4 bg-slate-700 rounded-md p-4">
              <div className="text-xl text-slate-50">Raw HID reports</div>
              <pre className="overflow-y-auto h-96">
                {rawReports.map((report) => report + '\n\n')}
              </pre>
            </div>

            <div className="w-full h-full bg-slate-700 rounded-md p-4">
              {keyIndex ? (
                <Graph
                  keyItem={keys[keyIndex]}
                  triggerOffset={triggerOffset}
                  resetOffset={resetOffset}
                />
              ) : (
                <div className="h-full">Click on a key to show some charts</div>
              )}
            </div>
          </div>

          <div className="space-x-4">
            <Button
              primary
              onClick={() => {
                devices.forEach((device) => device.forget())
                setDevices([])
              }}
            >
              Disconnect
            </Button>
          </div>
        </>
      )}
    </div>
  )
}

const heatmapColorsStyles = [
  'bg-slate-300',
  'bg-slate-400',
  'bg-slate-500',
  'bg-slate-600',
  'bg-slate-700',
]
function getColor(min: number, max: number, value?: number) {
  if (!value) return 'bg-slate-500'

  const index = Math.floor(((value - min) * (heatmapColorsStyles.length - 1)) / (max - min))
  return heatmapColorsStyles[index] || 'bg-slate-500'
}

type KeyProps = {
  keyItem?: Key
  mode: VisualizationMode
  calibrationRange: CalibrationRange
  legends: string[]
  onClick: (index: string) => void
}
function Key({ mode, calibrationRange, legends, keyItem: key, onClick }: KeyProps) {
  const color = useMemo(() => {
    switch (mode) {
      case 'NORMAL':
        return 'bg-slate-500'

      case 'IDLE_HEATMAP':
        return getColor(
          calibrationRange.idleValue.min,
          calibrationRange.idleValue.max,
          key?.calibration.idleValue,
        )

      case 'DISTANCE_HEATMAP':
        return getColor(
          calibrationRange.maxDistance.min,
          calibrationRange.maxDistance.max,
          key?.calibration.maxDistance,
        )
    }
  }, [key, calibrationRange, mode])

  return (
    <div
      className={`relative w-full h-full rounded-md ${color} flex flex-col justify-center overflow-hidden items-center [text-shadow:0_0_3px_rgba(0,0,0,0.9)] transition-transform cursor-pointer`}
      onClick={() => onClick(`${key?.row}-${key?.col}`)}
    >
      {key && (
        <div
          className={`absolute bottom-0 left-0 right-0 ${
            key.state.status !== 'STATUS_RESET' && key.state.status !== 'STATUS_RAPID_TRIGGER_RESET'
              ? 'bg-pink-700'
              : 'bg-pink-300'
          }`}
          style={{ height: `${(key.state.distance8bits * 100) / 255}%` }}
        ></div>
      )}
      {legends.map((legend, index) => (
        <span className="z-10" key={index}>
          {legend}
        </span>
      ))}
      {key && (
        <span className="z-10">
          {mode === 'IDLE_HEATMAP'
            ? key.calibration.idleValue.toString()
            : mode === 'DISTANCE_HEATMAP'
            ? key.calibration.maxDistance.toString()
            : key.state.distance8bits.toString()}
        </span>
      )}
    </div>
  )
}

type KeyboardProps = {
  keys: KeysByRowCol
  mode: VisualizationMode
  onClick: KeyProps['onClick']
}
function Keyboard({ keys, mode, onClick }: KeyboardProps) {
  const width = useMemo(() => layout.reduce((acc, key) => Math.max(acc, key.x + key.w), 0), [])
  const height = useMemo(() => layout.reduce((acc, key) => Math.max(acc, key.y + key.h), 0), [])
  const [calibrationRange, setCalibrationRange] = useState<CalibrationRange>({
    idleValue: { min: Infinity, max: 0 },
    maxDistance: { min: Infinity, max: 0 },
  })

  useEffect(() => {
    const calibrations = Object.values(keys).map((key) => key!.calibration)
    if (calibrations.length) {
      setCalibrationRange({
        idleValue: {
          min: calibrations.reduce(
            (acc, calibration) =>
              calibration.idleValue !== 0 ? Math.min(acc, calibration.idleValue) : acc,
            Infinity,
          ),
          max: calibrations.reduce((acc, calibration) => Math.max(acc, calibration.idleValue), 0),
        },
        maxDistance: {
          min: calibrations.reduce(
            (acc, calibration) =>
              calibration.maxDistance !== 0 ? Math.min(acc, calibration.maxDistance) : acc,
            Infinity,
          ),
          max: calibrations.reduce((acc, calibration) => Math.max(acc, calibration.maxDistance), 0),
        },
      })
    }
  }, [keys])

  return (
    <div className={`relative w-full text-sm`} style={{ aspectRatio: `${width}/${height}` }}>
      {layout.map((keyCoord) => {
        const index = `${keyCoord.row}-${keyCoord.col}`
        const key = keys[index]
        return (
          <div
            key={index}
            className={`absolute p-0.5`}
            style={{
              top: `${keyCoord.y * (100 / height)}%`,
              left: `${keyCoord.x * (100 / width)}%`,
              width: `${keyCoord.w * (100 / width)}%`,
              height: `${keyCoord.h * (100 / height)}%`,
            }}
          >
            <Key
              onClick={onClick}
              keyItem={key}
              legends={keyCoord.ref.split(' ')}
              mode={mode}
              calibrationRange={calibrationRange}
            />
          </div>
        )
      })}
    </div>
  )
}

const triggered = (ctx: any, value: string) =>
  ctx.p1.raw.status !== 'STATUS_RESET' && ctx.p1.raw.status !== 'STATUS_RAPID_TRIGGER_RESET'
    ? value
    : undefined
const reset = (ctx: any, value: string) =>
  ctx.p1.raw.status === 'STATUS_RESET' || ctx.p1.raw.status === 'STATUS_RAPID_TRIGGER_RESET'
    ? value
    : undefined

Chart.register(CategoryScale, LinearScale, PointElement, LineElement, Annotation)

type GraphProps = {
  keyItem?: Key
  triggerOffset: number
  resetOffset: number
}
function Graph({ keyItem, triggerOffset, resetOffset }: GraphProps) {
  const [isStateMode, setIsStateMode] = useState(true)
  if (!keyItem) return null

  return (
    <div className="w-full h-full">
      {!isStateMode ? (
        <Line
          options={{
            color: '#fff',
            animation: false,
            responsive: true,
            scales: {
              x: {
                display: false,
              },
              y: {
                ticks: {
                  color: '#fff',
                },
              },
            },
          }}
          data={{
            labels: keyItem.calibrations.map((calibration) => calibration.time),
            datasets: [
              {
                label: 'Idle',
                data: keyItem.calibrations.map((calibration) => calibration.idleValue),
                backgroundColor: '#be185d',
                borderColor: '#be185d',
                pointRadius: 0,
              },
              {
                label: 'Value',
                data: keyItem.states.map((state) => state.value),
                backgroundColor: '#f472b6',
                borderColor: '#f472b6',
                pointRadius: 0,
              },
              {
                label: 'Max Distance',
                data: keyItem.calibrations.map((calibration) => calibration.maxDistance),
                backgroundColor: '#fbcfe8',
                borderColor: '#fbcfe8',
                pointRadius: 0,
              },
            ],
          }}
        />
      ) : (
        <Line
          options={{
            animation: false,
            responsive: true,
            plugins: {
              legend: {
                display: false,
              },
              annotation: {
                annotations: [
                  {
                    type: 'line',
                    borderColor: '#be185d',
                    borderWidth: 2,
                    scaleID: 'y',
                    value: formatRawOffset(triggerOffset),
                  },
                  {
                    type: 'line',
                    borderColor: '#f9a8d4',
                    borderWidth: 2,
                    scaleID: 'y',
                    value: formatRawOffset(triggerOffset) - resetOffset,
                  },
                ],
              },
            },
            scales: {
              x: {
                display: false,
              },
              y: {
                min: 0,
                max: 255,
                ticks: {
                  color: '#fff',
                },
              },
            },
          }}
          data={{
            labels: keyItem.states,
            datasets: [
              {
                label: 'Distance',
                data: keyItem.states,
                parsing: {
                  yAxisKey: 'distance8bits',
                  xAxisKey: 'time',
                },
                segment: {
                  backgroundColor: (ctx) => triggered(ctx, '#be185d') || reset(ctx, '#f9a8d4'),
                  borderColor: (ctx) => triggered(ctx, '#be185d') || reset(ctx, '#f9a8d4'),
                },
                pointRadius: 0,
              },
              {
                label: 'Changing point',
                data: keyItem.states,
                parsing: {
                  yAxisKey: 'directionChangingPoint',
                  xAxisKey: 'time',
                },
                pointRadius: 3,
                pointBackgroundColor: 'red',
                showLine: false,
              },
            ],
          }}
        />
      )}
      <div className="flex justify-end gap-4 w-full">
        <Link preventScrollReset to="" onClick={() => setIsStateMode(!isStateMode)}>
          Change mode
        </Link>
      </div>
    </div>
  )
}
