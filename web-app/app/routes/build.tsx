import { useState } from 'react'
import { Field } from '../components/field'
import { Keyboard } from '../components/keyboard/keyboard'
import {
  initHistory,
  setAdcChannelForSelectedKeys,
  setMuxChannelForSelectedKeys,
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
      mode: 'buildLayout',
    }),
  )

  const selectedKeys = state.keyConfigs.filter(({ id }) => state.selectedKeys.has(id))

  return (
    <div className="p-4 sm:p-8 flex flex-col items-center gap-8">
      <div className="flex gap-4 text-slate-400 text-xs uppercase">
        <span
          onClick={() => setState((state) => ({ ...state, mode: 'buildLayout' }))}
          className={`${state.mode === 'buildLayout' ? 'text-pink-400' : 'text-slate-400'} hover:text-pink-400 active:text-pink-500 cursor-pointer`}
        >
          Layout
        </span>
        |
        <span
          onClick={() => setState((state) => ({ ...state, mode: 'buildChannels' }))}
          className={`${state.mode === 'buildChannels' ? 'text-pink-400' : 'text-slate-400'} hover:text-pink-400 active:text-pink-500 cursor-pointer`}
        >
          ADC & MUX channels
        </span>
      </div>

      <Keyboard state={state} setState={setState} />

      <div className="flex gap-4 w-full">
        <div className="flex flex-col text-slate-400 text-xs uppercase gap-4 w-1/2">
          <div className="flex gap-2">
            <span>
              {state.keyConfigs.length} key{state.keyConfigs.length > 1 ? 's' : ''}
            </span>
            {state.selectedKeys.size > 0 && (
              <>
                <span>/</span>
                <span>
                  {state.selectedKeys.size} key{state.selectedKeys.size > 1 ? 's' : ''} selected
                </span>
              </>
            )}
          </div>

          {state.mode === 'buildLayout' && (
            <>
              <div>
                {Math.ceil(state.keyConfigs.length / 16)} analog demultiplexer
                {Math.ceil(state.keyConfigs.length / 16) > 1 ? 's' : ''}
              </div>

              <div>
                pcb dimensions {(state.width * 19.05).toFixed(1)} x{' '}
                {(state.height * 19.05).toFixed(1)} mm
              </div>
            </>
          )}

          {state.keyConfigs.length > 0 && (
            <div>
              <span
                className="cursor-pointer hover:text-pink-400 active:text-pink-500"
                onClick={() => navigator.clipboard.writeText(JSON.stringify(state.keyConfigs))}
              >
                Copy json to clipboard
              </span>
            </div>
          )}
        </div>

        <div className="flex flex-col gap-4 w-1/2">
          {state.mode === 'buildChannels' && (
            <form className="flex flex-col gap-2">
              <Field
                autoFocus
                min={0}
                max={Math.ceil(state.keyConfigs.length / 16) - 1}
                key={`adc-${selectedKeys.map(({ id }) => id).join('-')}`}
                label="Analog Digital Converter (ADC) channel"
                type="number"
                defaultValue={
                  selectedKeys.every(
                    ({ hardware }) => hardware.adcChannel === selectedKeys[0]?.hardware.adcChannel,
                  )
                    ? selectedKeys[0]?.hardware.adcChannel
                    : undefined
                }
                onChange={(event) => {
                  setState((state) =>
                    setAdcChannelForSelectedKeys(state, parseInt(event.target.value)),
                  )
                }}
                disabled={state.selectedKeys.size === 0}
                hint="Select at least one key to configure its ADC channel"
              />

              <Field
                min={0}
                max={15}
                key={`mux-${selectedKeys.map(({ id }) => id).join('-')}`}
                label="Analog Multiplexer (MUX) channel"
                type="number"
                defaultValue={
                  state.selectedKeys.size === 1
                    ? state.keyConfigs.find(({ id }) => state.selectedKeys.has(id))?.hardware
                        .muxChannel
                    : undefined
                }
                onChange={(event) => {
                  setState((state) =>
                    setMuxChannelForSelectedKeys(state, parseInt(event.target.value)),
                  )
                }}
                hint="Select one key to configure its MUX channel"
                disabled={state.selectedKeys.size !== 1}
              />
            </form>
          )}
        </div>
      </div>
    </div>
  )
}
