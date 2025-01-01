import { useEffect } from 'react'

import { Keyboard } from '../components/keyboard.tsx'
import { Button } from '../ui/button.tsx'
import { Label } from '../ui/label.tsx'
import { useDevice } from '../useDevice.ts'

export default function Index() {
  const {
    device,
    deviceConfig,
    isLoading,
    isConnected,
    connect,
    disconnect,
    autoReconnect,
    write,
    read,
    loadUserConfig,
    userConfig,
    setUserConfig,
    persistUserConfig,
    loadKeys,
    keys,
  } = useDevice()
  // const [lastTickDate, setLastTickDate] = useState<number>(Date.now())

  useEffect(() => {
    autoReconnect()
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [])

  useEffect(() => {
    if (isConnected) {
      console.log({ device, deviceConfig })
      loadUserConfig()
      loadKeys()
    }
  }, [isConnected])

  return (
    <div className="space-y-8 p-8">
      <div className="relative">
        {!isConnected && !isLoading && (
          <div className="absolute z-10 top-0 bottom-0 left-0 right-0 flex justify-center items-center">
            <Button primary onClick={connect}>
              Connect your macrolev
            </Button>
          </div>
        )}

        <div className={!isConnected || isLoading ? 'opacity-80' : ''}>
          <Keyboard keys={keys} />
        </div>
      </div>

      {isConnected && device && userConfig && (
        <>
          <div>
            <div className="flex gap-2 items-center">
              <Label>Trigger Offset</Label>
              <input
                className="cursor-pointer"
                min={1}
                max={255}
                step={1}
                type="range"
                onChange={(event) => {
                  setUserConfig({ ...userConfig, triggerOffset: Number(event.target.value) })
                }}
                value={userConfig.triggerOffset}
              />
              <span className="text-slate-400">
                {Number(((userConfig.triggerOffset * 4) / 255).toFixed(1))} mm
              </span>
            </div>

            <div className="flex gap-2 items-center">
              <Label>Rapid Trigger Offset</Label>
              <input
                className="cursor-pointer"
                min={1}
                max={255}
                step={1}
                type="range"
                onChange={(event) => {
                  setUserConfig({ ...userConfig, rapidTriggerOffset: Number(event.target.value) })
                }}
                value={userConfig.rapidTriggerOffset}
              />
              <span className="text-slate-400">
                {Number(((userConfig.rapidTriggerOffset * 4) / 255).toFixed(1))} mm
              </span>
            </div>

            <div className="flex gap-2 items-center">
              <Label>Reset Threshold</Label>
              <input
                className="cursor-pointer"
                min={1}
                max={255}
                step={1}
                type="range"
                onChange={(event) => {
                  setUserConfig({ ...userConfig, resetThreshold: Number(event.target.value) })
                }}
                value={userConfig.resetThreshold}
              />
              <span className="text-slate-400">
                {Number(((userConfig.resetThreshold * 4) / 255).toFixed(2))} mm
              </span>
            </div>

            <div className="flex gap-2 items-center">
              <Label>Screaming Velocity Trigger</Label>
              <input
                className="cursor-pointer"
                min={1}
                max={255}
                step={1}
                type="range"
                onChange={(event) => {
                  setUserConfig({
                    ...userConfig,
                    screamingVelocityTrigger: Number(event.target.value),
                  })
                }}
                value={userConfig.screamingVelocityTrigger}
              />
              <span className="text-slate-400">{Number(userConfig.screamingVelocityTrigger)}</span>
            </div>

            <div className="flex gap-2 items-center">
              <Label>Tap Timeout</Label>
              <input
                min={1}
                max={65535}
                type="number"
                onChange={(event) => {
                  setUserConfig({
                    ...userConfig,
                    tapTimeout: Number(event.target.value),
                  })
                }}
                value={userConfig.tapTimeout}
              />
              <span className="text-slate-400">ms</span>
            </div>
          </div>

          <div className="space-x-4">
            <Button primary onClick={disconnect}>
              Disconnect
            </Button>

            <Button onClick={loadKeys}>Load Keys</Button>
            <Button
              onClick={() => {
                const json = JSON.stringify(keys, null, 2)
                navigator.clipboard.writeText(json)
              }}
            >
              Copy Keys to clipboard
            </Button>

            <Button onClick={loadUserConfig}>Load Config</Button>
            <Button onClick={persistUserConfig}>Save Config</Button>
            <Button onClick={() => write('VENDOR_REQUEST_RESET_CONFIG')}>Reset Config</Button>
            <Button
              onClick={async () =>
                console.log((await read('VENDOR_REQUEST_CYCLE_DURATION')).getInt32(0, true))
              }
            >
              Get cycle duration (in console)
            </Button>
            <Button onClick={() => write('VENDOR_REQUEST_DFU_MODE')}>DFU Mode</Button>
          </div>
          <pre>{JSON.stringify(userConfig, null, 2)}</pre>
        </>
      )}
    </div>
  )
}
