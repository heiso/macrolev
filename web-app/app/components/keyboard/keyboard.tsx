import {
  useEffect,
  useRef,
  useState,
  type Dispatch,
  type HTMLAttributes,
  type PropsWithChildren,
  type SetStateAction,
} from 'react'
import {
  addFirstKey,
  addKey,
  addToKeySelection,
  canAdd,
  deleteKeys,
  moveKeys,
  redo,
  resizeKey,
  selectAll,
  selectKey,
  selectRangeOfKeys,
  undo,
  unselectAll,
  type KeyConfig,
  type State,
} from './keyboard-state'

const MAX_MULTIPLIER = 80

type KeyboardProps = {
  state: State
  setState: Dispatch<SetStateAction<State>>
}
export function Keyboard({ state, setState }: KeyboardProps) {
  const containerRef = useRef<HTMLDivElement>(null)
  const keyboardRef = useRef<HTMLDivElement>(null)
  const [scaleFactor, setScaleFactor] = useState(1)
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
          if (state.mode === 'buildLayout') {
            setState((state) => moveKeys(state, 'right', isShiftPressed ? 4 : 1))
          }
          break

        case 'ArrowLeft':
          if (state.mode === 'buildLayout') {
            setState((state) => moveKeys(state, 'left', isShiftPressed ? 4 : 1))
          }
          break

        case 'ArrowUp':
          if (state.mode === 'buildLayout') {
            setState((state) => moveKeys(state, 'top', isShiftPressed ? 4 : 1))
          }
          break

        case 'ArrowDown':
          if (state.mode === 'buildLayout') {
            setState((state) => moveKeys(state, 'bottom', isShiftPressed ? 4 : 1))
          }
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
          if (state.mode === 'buildLayout') {
            setState((state) => deleteKeys(state))
          }
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
      if (containerRef.current && state.width) {
        setScaleFactor(
          Math.min(
            containerRef.current.getBoundingClientRect().width / state.width,
            MAX_MULTIPLIER,
          ),
        )
      }
    }
    window.addEventListener('resize', updateMultiplier)
    updateMultiplier()
    return () => window.removeEventListener('resize', updateMultiplier)
  }, [state.width])

  useEffect(() => {
    function handleClickOutside(event: MouseEvent) {
      if (
        state.mode === 'buildLayout' &&
        keyboardRef.current &&
        !keyboardRef.current.contains(event.target as Node)
      ) {
        setState((state) => unselectAll(state))
      }
    }

    document.addEventListener('mousedown', handleClickOutside)
    return () => {
      document.removeEventListener('mousedown', handleClickOutside)
    }
  }, [keyboardRef])

  return (
    <div className="flex w-full justify-center" ref={containerRef}>
      <div className="flex flex-col w-fit gap-4">
        <div className="bg-slate-700 rounded-lg p-4">
          <div
            className="relative transition-all duration-150"
            style={{ height: state.height * scaleFactor, width: state.width * scaleFactor }}
            ref={keyboardRef}
          >
            {state.keyConfigs.length === 0 && state.mode === 'buildLayout' && (
              <div className="w-full h-full" style={{ padding: (1 * scaleFactor) / 17 }}>
                <AddKey
                  style={{ borderRadius: scaleFactor * 0.15 }}
                  onClick={() => setState((state) => addFirstKey(state))}
                />
              </div>
            )}
            {state.keyConfigs.map((key) => (
              <div
                key={key.id}
                className={`group absolute transition-all duration-150 hover:z-40 ${state.selectedKeys.has(key.id) ? 'z-40' : ''}`}
                style={{
                  width: key.layout.width * scaleFactor,
                  height: key.layout.height * scaleFactor,
                  top: key.layout.y * scaleFactor,
                  left: key.layout.x * scaleFactor,
                  padding: (1 * scaleFactor) / 17,
                }}
              >
                {state.mode === 'buildLayout' && (
                  <AddKeyController
                    state={state}
                    keyConfig={key}
                    scaleFactor={scaleFactor}
                    setState={setState}
                  />
                )}

                <div
                  className={`flex w-full h-full rounded-sm sm:rounded-md group-hover:border-pink-500 group-hover:border-2 overflow-hidden items-center justify-center cursor-pointer select-none active:scale-110 ${state.selectedKeys.has(key.id) ? 'bg-pink-500' : 'bg-slate-500'}`}
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
                  <div
                    className={`${state.selectedKeys.has(key.id) ? 'text-pink-100' : 'text-slate-300'}`}
                  >
                    {state.mode === 'buildLayout' ? (
                      key.layout.width > 1 && <>{key.layout.width}u</>
                    ) : state.mode === 'buildChannels' ? (
                      <>
                        {key.hardware.adcChannel}:{key.hardware.muxChannel}
                      </>
                    ) : (
                      <>nope</>
                    )}
                  </div>
                </div>

                {state.mode === 'buildLayout' && (
                  <ResizeKeyController keyConfig={key} setState={setState} />
                )}
              </div>
            ))}
          </div>
        </div>
      </div>
    </div>
  )
}

