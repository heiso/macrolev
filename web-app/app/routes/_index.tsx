import { useEffect, useState } from 'react'
import { Button } from '../ui/button.tsx'

function parseReport(data: DataView) {
  const row = data.getUint8(0)
  const col = data.getUint8(1)
  const distance = data.getUint8(2)
  const velocity = data.getInt8(3)
  const acceleration = data.getInt8(4)
  const jerk = data.getInt8(5)

  return `${row}:${col} ${distance} ${velocity} ${acceleration} ${jerk}`
}

export default function Index() {
  const [device, setDevice] = useState<HIDDevice | null>(null)
  const [reports, setReports] = useState<string[]>([])

  useEffect(() => {
    if (!device) {
      navigator.hid
        .getDevices()
        .then((devices) =>
          setDevice(
            devices.find((device) =>
              device.collections.find((collection) => collection.usagePage == 0xff00),
            ) || null,
          ),
        )
    } else {
      if (!device.opened) {
        device.open().then(() => {
          device.oninputreport = (event) => {
            setReports((reports) => [...reports, parseReport(event.data)])
            console.log(event)
          }
          console.log('device opened', device)
        })
      }
    }
  }, [device])

  return (
    <div className="h-full w-full flex flex-col">
      {!device ? (
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
            setDevice(devices[0])
          }}
        >
          scan devices
        </Button>
      ) : (
        <Button
          primary
          onClick={async () => {
            device.forget()
            setDevice(null)
          }}
        >
          disconnect
        </Button>
      )}

      {device && (
        <div>
          <div>{device.productName}</div>
          <div>
            {reports.reverse().map((report, index) => (
              <div key={index}>{report}</div>
            ))}
          </div>
        </div>
      )}
    </div>
  )
}
