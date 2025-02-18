type SwitchMagneticProfile = {
  id: number
  adc_reading_by_distance: number[] // length 255, uint16 values
}

type MagnetDirection = 'NORTH_UP' | 'NORTH_DOWN'

type KeycodeType = 'NORMAL' | 'MODIFIER' | 'CONSUMER_CONTROL' | 'SPECIAL'

export type KeyConfig = {
  id: number
  /**
   * Used to determine the position of the key on the keyboard when using the web configuration tool
   */
  layout: {
    width: number
    height: number
    x: number
    y: number
  }
  hardware: {
    adcChannel: number
    muxChannel: number
    magnetDirection: MagnetDirection
    switchMagneticProfileId: number
  }
  deadzones: {
    startOffset: number
    endOffset: number
  }
  actuationDistance: number
  releaseDistance: number
  rapidTrigger: {
    isEnabled: number
    isContinuous: number
    actuationDistanceDelta: number
    releaseDistanceDelta: number
  }
  layers: {
    holdTimeBeforeActuation: number
    isAutoCapsEnabled: number
    autoCapsVelocityTreshold: number
    keycodes: {
      type: KeycodeType
      value: number
    }[]
  }[]
}

type UserConfig = {
  switchMagneticProfiles: SwitchMagneticProfile[]
  keyConfigs: KeyConfig[]
}

// type Key = {
//   browserCode: string
//   hidCode: string
//   legend: string
// }

export const HID_KEYCODES: Record<string, string> = {
  AltLeft: '0xe2',
  AltRight: '0xe6',
  ArrowDown: '0x51',
  ArrowLeft: '0x50',
  ArrowRight: '0x4f',
  ArrowUp: '0x52',
  Backquote: '0x35',
  Backslash: '0x31',
  Backspace: '0x2a',
  BracketLeft: '0x2f',
  BracketRight: '0x30',
  CapsLock: '0x39',
  Comma: '0x36',
  ControlLeft: '0xe0',
  Delete: '0x4c',
  Digit0: '0x27',
  Digit1: '0x1e',
  Digit2: '0x1f',
  Digit3: '0x20',
  Digit4: '0x21',
  Digit5: '0x22',
  Digit6: '0x23',
  Digit7: '0x24',
  Digit8: '0x25',
  Digit9: '0x26',
  End: '0x4d',
  Enter: '0x28',
  Equal: '0x2e',
  Escape: '0x29',
  F1: '0x3a',
  F2: '0x3b',
  F3: '0x3c',
  F4: '0x3d',
  F5: '0x3e',
  F6: '0x3f',
  F7: '0x40',
  F8: '0x41',
  F9: '0x42',
  F10: '0x43',
  F11: '0x44',
  F12: '0x45',
  Home: '0x4a',
  IntlBackslash: '0x31',
  KeyA: '0x04',
  KeyB: '0x05',
  KeyC: '0x06',
  KeyD: '0x07',
  KeyE: '0x08',
  KeyF: '0x09',
  KeyG: '0x0a',
  KeyH: '0x0b',
  KeyI: '0x0c',
  KeyJ: '0x0d',
  KeyK: '0x0e',
  KeyL: '0x0f',
  KeyM: '0x10',
  KeyN: '0x11',
  KeyO: '0x12',
  KeyP: '0x13',
  KeyQ: '0x14',
  KeyR: '0x15',
  KeyS: '0x16',
  KeyT: '0x17',
  KeyU: '0x18',
  KeyV: '0x19',
  KeyW: '0x1a',
  KeyX: '0x1b',
  KeyY: '0x1c',
  KeyZ: '0x1d',
  MetaLeft: '0xe3',
  MetaRight: '0xe7',
  Minus: '0x2d',
  NumpadEnter: '0x58',
  PageDown: '0x4e',
  PageUp: '0x4b',
  Period: '0x37',
  Quote: '0x34',
  Semicolon: '0x33',
  ShiftLeft: '0xe1',
  ShiftRight: '0xe5',
  Slash: '0x38',
  Space: '0x2c',
  Tab: '0x2b',
}

const STEP = 0.25