type AddKeyControllerProps = {
  state: State
  keyConfig: KeyConfig
  scaleFactor: number
  setState: Dispatch<SetStateAction<State>>
}
function AddKeyController({ state, keyConfig, scaleFactor, setState }: AddKeyControllerProps) {
  return (
    <>
      {canAdd(state, keyConfig, 'top') && (
        <div
          className="hidden z-20 group-hover:flex absolute w-full h-full -top-full left-0 justify-center items-end"
          style={{ padding: (1 * scaleFactor) / 17 }}
        >
          <AddKey
            onClick={() => setState((state) => addKey(state, keyConfig, 'top'))}
            style={{ borderRadius: scaleFactor * 0.15 }}
          />
        </div>
      )}

      {canAdd(state, keyConfig, 'right') && (
        <div
          className="hidden z-20 group-hover:flex absolute w-full h-full -right-full top-0 justify-start items-center"
          style={{ padding: (1 * scaleFactor) / 17 }}
        >
          <AddKey
            onClick={() => setState((state) => addKey(state, keyConfig, 'right'))}
            style={{ borderRadius: scaleFactor * 0.15 }}
          />
        </div>
      )}

      {canAdd(state, keyConfig, 'bottom') && (
        <div
          className="hidden z-20 group-hover:flex absolute w-full h-full -bottom-full left-0 justify-center items-start"
          style={{ padding: (1 * scaleFactor) / 17 }}
        >
          <AddKey
            onClick={() => setState((state) => addKey(state, keyConfig, 'bottom'))}
            style={{ borderRadius: scaleFactor * 0.15 }}
          />
        </div>
      )}

      {canAdd(state, keyConfig, 'left') && (
        <div
          className="hidden z-20 group-hover:flex absolute w-full h-full -left-full top-0 justify-end items-center"
          style={{ padding: (1 * scaleFactor) / 17 }}
        >
          <AddKey
            onClick={() => setState((state) => addKey(state, keyConfig, 'left'))}
            style={{ borderRadius: scaleFactor * 0.15 }}
          />
        </div>
      )}
    </>
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

type ResizeKeyControllerProps = {
  keyConfig: KeyConfig
  setState: Dispatch<SetStateAction<State>>
}
function ResizeKeyController({ keyConfig, setState }: ResizeKeyControllerProps) {
  return (
    <>
      <div
        className="hidden group-hover:flex absolute z-40 hover:bg-pink-400 w-6 h-4 bg-pink-500 cursor-pointer -top-3 text-xs left-1/2 -ml-3 rounded-t-lg select-none justify-center items-center active:scale-110"
        onClick={(event) => {
          event.stopPropagation()
          setState((state) => resizeKey(state, keyConfig, 'y', true))
        }}
      >
        <div className="leading-none font-semibold">-</div>
      </div>

      <div
        className="hidden group-hover:flex absolute z-40 hover:bg-pink-400 w-4 h-6 bg-pink-500 cursor-pointer -right-3 text-xs top-1/2 -mt-3 rounded-r-lg select-none justify-center items-center active:scale-110"
        onClick={(event) => {
          event.stopPropagation()
          setState((state) => resizeKey(state, keyConfig, 'x'))
        }}
      >
        <div className="leading-none font-semibold">+</div>
      </div>

      <div
        className="hidden group-hover:flex absolute z-40 hover:bg-pink-400 w-6 h-4 bg-pink-500 cursor-pointer -bottom-3 text-xs left-1/2 -ml-3 rounded-b-lg select-none justify-center items-center active:scale-110"
        onClick={(event) => {
          event.stopPropagation()
          setState((state) => resizeKey(state, keyConfig, 'y'))
        }}
      >
        <div className="leading-none font-semibold">+</div>
      </div>

      <div
        className="hidden group-hover:flex absolute z-40 hover:bg-pink-400 w-4 h-6 bg-pink-500 cursor-pointer -left-3 text-xs top-1/2 -mt-3 rounded-l-lg select-none justify-center items-center activer"
        onClick={(event) => {
          event.stopPropagation()
          setState((state) => resizeKey(state, keyConfig, 'x', true))
        }}
      >
        <div className="leading-none font-semibold">-</div>
      </div>
    </>
  )
}
