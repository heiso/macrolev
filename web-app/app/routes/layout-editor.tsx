import {
  useEffect,
  useMemo,
  useRef,
  useState,
  type HTMLAttributes,
  type PropsWithChildren,
} from 'react'
import { getNormalizedKLEKeys } from '../kle-parser'
import KLElayout from '../layout.json'

type Key = {
  id: number
  amux?: number
  amuxSelect?: number
  width: number
  height: number
  x: number
  y: number
}

type State = {
  layout: Key[]
  selectedKeys: Set<Key['id']>
  history: State[]
  currentHistoryIndex: number
}

const MAX_MULTIPLIER = 60
const STEP = 0.25

function initHistory(state: State): State {
  return {
    ...state,
    history: [{ ...state, history: [] }],
  }
}

function addToHistory(state: State): State {
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

function undo(state: State): State {
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

function redo(state: State): State {
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

function unselectAll(state: State): State {
  return {
    ...state,
    selectedKeys: new Set<Key['id']>(),
  }
}

function selectAll(state: State): State {
  return {
    ...state,
    selectedKeys: new Set<Key['id']>(state.layout.map(({ id }) => id)),
  }
}

function fixCoordinatesToZero(state: State): State {
  if (state.layout.length === 0) {
    return state
  }

  const { xOffset, yOffset } = state.layout.reduce(
    (acc, key) => {
      return {
        xOffset: Math.min(acc.xOffset, key.x),
        yOffset: Math.min(acc.yOffset, key.y),
      }
    },
    { xOffset: state.layout[0].x, yOffset: state.layout[0].y },
  )

  return {
    ...state,
    layout: state.layout.map((key) => ({
      ...key,
      x: key.x - xOffset,
      y: key.y - yOffset,
    })),
  }
}

function moveKeys(
  state: State,
  direction: 'top' | 'right' | 'bottom' | 'left',
  stepMultiplier = 1,
): State {
  const axis: Extract<keyof Key, 'x' | 'y'> =
    direction === 'left' || direction === 'right' ? 'x' : 'y'
  const modifier = direction === 'left' || direction === 'top' ? -1 : +1

  return addToHistory(
    fixCoordinatesToZero({
      ...state,
      layout: state.layout.map((key) => {
        if (state.selectedKeys.has(key.id)) {
          return {
            ...key,
            [axis]: key[axis] + modifier * STEP * stepMultiplier,
          }
        }
        return key
      }),
    }),
  )
}

function deleteKeys(state: State): State {
  return addToHistory(
    fixCoordinatesToZero(
      unselectAll({
        ...state,
        layout: state.layout.filter(({ id }) => !state.selectedKeys.has(id)),
      }),
    ),
  )
}

function resizeKey(state: State, clickedKey: Key, axis: 'x' | 'y', smaller?: boolean): State {
  return addToHistory(
    fixCoordinatesToZero({
      ...state,
      layout: state.layout.map((key) => {
        if (key.id === clickedKey.id) {
          if (axis === 'x') {
            return {
              ...key,
              width: Math.max(smaller ? key.width - STEP : key.width + STEP, 1),
            }
          }

          return {
            ...key,
            height: Math.max(smaller ? key.height - STEP : key.height + STEP, 1),
          }
        }
        return key
      }),
    }),
  )
}

function addFirstKey(state: State): State {
  return fixCoordinatesToZero(
    unselectAll({
      ...state,
      layout: [
        {
          id: state.layout.length,
          height: 1,
          width: 1,
          x: 0,
          y: 0,
        },
      ],
    }),
  )
}

function addKey(
  state: State,
  clickedKey: Key,
  direction: 'top' | 'right' | 'bottom' | 'left',
): State {
  return addToHistory(
    fixCoordinatesToZero(
      unselectAll({
        ...state,
        layout: [
          ...state.layout,
          {
            id: state.layout.length,
            height: clickedKey.height,
            width: clickedKey.width,
            x:
              direction === 'left'
                ? clickedKey.x - clickedKey.width
                : direction === 'right'
                  ? clickedKey.x + clickedKey.width
                  : clickedKey.x,
            y:
              direction === 'top'
                ? clickedKey.y - clickedKey.height
                : direction === 'bottom'
                  ? clickedKey.y + clickedKey.height
                  : clickedKey.y,
          },
        ],
      }),
    ),
  )
}

function selectKey(state: State, clickedKey: Key): State {
  return addToHistory({
    ...state,
    selectedKeys:
      state.selectedKeys.size === 1 && state.selectedKeys.has(clickedKey.id)
        ? new Set<Key['id']>()
        : new Set<Key['id']>([clickedKey.id]),
  })
}

function selectRangeOfKeys(state: State, clickedKey: Key): State {
  if (state.selectedKeys.size === 0) {
    return state
  }

  const mostUpLeftSelectedKey = state.layout
    .filter(({ id }) => state.selectedKeys.has(id))
    .sort((a, b) => a.x - b.x || a.y - b.y)[0]

  return addToHistory({
    ...state,
    selectedKeys: state.layout.reduce((acc, key) => {
      if (
        ((mostUpLeftSelectedKey.x <= key.x && key.x <= clickedKey.x) ||
          (mostUpLeftSelectedKey.x >= key.x && key.x >= clickedKey.x)) &&
        ((mostUpLeftSelectedKey.y <= key.y && key.y <= clickedKey.y) ||
          (mostUpLeftSelectedKey.y >= key.y && key.y >= clickedKey.y))
      ) {
        acc.add(key.id)
      }
      return acc
    }, new Set<Key['id']>()),
  })
}

function addToKeySelection(state: State, clickedKey: Key): State {
  return addToHistory({
    ...state,
    selectedKeys: new Set<Key['id']>([...state.selectedKeys, clickedKey.id]),
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

function canAdd(state: State, key: Key, direction: 'top' | 'right' | 'bottom' | 'left') {
  const keyInZone = state.layout.find(({ id, height, y, x, width }) => {
    if (id === key.id) return false

    if (direction === 'top') {
      return isRectangleInCollisionWithArea(
        { x, y, width, height },
        { x: key.x, y: key.y - key.height, width: key.width, height: key.height },
      )
    } else if (direction === 'right') {
      return isRectangleInCollisionWithArea(
        { x, y, width, height },
        { x: key.x, y: key.y, width: key.width + key.width, height: key.height },
      )
    } else if (direction === 'bottom') {
      return isRectangleInCollisionWithArea(
        { x, y, width, height },
        { x: key.x, y: key.y, width: key.width, height: key.height + key.height },
      )
    } else if (direction === 'left') {
      return isRectangleInCollisionWithArea(
        { x, y, width, height },
        { x: key.x - key.width, y: key.y, width: key.width, height: key.height },
      )
    }
  })
  return !keyInZone
}

export default function Index() {
  const containerRef = useRef<HTMLDivElement>(null)
  const keyboardRef = useRef<HTMLDivElement>(null)
  const [state, setState] = useState<State>(
    initHistory({
      layout: getNormalizedKLEKeys(KLElayout).map(({ h, w, x, y }, index) => ({
        id: index,
        x,
        y,
        height: h,
        width: w,
      })),
      // layout: [{ width: 1, height: 1, x: 0, y: 0, id: 0 }],
      selectedKeys: new Set<Key['id']>(),
      history: [],
      currentHistoryIndex: 0,
    }),
  )
  const { width, height } = useMemo(
    () =>
      state.layout.reduce(
        (acc, key) => ({
          width: Math.max(acc.width, key.x + key.width),
          height: Math.max(acc.height, key.y + key.height),
        }),
        { width: 1, height: 1 },
      ),
    [state.layout],
  )
  const [scaleFactor, setMultiplier] = useState(1)
  const [isShiftPressed, setIsShiftPressed] = useState(false)
  const [isCmdPressed, setIsCmdPressed] = useState(false)

  useEffect(() => {
    function handleKeyDown(event: KeyboardEvent) {
      switch (event.key) {
        case 'Control':
        case 'Alt':
        case 'Meta':
          if (!isCmdPressed) {
            setIsCmdPressed(true)
          }
          break

        case 'Shift':
          if (!isShiftPressed) {
            setIsShiftPressed(true)
          }
          break

        case 'ArrowRight':
          setState((state) => moveKeys(state, 'right', isShiftPressed ? 4 : 1))
          break

        case 'ArrowLeft':
          setState((state) => moveKeys(state, 'left', isShiftPressed ? 4 : 1))
          break

        case 'ArrowUp':
          setState((state) => moveKeys(state, 'top', isShiftPressed ? 4 : 1))
          break

        case 'ArrowDown':
          setState((state) => moveKeys(state, 'bottom', isShiftPressed ? 4 : 1))
          break

        case 'z':
          if (isCmdPressed) {
            setState((state) => undo(state))
          }
          break

        case 'Z':
          if (isCmdPressed) {
            setState((state) => redo(state))
          }
          break

        case 'a':
          if (isCmdPressed) {
            event.preventDefault()
            setState((state) => selectAll(state))
          }
          break

        default:
          break
      }
    }

    function handleKeyUp(event: KeyboardEvent) {
      switch (event.key) {
        case 'Control':
        case 'Alt':
        case 'Meta':
          if (isCmdPressed) {
            setIsCmdPressed(false)
          }
          break

        case 'Shift':
          if (isShiftPressed) {
            setIsShiftPressed(false)
          }
          break

        case 'Backspace':
        case 'Delete':
          setState((state) => deleteKeys(state))
          break

        case 'Escape':
          setState((state) => unselectAll(state))
          break

        default:
          break
      }
    }

    window.addEventListener('keydown', handleKeyDown)
    window.addEventListener('keyup', handleKeyUp)
    return () => {
      window.removeEventListener('keydown', handleKeyDown)
      window.removeEventListener('keyup', handleKeyUp)
    }
  }, [state, isShiftPressed, isCmdPressed])

  useEffect(() => {
    function updateMultiplier() {
      if (containerRef.current && width) {
        setMultiplier(
          Math.min(containerRef.current.getBoundingClientRect().width / width, MAX_MULTIPLIER),
        )
      }
    }
    window.addEventListener('resize', updateMultiplier)
    updateMultiplier()
    return () => window.removeEventListener('resize', updateMultiplier)
  }, [width])

  useEffect(() => {
    function handleClickOutside(event: MouseEvent) {
      if (keyboardRef.current && !keyboardRef.current.contains(event.target as Node)) {
        setState((state) => unselectAll(state))
      }
    }

    document.addEventListener('mousedown', handleClickOutside)
    return () => {
      document.removeEventListener('mousedown', handleClickOutside)
    }
  }, [keyboardRef])

  return (
    <div className="p-4 sm:p-8 flex flex-col items-center gap-8">
      <div className="flex w-full justify-center" ref={containerRef}>
        <div className="bg-slate-700 rounded-lg p-4">
          <div
            className="relative transition-all duration-150"
            style={{ height: height * scaleFactor, width: width * scaleFactor }}
            ref={keyboardRef}
          >
            {state.layout.length === 0 && (
              <div className="w-full h-full" style={{ padding: (1 * scaleFactor) / 17 }}>
                <AddKey
                  style={{ borderRadius: scaleFactor * 0.15 }}
                  onClick={() => setState((state) => addFirstKey(state))}
                />
              </div>
            )}
            {state.layout.map((key) => (
              <div
                key={key.id}
                className={`group absolute transition-all duration-150 hover:z-40 ${state.selectedKeys.has(key.id) ? 'z-40' : ''}`}
                style={{
                  width: key.width * scaleFactor,
                  height: key.height * scaleFactor,
                  top: key.y * scaleFactor,
                  left: key.x * scaleFactor,
                  padding: (1 * scaleFactor) / 17,
                }}
              >
                {canAdd(state, key, 'top') && (
                  <div
                    className="hidden z-20 group-hover:flex absolute w-full h-full -top-full left-0 justify-center items-end"
                    style={{ padding: (1 * scaleFactor) / 17 }}
                  >
                    <AddKey
                      onClick={() => setState((state) => addKey(state, key, 'top'))}
                      style={{ borderRadius: scaleFactor * 0.15 }}
                    />
                  </div>
                )}

                {canAdd(state, key, 'right') && (
                  <div
                    className="hidden z-20 group-hover:flex absolute w-full h-full -right-full top-0 justify-start items-center"
                    style={{ padding: (1 * scaleFactor) / 17 }}
                  >
                    <AddKey
                      onClick={() => setState((state) => addKey(state, key, 'right'))}
                      style={{ borderRadius: scaleFactor * 0.15 }}
                    />
                  </div>
                )}

                {canAdd(state, key, 'bottom') && (
                  <div
                    className="hidden z-20 group-hover:flex absolute w-full h-full -bottom-full left-0 justify-center items-start"
                    style={{ padding: (1 * scaleFactor) / 17 }}
                  >
                    <AddKey
                      onClick={() => setState((state) => addKey(state, key, 'bottom'))}
                      style={{ borderRadius: scaleFactor * 0.15 }}
                    />
                  </div>
                )}

                {canAdd(state, key, 'left') && (
                  <div
                    className="hidden z-20 group-hover:flex absolute w-full h-full -left-full top-0 justify-end items-center"
                    style={{ padding: (1 * scaleFactor) / 17 }}
                  >
                    <AddKey
                      onClick={() => setState((state) => addKey(state, key, 'left'))}
                      style={{ borderRadius: scaleFactor * 0.15 }}
                    />
                  </div>
                )}

                <div
                  className={`flex w-full h-full rounded-sm sm:rounded-md group-hover:border-2 group-hover:border-pink-500 overflow-hidden items-center justify-center cursor-pointer select-none transition-all active:scale-110 ${state.selectedKeys.has(key.id) ? 'bg-pink-500' : 'bg-slate-500'}`}
                  style={{ borderRadius: scaleFactor * 0.15 }}
                  onClick={() =>
                    setState((state) => {
                      if (isCmdPressed) {
                        return addToKeySelection(state, key)
                      }
                      if (isShiftPressed) {
                        return selectRangeOfKeys(state, key)
                      }
                      return selectKey(state, key)
                    })
                  }
                >
                  {key.width > 1 && <div className="text-slate-300">{key.width}u</div>}
                </div>

                <div
                  className="hidden group-hover:flex absolute z-40 hover:bg-pink-400 w-6 h-4 bg-pink-500 cursor-pointer -top-3 text-xs left-1/2 -ml-3 rounded-t-lg select-none justify-center items-center active:scale-110"
                  onClick={(event) => {
                    event.stopPropagation()
                    setState((state) => resizeKey(state, key, 'y', true))
                  }}
                >
                  <div className="leading-none font-semibold">-</div>
                </div>

                <div
                  className="hidden group-hover:flex absolute z-40 hover:bg-pink-400 w-4 h-6 bg-pink-500 cursor-pointer -right-3 text-xs top-1/2 -mt-3 rounded-r-lg select-none justify-center items-center active:scale-110"
                  onClick={(event) => {
                    event.stopPropagation()
                    setState((state) => resizeKey(state, key, 'x'))
                  }}
                >
                  <div className="leading-none font-semibold">+</div>
                </div>

                <div
                  className="hidden group-hover:flex absolute z-40 hover:bg-pink-400 w-6 h-4 bg-pink-500 cursor-pointer -bottom-3 text-xs left-1/2 -ml-3 rounded-b-lg select-none justify-center items-center active:scale-110"
                  onClick={(event) => {
                    event.stopPropagation()
                    setState((state) => resizeKey(state, key, 'y'))
                  }}
                >
                  <div className="leading-none font-semibold">+</div>
                </div>

                <div
                  className="hidden group-hover:flex absolute z-40 hover:bg-pink-400 w-4 h-6 bg-pink-500 cursor-pointer -left-3 text-xs top-1/2 -mt-3 rounded-l-lg select-none justify-center items-center activer"
                  onClick={(event) => {
                    event.stopPropagation()
                    setState((state) => resizeKey(state, key, 'x', true))
                  }}
                >
                  <div className="leading-none font-semibold">-</div>
                </div>
              </div>
            ))}
          </div>
        </div>
      </div>

      <div className="flex text-slate-400">
        {state.selectedKeys.size > 0 && <div>{state.selectedKeys.size} keys selected</div>}
      </div>
    </div>
  )
}

type AddKeyProps = PropsWithChildren<HTMLAttributes<HTMLDivElement>>
function AddKey({ children, ...props }: AddKeyProps) {
  return (
    <div
      {...props}
      className="flex bg-slate-500 w-full h-full rounded-sm sm:rounded-md items-center justify-center cursor-pointer select-none opacity-50 hover:opacity-95 transition-all active:scale-125 font-bold"
    >
      +
    </div>
  )
}