export const defaultKeyConfig: KeyConfig = {
  id: 0,
  layout: {
    width: 1,
    height: 1,
    x: 0,
    y: 0,
  },
  hardware: {
    adcChannel: 0,
    muxChannel: 0,
    magnetDirection: 'NORTH_UP',
    switchMagneticProfileId: 0,
  },
  deadzones: {
    startOffset: 0,
    endOffset: 0,
  },
  actuationDistance: 0,
  releaseDistance: 0,
  rapidTrigger: {
    isEnabled: 0,
    isContinuous: 0,
    actuationDistanceDelta: 0,
    releaseDistanceDelta: 0,
  },
  layers: [
    {
      holdTimeBeforeActuation: 0,
      isAutoCapsEnabled: 0,
      autoCapsVelocityTreshold: 0,
      keycodes: [{ type: 'NORMAL', value: 0 }],
    },
  ],
}

export type State = {
  keyConfigs: KeyConfig[]
  selectedKeys: Set<KeyConfig['id']>
  selectedLayer: number
  history: State[]
  currentHistoryIndex: number
  width: number
  height: number
  mode: 'buildLayout' | 'buildChannels' | 'configKeymap' | 'configActuation' | 'configDebug'
}

export function initHistory(state: State): State {
  const { width, height } = state.keyConfigs.reduce(
    (acc, key) => ({
      width: Math.max(acc.width, key.layout.x + key.layout.width),
      height: Math.max(acc.height, key.layout.y + key.layout.height),
    }),
    { width: 1, height: 1 },
  )
  return {
    ...state,
    width,
    height,
    history: [{ ...state, width, height, history: [] }],
  }
}

export function addToHistory(state: State): State {
  const newHistory = [
    ...state.history.slice(0, state.currentHistoryIndex + 1),
    { ...state, history: [] },
  ]

  return {
    ...state,
    history: newHistory,
    currentHistoryIndex: newHistory.length - 1,
  }
}

export function undo(state: State): State {
  const newHistoryIndex = state.currentHistoryIndex - 1

  if (0 <= newHistoryIndex && newHistoryIndex < state.history.length) {
    return {
      ...state.history[newHistoryIndex],
      history: state.history,
      currentHistoryIndex: newHistoryIndex,
    }
  }
  return state
}

export function redo(state: State): State {
  const newHistoryIndex = state.currentHistoryIndex + 1

  if (0 <= newHistoryIndex && newHistoryIndex < state.history.length) {
    return {
      ...state.history[newHistoryIndex],
      history: state.history,
      currentHistoryIndex: newHistoryIndex,
    }
  }
  return state
}

export function unselectAll(state: State): State {
  return {
    ...state,
    selectedKeys: new Set<KeyConfig['id']>(),
  }
}

export function selectAll(state: State): State {
  return {
    ...state,
    selectedKeys: new Set<KeyConfig['id']>(state.keyConfigs.map(({ id }) => id)),
  }
}

function fixCoordinates(state: State): State {
  const { xOffset, yOffset, width, height } = state.keyConfigs.reduce(
    (acc, key) => {
      return {
        xOffset: Math.min(acc.xOffset, key.layout.x),
        yOffset: Math.min(acc.yOffset, key.layout.y),
        width: Math.max(acc.width, key.layout.x + key.layout.width),
        height: Math.max(acc.height, key.layout.y + key.layout.height),
      }
    },
    {
      xOffset: state.keyConfigs[0]?.layout.x || 0,
      yOffset: state.keyConfigs[0]?.layout.y || 0,
      width: 1,
      height: 1,
    },
  )

  return {
    ...state,
    width,
    height,
    keyConfigs: state.keyConfigs.map((key) => ({
      ...key,
      layout: {
        ...key.layout,
        x: key.layout.x - xOffset,
        y: key.layout.y - yOffset,
      },
    })),
  }
}

function fixSize(state: State): State {
  const { width, height } = state.keyConfigs.reduce(
    (acc, key) => {
      return {
        width: Math.max(acc.width, key.layout.x + key.layout.width),
        height: Math.max(acc.height, key.layout.y + key.layout.height),
      }
    },
    {
      width: 1,
      height: 1,
    },
  )

  return {
    ...state,
    width,
    height,
  }
}

