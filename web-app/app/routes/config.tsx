import { useState } from 'react'
import { Link } from 'react-router'
import { Field } from '../components/field'
import { Keyboard } from '../components/keyboard/keyboard'
import {
  initHistory,
  setKeycodeForSelectedKeys,
  type KeyConfig,
  type State,
} from '../components/keyboard/keyboard-state'
import defaultLayout from '../layouts/macrolev-azerty-mac-fr.json'

export default function Index() {
  const [state, setState] = useState<State>(
    initHistory({
      keyConfigs: defaultLayout as KeyConfig[],
      selectedKeys: new Set<KeyConfig['id']>(),
      selectedLayer: 0,
      history: [],
      currentHistoryIndex: 0,
      width: 0,
      height: 0,
      mode: 'configKeymap',
    }),
  )

  // const [isListeningToKeydown, setIsListeningToKeydown] = useState(false)

  // useEffect(() => {
  //   if (isListeningToKeydown) {
  //     window.addEventListener('keydown', handleKeydown)
  //   } else {
  //     window.removeEventListener('keydown', handleKeydown)
  //   }
  //   return () => {
  //     window.removeEventListener('keydown', handleKeydown)
  //   }
  // }, [isListeningToKeydown])

  // function handleKeydown(event: KeyboardEvent) {
  //   if (state.selectedKeys.size > 0) {
  //     setState(
  //       setKeycodeForSelectedKeys(state, {
  //         autoCapsVelocityTreshold: 0,
  //         holdTimeBeforeActuation: 0,
  //         isAutoCapsEnabled: 0,
  //         keycodes: [{ type: 'NORMAL', value: parseInt(HID_KEYCODES[event.code], 16) }],
  //       }),
  //     )
  //   }
  //   setIsListeningToKeydown(false)
  // }

  const layersCount = state.keyConfigs.reduce(
    (acc, keyConfig) => Math.max(acc, keyConfig.layers.length),
    0,
  )

  const selectedKeys = state.keyConfigs.filter(({ id }) => state.selectedKeys.has(id))

  return (
    <div className="p-4 sm:p-8 flex flex-col items-center gap-8">
      <div className="flex gap-4 text-slate-400 text-xs uppercase">
        <Link
          to=""
          className={`${state.mode === 'configKeymap' ? 'text-pink-400' : 'text-slate-400'} hover:text-pink-400 active:text-pink-500 cursor-pointer`}
        >
          Keymap
        </Link>
        |
        <Link
          to=""
          className={`${state.mode === 'configActuation' ? 'text-pink-400' : 'text-slate-400'} hover:text-pink-400 active:text-pink-500 cursor-pointer`}
        >
          Actuation
        </Link>
        |
        <Link
          to=""
          className={`${state.mode === 'configDebug' ? 'text-pink-400' : 'text-slate-400'} hover:text-pink-400 active:text-pink-500 cursor-pointer`}
        >
          Debug
        </Link>
      </div>

      <div className="flex w-full gap-4">
        <div className="flex flex-col gap-2">
          {new Array(layersCount).fill(null).map((_, index) => (
            <div
              key={index}
              className={`${state.selectedLayer === index ? 'bg-pink-500 text-pink-100' : 'bg-slate-500 text-slate-300'} text-center select-none active:scale-110 cursor-pointer px-4 py-2 rounded-sm sm:rounded-md text-nowrap`}
              onClick={() => {
                if (state.selectedLayer !== index) {
                  setState((state) => ({ ...state, selectedLayer: index }))
                }
              }}
            >
              Layer {index}
            </div>
          ))}
          <div
            className={`bg-slate-500 text-slate-300 text-center select-none active:scale-110 cursor-pointer px-4 py-2 rounded-sm sm:rounded-md text-nowrap`}
            onClick={() => {
              /**
               * Add a new layer
               */
            }}
          >
            +
          </div>
        </div>

        <Keyboard state={state} setState={setState} />
      </div>

      {state.mode === 'configKeymap' && (
        <div className="flex gap-4 w-full">
          <div className="flex flex-col w-1/2">
            <div className="text-slate-400 text-xs uppercase">
              <span
                className="cursor-pointer hover:text-pink-400 active:text-pink-500"
                onClick={() => navigator.clipboard.writeText(JSON.stringify(state.keyConfigs))}
              >
                Copy json to clipboard
              </span>
            </div>

            <div className="flex flex-col gap-2">
              {/* <Button
                disabled={state.selectedKeys.size === 0}
                onClick={() => setIsListeningToKeydown(true)}
              >
                {isListeningToKeydown ? 'Listening...' : 'Click here to set a key'}
              </Button> */}

              <form className="flex flex-col gap-2">
                <Field
                  autoFocus
                  key={`keycode-${selectedKeys.map(({ id }) => id).join('-')}`}
                  label="Keycode"
                  type="text"
                  defaultValue={
                    selectedKeys[0]?.layers[state.selectedLayer].keycodes[0].value !== undefined &&
                    selectedKeys.every(
                      ({ layers }) =>
                        layers[state.selectedLayer].keycodes[0].value ===
                        selectedKeys[0]?.layers[state.selectedLayer].keycodes[0].value,
                    )
                      ? `0x${selectedKeys[0]?.layers[state.selectedLayer].keycodes[0].value.toString(16).padStart(2, '0')}`
                      : undefined
                  }
                  onChange={(event) => {
                    const value = event.target.value === '' ? 0 : parseInt(event.target.value, 16)
                    if (isNaN(value)) {
                      return
                    }

                    setState((state) =>
                      setKeycodeForSelectedKeys(state, {
                        autoCapsVelocityTreshold: 0,
                        holdTimeBeforeActuation: 0,
                        isAutoCapsEnabled: 0,
                        keycodes: [{ type: 'NORMAL', value }],
                      }),
                    )
                  }}
                  disabled={state.selectedKeys.size === 0}
                  hint="Set a keycode for the selected keys in hexadecimal"
                />
              </form>
            </div>
          </div>
        </div>
      )}
    </div>
  )
}
