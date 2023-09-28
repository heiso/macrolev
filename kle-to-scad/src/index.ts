import json from '../layout.json'
import { writeFileSync, truncateSync, existsSync, mkdirSync } from 'fs'

const generateSupport = process.argv[2] === 'support' ? true : false
const OUT_DIR = './output/'
const OUT_FILE = generateSupport ? 'support.scad' : 'plate.scad'

const higlightedRef: string[] = []
// const higlightedRef: string[] = ['↩︎', '']

type KLERawData = Array<Record<string, string | boolean | number> | KLERawDataKey[]>
type KLERawDataKey = string | KLERawDataKeyConfig
type KLERawDataKeyConfig =
  | {
      x?: number
      x2?: number
      y?: number
      y2?: number
      w?: number
      w2?: number
      h?: number
      h2?: number
    }
  | Record<string, number>

type NormalizedKLEKey = {
  ref: string
  x: number
  y: number
  w: number
  h: number
  secondRect?: NormalizedKLEKey
}

function getNormalizedKLEKeys(keyMatrix: KLERawData): NormalizedKLEKey[] {
  const defaultKeyConfig = { ref: '', x: 0, y: 0, w: 1, h: 1 }

  return keyMatrix.reduce<NormalizedKLEKey[]>((acc, row, rowIndex) => {
    if (Array.isArray(row)) {
      let lastKeyConfig: NormalizedKLEKey | undefined
      let keyConfig: NormalizedKLEKey | undefined

      row.forEach((key) => {
        if (!keyConfig) {
          keyConfig = { ...defaultKeyConfig, y: rowIndex }
        }

        if (typeof key !== 'string') {
          keyConfig = {
            ...keyConfig,
            x: key.x || defaultKeyConfig.x,
            y: rowIndex + (key.y ?? defaultKeyConfig.y),
            w: key.w || defaultKeyConfig.w,
            h: key.h || defaultKeyConfig.h,
          }

          if (key.x2 || key.y2 || key.w2 || key.h2) {
            keyConfig.secondRect = {
              ...defaultKeyConfig,
              x: key.x2 ?? 0,
              y: keyConfig.y + (key.y2 ?? 0),
              w: key.w2 || keyConfig.w,
              h: key.h2 || keyConfig.h,
            }
          }
        } else {
          keyConfig = {
            ...keyConfig,
            ref: key.split('\n').filter(Boolean).join(' '),
            x: (lastKeyConfig ? lastKeyConfig.x + lastKeyConfig.w : 0) + keyConfig.x,
            y: -keyConfig.y - keyConfig.h,
          }

          if (keyConfig.secondRect) {
            keyConfig.secondRect.x = keyConfig.x + keyConfig.secondRect.x
            keyConfig.secondRect.y = -keyConfig.secondRect.y - keyConfig.secondRect.h
          }

          acc.push(keyConfig)

          lastKeyConfig = { ...keyConfig }
          keyConfig = undefined
        }
        return acc
      })
    }
    return acc
  }, [])
}

function getKeysWithScadCoordinates(keys: NormalizedKLEKey[]): NormalizedKLEKey[] {
  const yOffset = keys
    .map(({ y, secondRect }) => (y < (secondRect?.y || 0) ? y : secondRect?.y || 0))
    .sort((a, b) => a - b)[0]

  return keys.map((key) => {
    return {
      ...key,
      x: key.x,
      y: key.y + Math.abs(yOffset),
      w: key.w,
      h: key.h,
      secondRect: key.secondRect && {
        ...key.secondRect,
        x: key.secondRect.x,
        y: key.secondRect.y + Math.abs(yOffset),
        w: key.secondRect.w,
        h: key.secondRect.h,
      },
    }
  })
}

function hole(key: NormalizedKLEKey) {
  return `hole(x = ${key.x}, y = ${key.y}, w = ${key.w}, h = ${key.h}, is_support = ${generateSupport});`
}

function getSize(keys: NormalizedKLEKey[]) {
  const maxXKey = [
    ...keys,
    ...(keys.filter((key) => key.secondRect).map((key) => key.secondRect) as NormalizedKLEKey[]),
  ].sort((a, b) => b.x + b.w - (a.x + a.w))[0]
  const maxYKey = [...keys].sort((a, b) => b.y - a.y)[0]

  return { x: maxXKey.x + maxXKey.w, y: maxYKey.y + maxYKey.h }
}

const keys = getKeysWithScadCoordinates(getNormalizedKLEKeys(json))

if (higlightedRef.length) {
  console.log(keys.filter(({ ref }) => higlightedRef.includes(ref)))
}

const size = getSize(keys)

const out = `
include <../src/utils.scad>

plate(${size.x}, ${size.y}) {
  ${keys.map(hole).join('\n')}
}`

if (!existsSync(OUT_DIR)) {
  mkdirSync(OUT_DIR)
}
if (existsSync(OUT_DIR + OUT_FILE)) {
  truncateSync(OUT_DIR + OUT_FILE)
}
writeFileSync(OUT_DIR + OUT_FILE, out)