export function moveKeys(
  state: State,
  direction: 'top' | 'right' | 'bottom' | 'left',
  stepMultiplier = 1,
): State {
  const axis: Extract<keyof KeyConfig['layout'], 'x' | 'y'> =
    direction === 'left' || direction === 'right' ? 'x' : 'y'
  const modifier = direction === 'left' || direction === 'top' ? -1 : +1

  return addToHistory(
    fixSize(
      fixCoordinates({
        ...state,
        keyConfigs: state.keyConfigs.map((key) => {
          if (state.selectedKeys.has(key.id)) {
            return {
              ...key,
              layout: {
                ...key.layout,
                [axis]: key.layout[axis] + modifier * STEP * stepMultiplier,
              },
            }
          }
          return key
        }),
      }),
    ),
  )
}

export function deleteKeys(state: State): State {
  return addToHistory(
    fixSize(
      fixCoordinates(
        unselectAll({
          ...state,
          keyConfigs: state.keyConfigs.filter(({ id }) => !state.selectedKeys.has(id)),
        }),
      ),
    ),
  )
}

export function resizeKey(
  state: State,
  clickedKey: KeyConfig,
  axis: 'x' | 'y',
  smaller?: boolean,
): State {
  return addToHistory(
    fixSize(
      fixCoordinates({
        ...state,
        keyConfigs: state.keyConfigs.map((key) => {
          if (key.id === clickedKey.id) {
            if (axis === 'x') {
              return {
                ...key,
                layout: {
                  ...key.layout,
                  width: Math.max(smaller ? key.layout.width - STEP : key.layout.width + STEP, 1),
                },
              }
            }

            return {
              ...key,
              layout: {
                ...key.layout,
                height: Math.max(smaller ? key.layout.height - STEP : key.layout.height + STEP, 1),
              },
            }
          }
          return key
        }),
      }),
    ),
  )
}

export function addFirstKey(state: State): State {
  return fixSize(
    fixCoordinates(
      unselectAll({
        ...state,
        keyConfigs: [
          {
            ...defaultKeyConfig,
            id: state.keyConfigs.length,
          },
        ],
      }),
    ),
  )
}

export function addKey(
  state: State,
  clickedKey: KeyConfig,
  direction: 'top' | 'right' | 'bottom' | 'left',
): State {
  const lastId = state.keyConfigs.reduce(
    (acc, key) => Math.max(acc, key.id),
    state.keyConfigs[0]?.id || 0,
  )
  return addToHistory(
    fixSize(
      fixCoordinates(
        unselectAll({
          ...state,
          keyConfigs: [
            ...state.keyConfigs,
            {
              ...defaultKeyConfig,
              id: lastId + 1,
              layout: {
                height: clickedKey.layout.height,
                width: clickedKey.layout.width,
                x:
                  direction === 'left'
                    ? clickedKey.layout.x - clickedKey.layout.width
                    : direction === 'right'
                      ? clickedKey.layout.x + clickedKey.layout.width
                      : clickedKey.layout.x,
                y:
                  direction === 'top'
                    ? clickedKey.layout.y - clickedKey.layout.height
                    : direction === 'bottom'
                      ? clickedKey.layout.y + clickedKey.layout.height
                      : clickedKey.layout.y,
              },
            },
          ],
        }),
      ),
    ),
  )
}

export function selectKey(state: State, clickedKey: KeyConfig): State {
  return addToHistory({
    ...state,
    selectedKeys:
      state.selectedKeys.size === 1 && state.selectedKeys.has(clickedKey.id)
        ? new Set<KeyConfig['id']>()
        : new Set<KeyConfig['id']>([clickedKey.id]),
  })
}

export function selectRangeOfKeys(state: State, clickedKey: KeyConfig): State {
  if (state.selectedKeys.size === 0) {
    return state
  }

  const mostUpLeftSelectedKey = state.keyConfigs
    .filter(({ id }) => state.selectedKeys.has(id))
    .sort((a, b) => a.layout.x - b.layout.x || a.layout.y - b.layout.y)[0]

  return addToHistory({
    ...state,
    selectedKeys: state.keyConfigs.reduce((acc, key) => {
      if (
        ((mostUpLeftSelectedKey.layout.x <= key.layout.x && key.layout.x <= clickedKey.layout.x) ||
          (mostUpLeftSelectedKey.layout.x >= key.layout.x &&
            key.layout.x >= clickedKey.layout.x)) &&
        ((mostUpLeftSelectedKey.layout.y <= key.layout.y && key.layout.y <= clickedKey.layout.y) ||
          (mostUpLeftSelectedKey.layout.y >= key.layout.y && key.layout.y >= clickedKey.layout.y))
      ) {
        acc.add(key.id)
      }
      return acc
    }, new Set<KeyConfig['id']>()),
  })
}

