import { useEffect, useState } from 'react'
import { Label } from '../ui/label'

enum Status {
  MIGHT_BE_TAP,
  TAP,
  TRIGGERED,
  RESET,
  RAPID_TRIGGER_RESET,
}

export function SwitchOffsetsConfiguration() {
  const [distance, setDitance] = useState(0)
  // const [prevDistance, setPrevDitance] = useState(distance)
  const [triggerOffset, setTriggerOffset] = useState(50)
  const [resetOffsetFromTrigger, setResetOffsetFromTrigger] = useState(5)
  const [status, setStatus] = useState<Status>(Status.RESET)
  // const [isRapidTriggerEnabled, setIsRapidTriggerEnabled] = useState(false)
  // const [distanceWhereDirectionChanged, setDistanceWhereDirectionChanged] = useState(0)

  const resetOffset = triggerOffset - resetOffsetFromTrigger
  const isAfterTriggerOffset = distance > triggerOffset
  const isBeforeResetOffset = distance < resetOffset
  // const isAfterRapidTriggerOffset = distance > key->actuation.direction_changed_point - rapidTriggerOffset + keyboard_user_config.reset_threshold;
  // const isBeforeRapidResetOffset = distance < key->actuation.direction_changed_point - rapidTriggerOffset;

  useEffect(() => {
    switch (status) {
      case Status.RESET:
        if (isAfterTriggerOffset) {
          setStatus(Status.TRIGGERED)
        }
        break

      case Status.TRIGGERED:
        if (isBeforeResetOffset) {
          setStatus(Status.RESET)
        }
        break
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [distance])

  return (
    <div className="flex flex-col gap-8 sm:flex-row justify-center items-center">
      <div className="flex gap-8">
        <input
          type="range"
          min={0}
          max={100}
          step={1}
          defaultValue={distance}
          onChange={(event) => setDitance(Number(event.target.value))}
          style={{ writingMode: 'vertical-lr' }}
        />

        <div className="flex gap-1">
          <div className="border border-slate-500 w-8 h-32">
            <div
              className={`w-full ${status === Status.TRIGGERED ? 'bg-pink-700' : 'bg-slate-700'} `}
              style={{ height: `${distance}%` }}
            ></div>
          </div>

          <div className="h-full relative w-16">
            <div
              style={{ top: `${triggerOffset}%` }}
              className="absolute border-t border-slate-400 pl-6 h-6"
            >
              <div className="text-nowrap w-10">T {triggerOffset}</div>
            </div>

            <div
              style={{ top: `${resetOffset}%` }}
              className="absolute border-b border-slate-400 pl-6 h-6 -mt-6"
            >
              <div className="text-nowrap w-10">R {resetOffset}</div>
            </div>
          </div>
        </div>
      </div>

      <div className="flex flex-col gap-4">
        <div className="flex flex-col">
          <Label>Trigger offset</Label>
          <div className="flex flex-row gap-2">
            <input
              type="range"
              step={1}
              min={resetOffsetFromTrigger}
              max={100}
              value={triggerOffset}
              onChange={(event) => setTriggerOffset(Number(event.target.value))}
            />
            <input
              className="bg-transparent border-slate-400 border-b "
              type="number"
              min={resetOffsetFromTrigger}
              max={100}
              value={triggerOffset}
              onChange={(event) => setTriggerOffset(Number(event.target.value))}
            />
            %
          </div>
        </div>

        <div className="flex flex-col">
          <Label>Reset offset</Label>
          <div className="flex flex-row gap-2">
            <input
              type="range"
              step={1}
              min={0}
              max={100}
              value={resetOffsetFromTrigger}
              onChange={(event) => {
                const value = Number(event.target.value)
                if (value > triggerOffset) {
                  setTriggerOffset(value)
                }
                setResetOffsetFromTrigger(value)
              }}
            />
            <input
              className="bg-transparent border-slate-400 border-b "
              type="number"
              min={0}
              max={100}
              value={resetOffsetFromTrigger}
              onChange={(event) => {
                const value = Number(event.target.value)
                if (value > triggerOffset) {
                  setTriggerOffset(value)
                }
                setResetOffsetFromTrigger(value)
              }}
            />
            %
          </div>
        </div>
      </div>
    </div>
  )
}
