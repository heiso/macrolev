import { useEffect, useRef, useState } from 'react'
import { generatePath, Link } from 'react-router'
import layout from '../layout.json'
import { paths } from '../routes'
import { Status } from '../useDevice'

type KLEKeyConfig = {
  w: number
  h: number
}

const defaultKLEKeyConfig: KLEKeyConfig = { w: 1, h: 1 }

const width = layout
  .filter((item) => Array.isArray(item))
  .reduce((acc, row) => {
    let lastW = defaultKLEKeyConfig.w

    return Math.max(
      acc,
      row.reduce((width, key) => {
        if (typeof key === 'string') {
          width = width + lastW
          lastW = defaultKLEKeyConfig.w
        } else {
          if (key.x) {
            width = width + key.x
          }
          if (key.w) {
            lastW = key.w
          }
        }
        return width
      }, 0),
    )
  }, 0)

type KeyboardProps = {
  keyMatrix?: ({
    distance: number
    status: Status
  } | null)[][]
  selectedKey?: {
    row: number
    column: number
  }
}
export function Keyboard({ keyMatrix, selectedKey }: KeyboardProps) {
  const containerRef = useRef<HTMLDivElement>(null)
  const [multiplier, setMultiplier] = useState(0)

  useEffect(() => {
    function updateMultiplier() {
      if (containerRef.current) {
        setMultiplier(containerRef.current.getBoundingClientRect().width / width)
      }
    }
    window.addEventListener('resize', updateMultiplier)
    updateMultiplier()
    return () => window.removeEventListener('resize', updateMultiplier)
  }, [])

  let rowIndex = 0

  return (
    <div className="p-2 bg-slate-700 rounded-lg w-full h-full max-w-screen-lg">
      <div className="flex flex-col w-full h-full" ref={containerRef}>
        {layout.map((row) => {
          if (!Array.isArray(row)) {
            return null
          }

          let keyConfig = defaultKLEKeyConfig
          let colIndex = 0

          const element = (
            <div key={rowIndex} className="flex w-fit">
              {row.map((key) => {
                if (typeof key !== 'string') {
                  keyConfig = { ...defaultKLEKeyConfig, ...key }
                  if (key.x) {
                    return <div style={{ width: key.x * multiplier }}></div>
                  }
                } else {
                  const element = (
                    <div
                      key={colIndex}
                      className="p-0.5"
                      style={{
                        width: keyConfig.w * multiplier,
                        height: keyConfig.h * multiplier,
                        marginBottom: keyConfig.h > 1 ? -(keyConfig.h / 2) * multiplier : 0,
                      }}
                    >
                      <Link
                        to={generatePath(paths.configurator, {
                          column: String(colIndex),
                          row: String(rowIndex),
                        })}
                        className={`group flex relative bg-slate-500 w-full h-full rounded-sm sm:rounded-md hover:bg-slate-400 overflow-hidden items-center justify-center ${selectedKey?.row === rowIndex && selectedKey?.column === colIndex} ? 'border border-slate-300' : ''`}
                      >
                        <div className="z-10 text-xs">{key}</div>
                        {keyMatrix?.[rowIndex]?.[colIndex] && (
                          <div
                            className={`w-full absolute top-0 ${keyMatrix[rowIndex][colIndex]?.status === Status.TRIGGERED ? 'bg-pink-600 group-hover:bg-pink-500' : 'bg-pink-500 group-hover:bg-pink-400'}`}
                            style={{ height: `${keyMatrix[rowIndex][colIndex]?.distance}%` }}
                          />
                        )}
                      </Link>
                    </div>
                  )

                  keyConfig = defaultKLEKeyConfig
                  colIndex = colIndex + 1

                  return element
                }
              })}
            </div>
          )

          rowIndex = rowIndex + 1

          return element
        })}
      </div>
    </div>
  )
}
