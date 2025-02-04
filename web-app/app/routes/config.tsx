import { useState } from 'react'
import { Link } from 'react-router'
import { Keyboard } from '../components/keyboard/keyboard'
import { initHistory, type KeyConfig, type State } from '../components/keyboard/keyboard-state'
import { SwitchOffsetsConfiguration } from '../components/switch-offsets-configuration'
import defaultLayout from '../layouts/macrolev-azerty-mac-fr.json'

export default function Index() {
  const [state, setState] = useState<State>(
    initHistory({
      keyConfigs: defaultLayout as KeyConfig[],
      selectedKeys: new Set<KeyConfig['id']>(),
      history: [],
      currentHistoryIndex: 0,
      width: 0,
      height: 0,
      mode: 'configKeymap',
    }),
  )

  const layersCount = state.keyConfigs.reduce(
    (acc, keyConfig) => Math.max(acc, keyConfig.layers.length),
    0,
  )

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

      <div className="flex gap-4 w-full">
        <div className="flex flex-col gap-4">
          {new Array(layersCount).fill(null).map((_, index) => (
            <div key={index}>{index}</div>
          ))}
        </div>

        <Keyboard state={state} setState={setState} />
      </div>

      {state.mode === 'configKeymap' && (
        <div className="flex gap-4 w-full">
          <div className="flex flex-col w-1/2">
            <div
              onKeyDown={(event) => console.log(event.code)}
              className="flex justify-center items-center border border-slate-200 rounded-md w-full min-h-8"
            >
              {/* TODO: enable the keydown event only when user has clicked on a button, then disable the onkeydown when the keycode has been captured */}
              <div>Type a key to set</div>
            </div>
          </div>
        </div>
      )}

      <SwitchOffsetsConfiguration />
    </div>
  )
}
