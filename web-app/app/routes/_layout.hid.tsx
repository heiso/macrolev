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
import { getKeysWithScadCoordinates, getNormalizedKLEKeys } from '../kle-parser.ts'
import layout from '../layout.json'
import { Button } from '../ui/button.tsx'

const status = ['STATUS_MIGHT_BE_TAP', 'STATUS_TAP', 'STATUS_TRIGGERED', 'STATUS_RESET'] as const

/**
 * scan
 * get devices
 * store correct device
 */

type key = {
  row: number
  col: number
  idleValue: number
  maxDistance: number
  value: number
  distance8bits: number
  status: (typeof status)[number]
}

const MAX_REPORTS = 1000

function dataViewToHexs(data: DataView) {
  let hexs = data.byteLength.toString()

  for (let i = 0; i < data.byteLength; i++) {
    if (i % 10 == 0) {
      hexs += '\n'
    }
    hexs += data.getUint8(i).toString(16).padStart(2, '0').toUpperCase() + ' '
  }

  return hexs
}

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
const triggered = (ctx: any, value: string) => (ctx.p1.raw.trigger ? value : undefined)
const reset = (ctx: any, value: string) => (ctx.p1.raw.reset ? value : undefined)

function parseReport(data: DataView) {
  const keys = new Array(6).fill(0).map((_, index) => {
    const offset = index * 10
    return {
      // uint8_t row;
      // uint8_t column;
      // uint16_t idle_value;
      // uint16_t max_distance;
      // uint16_t value;
      // uint8_t distance_8bits;
      // enum actuation_status status;
      row: data.getUint8(offset),
      col: data.getUint8(offset + 1),
      idleValue: data.getUint16(offset + 2, true),
      maxDistance: data.getUint16(offset + 4, true),
      value: data.getUint16(offset + 6, true),
      distance8bits: data.getUint8(offset + 8),
      status: status[data.getUint8(offset + 9)],
    }
  })

  return {
    time: Date.now(),
    keys,
    // row: data.getUint8(0),
    // col: data.getUint8(1),
    // idleValue: data.getUint16(2, true),
    // maxDistance: data.getUint16(4, true),
    // value: data.getUint16(6, true),
    // distance8bits: data.getUint8(8),
    // status: status[data.getUint8(9)],
    duration: data.getUint8(59),
    hex: dataViewToHexs(data),
  }
}

type Report = ReturnType<typeof parseReport>

Chart.register(CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend)

export default function Index() {
  const [devices, setDevices] = useState<HIDDevice[]>([])
  const [reports, setReports] = useState<Report[]>([])

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
            device.open().then(() => {
              device.oninputreport = (event) => {
                setReports((reports) => [
                  ...(reports.length >= MAX_REPORTS
                    ? reports.slice(reports.length - MAX_REPORTS)
                    : reports),
                  parseReport(event.data),
                ])
              }
            })
          }
        })
    }
  }, [devices])

  return (
    <>
      {devices.length == 0 ? (
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
          Scan
        </Button>
      ) : (
        <>
          <Button
            primary
            onClick={() => {
              devices.forEach((device) => device.forget())
              setDevices([])
            }}
          >
            Disconnect
          </Button>
          <Button
            onClick={() => {
              setReports([])
              setDevices([])
            }}
          >
            Refresh
          </Button>
        </>
      )}

      <Keyboard />

      <Reports reports={reports} />
    </>
  )
}

function Keyboard() {
  const keys = getKeysWithScadCoordinates(getNormalizedKLEKeys(layout))
  return (
    <div className={`relative w-full aspect-[17/6] text-sm`}>
      {keys.map((key, index) => (
        <div
          className={`absolute overflow-hidden p-0.5`}
          style={{
            bottom: `${key.y * 3}rem`,
            left: `${key.x * 3}rem`,
            width: `${key.w * 3}rem`,
            height: `${key.h * 3}rem`,
          }}
          key={index}
        >
          <div className="w-full h-full rounded-md bg-slate-500 flex flex-col justify-center items-center">
            {key.ref.split(' ').map((legend, index) => (
              <span key={index}>{legend}</span>
            ))}
          </div>
        </div>
      ))}
    </div>
  )
}

type GraphProps = {
  reports: ReturnType<typeof parseReport>[]
}
function Graph({ reports }: GraphProps) {
  return (
    <>
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
                label: 'Max Distance',
                data: reports.map((report) => report.idleValue - report.maxDistance),
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
            ],
          }}
        />
      </div>
    </>
  )
}

type ReportsProps = {
  reports: Report[]
}
function Reports({ reports }: ReportsProps) {
  return <pre>{reports.sort((a, b) => b.time - a.time).map((report) => report.hex + '\n\n')}</pre>
}
