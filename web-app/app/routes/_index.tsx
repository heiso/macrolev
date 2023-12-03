import {
  CategoryScale,
  Chart,
  Legend,
  LineElement,
  LinearScale,
  PointElement,
  Title,
  Tooltip,
} from 'chart.js'
import { useEffect, useState } from 'react'
import { Line } from 'react-chartjs-2'
import { Button } from '../ui/button.tsx'

const MAX_REPORT_POINTS = 100

const colors = [
  '#0000ff',
  '#ffff00',
  '#ff00ff',
  '#00ffff',
  '#ff0000',
  '#00ff00',
  '#ffffff',
  '#000000',
]
const triggered = (ctx, value) => (ctx.p1.raw.trigger ? value : undefined)
const reset = (ctx, value) => (ctx.p1.raw.reset ? value : undefined)

function parseReport(data: DataView) {
  return {
    time: Date.now(),
    row: data.getUint8(0),
    col: data.getUint8(1),
    idleValue: data.getUint16(2, true),
    maxDistance: data.getUint16(4, true),
    value: data.getUint16(6, true),
    distance: data.getUint16(8, true),
    distance8bits: data.getUint8(10),
    velocity: data.getInt8(11),
    acceleration: data.getInt8(12),
    jerk: data.getInt8(13),
    trigger: data.getUint8(14) ? 255 : 0,
    reset: data.getUint8(15) ? 255 : 0,
  }
}

Chart.register(CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend)

export default function Index() {
  const [device, setDevice] = useState<HIDDevice | null>(null)
  const [reports, setReports] = useState<ReturnType<typeof parseReport>[]>([])
  const [currentKey, setCurrentKey] = useState<ReturnType<typeof parseReport>>()

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
            const parsed = parseReport(event.data)
            setReports((reports) => [
              ...(reports.length >= MAX_REPORT_POINTS
                ? reports.slice(reports.length - MAX_REPORT_POINTS)
                : reports),
              parsed,
            ])
            setCurrentKey(parsed)
          }
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
        <div className="flex flex-col gap-6">
          <div>
            <div>Device: {device.productName}</div>
            {currentKey && (
              <ul>
                {reports[reports.length - 1] && reports[reports.length - 2] && (
                  <li>
                    timing (ms):{' '}
                    {reports[reports.length - 1].time - reports[reports.length - 2].time}
                  </li>
                )}
                <li>row: {currentKey.row}</li>
                <li>col: {currentKey.col}</li>
                <li>idleValue: {currentKey.idleValue}</li>
                <li>maxDistance: {currentKey.maxDistance}</li>
              </ul>
            )}
          </div>
          <div>
            <div className="text-center">Calibration values</div>
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
                labels: reports.map((report) => report.time),
                datasets: [
                  {
                    label: 'Idle',
                    data: reports.map((report) => report.idleValue),
                    backgroundColor: colors[0],
                    borderColor: colors[0],
                    pointRadius: 0,
                  },
                  {
                    label: 'Value',
                    data: reports.map((report) => report.value),
                    backgroundColor: colors[1],
                    borderColor: colors[1],
                    pointRadius: 0,
                  },
                  {
                    label: 'Distance',
                    data: reports.map((report) => report.distance),
                    backgroundColor: colors[2],
                    borderColor: colors[2],
                    pointRadius: 0,
                  },
                ],
              }}
            />
          </div>
          <div>
            <div className="text-center">Values</div>
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
                    min: -255,
                    max: 255,
                    ticks: {
                      color: '#fff',
                    },
                  },
                },
              }}
              data={{
                labels: reports,
                datasets: [
                  {
                    label: 'Distance',
                    data: reports,
                    parsing: {
                      yAxisKey: 'distance8bits',
                      xAxisKey: 'time',
                    },
                    segment: {
                      backgroundColor: (ctx) => triggered(ctx, colors[5]) || reset(ctx, colors[6]),
                      borderColor: (ctx) => triggered(ctx, colors[5]) || reset(ctx, colors[6]),
                    },
                    backgroundColor: colors[2],
                    borderColor: colors[2],
                    pointRadius: 0,
                  },
                  // {
                  //   label: 'Velocity',
                  //   data: reports.map((report) => report.velocity),
                  //   backgroundColor: colors[3],
                  //   borderColor: colors[3],
                  //   pointRadius: 0,
                  // },
                  // {
                  //   label: 'Acceleration',
                  //   data: reports.map((report) => report.acceleration),
                  //   backgroundColor: colors[4],
                  //   borderColor: colors[4],
                  //   pointRadius: 0,
                  // },
                  // {
                  //   label: 'Jerk',
                  //   data: reports.map((report) => report.jerk),
                  //   backgroundColor: colors[5],
                  //   borderColor: colors[5],
                  //   pointRadius: 0,
                  // },
                  // {
                  //   label: 'Trigger',
                  //   data: reports.map((report) => report.trigger),
                  //   backgroundColor: colors[6],
                  //   borderColor: colors[6],
                  //   pointRadius: 0,
                  // },
                  // {
                  //   label: 'Reset',
                  //   data: reports.map((report) => report.reset),
                  //   backgroundColor: colors[7],
                  //   borderColor: colors[7],
                  //   pointRadius: 0,
                  // },
                ],
              }}
            />
          </div>
        </div>
      )}
    </div>
  )
}
