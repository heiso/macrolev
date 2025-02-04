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

// {adc_channel, amux_channel}
// const uint8_t channels_by_row_col[MATRIX_ROWS][MATRIX_COLS][2] = {
//     {{0, 1}, {0, 0}, {0, 13}, {0, 12}, {1, 0}, {1, 12}, {2, 1}, {2, 0}, {2, 12}, {3, 1}, {3, 0}, {3, 12}, {4, 1}, {4, 0}, {4, 13}},
//     {{0, 2}, {0, 7}, {0, 10}, {1, 2}, {1, 7}, {1, 10}, {2, 3}, {2, 7}, {2, 10}, {3, 3}, {3, 7}, {3, 11}, {4, 4}, {4, 8}, {4, 12}},
//     {{0, 3}, {0, 6}, {0, 11}, {1, 3}, {1, 6}, {1, 11}, {2, 4}, {2, 6}, {2, 11}, {3, 4}, {3, 8}, {4, 2}, {4, 5}, {4, 7}, {XXXX, XXXX}},
//     {{XXXX, XXXX}, {0, 5}, {0, 8}, {1, 1}, {1, 5}, {1, 8}, {2, 2}, {2, 5}, {2, 8}, {3, 2}, {3, 6}, {3, 9}, {4, 3}, {4, 10}, {XXXX, XXXX}},
//     {{XXXX, XXXX}, {0, 4}, {0, 9}, {1, 4}, {XXXX, XXXX}, {1, 9}, {XXXX, XXXX}, {2, 9}, {XXXX, XXXX}, {3, 5}, {3, 10}, {XXXX, XXXX}, {4, 6}, {4, 9}, {4, 11}},
// };

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
      keycodes: [],
    },
  ],
}

export type State = {
  keyConfigs: KeyConfig[]
  selectedKeys: Set<KeyConfig['id']>
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
