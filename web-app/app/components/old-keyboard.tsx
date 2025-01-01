import { useEffect, useMemo, useState } from 'react'
import layout from '../parsed-layout.json'
import { type Key } from '../useDevice'

export const visualizationModes = ['NORMAL', 'DISTANCE_HEATMAP', 'IDLE_HEATMAP'] as const
export type VisualizationMode = (typeof visualizationModes)[number]

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

type KeyboardProps = {
  keys: Key[]
}
export function Keyboard({ keys }: KeyboardProps) {
  const width = useMemo(() => layout.reduce((acc, key) => Math.max(acc, key.x + key.w), 0), [])
  const height = useMemo(() => layout.reduce((acc, key) => Math.max(acc, key.y + key.h), 0), [])
  const [calibrationRange, setCalibrationRange] = useState<CalibrationRange>({
    idleValue: { min: Infinity, max: 0 },
    maxDistance: { min: Infinity, max: 0 },
  })

  useEffect(() => {
    const calibrations = keys.map((key) => key.calibration)
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
        const key = keys.find(({ row, column }) => keyCoord.row === row && keyCoord.col === column)
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
              keyItem={key}
              legends={keyCoord.ref.split(' ')}
              mode="IDLE_HEATMAP"
              calibrationRange={calibrationRange}
            />
          </div>
        )
      })}
    </div>
  )
}

type KeyProps = {
  keyItem?: Key
  mode: VisualizationMode
  calibrationRange: CalibrationRange
  legends: string[]
}
function Key({ mode, calibrationRange, legends, keyItem: key }: KeyProps) {
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
      className={`relative w-full h-full rounded-md ${color} flex flex-col justify-center overflow-hidden items-center [text-shadow:0_0_3px_rgba(0,0,0,0.9)] transition-transform`}
    >
      {key && (
        <div
          className={`absolute bottom-0 left-0 right-0 ${
            key.actuation.status !== 'STATUS_RESET' &&
            key.actuation.status !== 'STATUS_RAPID_TRIGGER_RESET'
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