export function addToKeySelection(state: State, clickedKey: KeyConfig): State {
  return addToHistory({
    ...state,
    selectedKeys: state.selectedKeys.has(clickedKey.id)
      ? new Set<KeyConfig['id']>([...state.selectedKeys].filter((id) => clickedKey.id !== id))
      : new Set<KeyConfig['id']>([...state.selectedKeys, clickedKey.id]),
  })
}

function isRectangleInCollisionWithArea(
  rect: { x: number; y: number; width: number; height: number },
  area: { x: number; y: number; width: number; height: number },
) {
  return !(
    rect.x + rect.width <= area.x || // rect is to the left of area
    rect.x >= area.x + area.width || // rect is to the right of area
    rect.y + rect.height <= area.y || // rect is above area
    rect.y >= area.y + area.height // rect is below area
  )
}

export function canAdd(
  state: State,
  key: KeyConfig,
  direction: 'top' | 'right' | 'bottom' | 'left',
) {
  const keyInZone = state.keyConfigs.find(({ id, layout }) => {
    if (id === key.id) return false

    const { x, y, width, height } = layout

    if (direction === 'top') {
      return isRectangleInCollisionWithArea(
        { x, y, width, height },
        {
          x: key.layout.x,
          y: key.layout.y - key.layout.height,
          width: key.layout.width,
          height: key.layout.height,
        },
      )
    } else if (direction === 'right') {
      return isRectangleInCollisionWithArea(
        { x, y, width, height },
        {
          x: key.layout.x,
          y: key.layout.y,
          width: key.layout.width + key.layout.width,
          height: key.layout.height,
        },
      )
    } else if (direction === 'bottom') {
      return isRectangleInCollisionWithArea(
        { x, y, width, height },
        {
          x: key.layout.x,
          y: key.layout.y,
          width: key.layout.width,
          height: key.layout.height + key.layout.height,
        },
      )
    } else if (direction === 'left') {
      return isRectangleInCollisionWithArea(
        { x, y, width, height },
        {
          x: key.layout.x - key.layout.width,
          y: key.layout.y,
          width: key.layout.width,
          height: key.layout.height,
        },
      )
    }
  })
  return !keyInZone
}

export function setAdcChannelForSelectedKeys(state: State, adcChannel: number): State {
  return addToHistory({
    ...state,
    keyConfigs: [
      ...state.keyConfigs.filter(({ id }) => !state.selectedKeys.has(id)),
      ...state.keyConfigs
        .filter(({ id }) => state.selectedKeys.has(id))
        .map((key) => ({
          ...key,
          hardware: { ...key.hardware, adcChannel },
        })),
    ],
  })
}

export function setMuxChannelForSelectedKeys(state: State, muxChannel: number): State {
  return addToHistory({
    ...state,
    keyConfigs: [
      ...state.keyConfigs.filter(({ id }) => !state.selectedKeys.has(id)),
      ...state.keyConfigs
        .filter(({ id }) => state.selectedKeys.has(id))
        .map((key) => ({
          ...key,
          hardware: { ...key.hardware, muxChannel },
        })),
    ],
  })
}

export function setKeycodeForSelectedKeys(state: State, layer: KeyConfig['layers'][number]): State {
  return addToHistory({
    ...state,
    keyConfigs: [
      ...state.keyConfigs.filter(({ id }) => !state.selectedKeys.has(id)),
      ...state.keyConfigs
        .filter(({ id }) => state.selectedKeys.has(id))
        .map((key) => ({
          ...key,
          layers: key.layers.map((oldLayer, index) =>
            index === state.selectedLayer ? layer : oldLayer,
          ),
        })),
    ],
  })
}
