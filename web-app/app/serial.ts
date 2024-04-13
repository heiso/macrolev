var serial: any = {}

export class Port {
  static async getPorts() {
    const devices = await navigator.usb.getDevices()
    return devices.map((device) => new serial.Port(device))
  }

  static async requestPort() {
    const filters = [
      { vendorId: 0xcafe }, // TinyUSB
      { vendorId: 0x239a }, // Adafruit
      { vendorId: 0x2e8a }, // Raspberry Pi
      { vendorId: 0x303a }, // Espressif
      { vendorId: 0x2341 }, // Arduino
    ]
    return navigator.usb.requestDevice({ filters: filters }).then((device) => new Port(device))
  }

  public interfaceNumber: number
  public endpointIn: number
  public endpointOut: number

  constructor(
    public device: USBDevice,
    public onReceive: (data?: DataView) => void = () => {},
    public onReceiveError: (error: unknown) => void = () => {},
  ) {
    this.interfaceNumber = 0
    this.endpointIn = 0
    this.endpointOut = 0
  }

  async connect() {
    let readLoop = () => {
      this.device.transferIn(this.endpointIn, 64).then(
        (result) => {
          this.onReceive(result.data)
          readLoop()
        },
        (error) => {
          this.onReceiveError(error)
        },
      )
    }

    return this.device
      .open()
      .then(() => {
        if (this.device.configuration === null) {
          return this.device.selectConfiguration(1)
        }
      })
      .then(() => {
        var interfaces = this.device.configuration?.interfaces || []
        interfaces.forEach((element) => {
          element.alternates.forEach((elementalt) => {
            if (elementalt.interfaceClass == 0xff) {
              this.interfaceNumber = element.interfaceNumber
              elementalt.endpoints.forEach((elementendpoint) => {
                if (elementendpoint.direction == 'out') {
                  this.endpointOut = elementendpoint.endpointNumber
                }
                if (elementendpoint.direction == 'in') {
                  this.endpointIn = elementendpoint.endpointNumber
                }
              })
            }
          })
        })
      })
      .then(() => this.device.claimInterface(this.interfaceNumber))
      .then(() => this.device.selectAlternateInterface(this.interfaceNumber, 0))
      .then(() =>
        this.device.controlTransferOut({
          requestType: 'class',
          recipient: 'interface',
          request: 0x22,
          value: 0x01,
          index: this.interfaceNumber,
        }),
      )
      .then(() => {
        readLoop()
      })
  }

  async disconnect() {
    return this.device
      .controlTransferOut({
        requestType: 'class',
        recipient: 'interface',
        request: 0x22,
        value: 0x00,
        index: this.interfaceNumber,
      })
      .then(() => this.device.close())
  }

  send(data: BufferSource) {
    return this.device.transferOut(this.endpointOut, data)
  }
}
