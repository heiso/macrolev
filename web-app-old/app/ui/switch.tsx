import { forwardRef, useState, type InputHTMLAttributes } from 'react'

export type SwitchProps = InputHTMLAttributes<HTMLInputElement> & {
  error?: string
  leftLabel: string
  rightLabel: string
}

export const Switch = forwardRef<HTMLInputElement, SwitchProps>(
  ({ error, className, leftLabel, rightLabel, ...props }, ref) => {
    const [isRight, setIsRight] = useState<boolean>(Boolean(props.defaultChecked))

    return (
      <label className="cursor-pointer flex flex-row items-center select-none">
        <div className="px-4 py-3 text-right">{leftLabel}</div>
        <div className="border-pink-500 hover:border-pink-500 focus:border-pink-500 rounded-full bg-pink-100 p-0.5">
          <div className="relative h-4 w-7">
            <input
              type="checkbox"
              className="hidden"
              ref={ref}
              onChange={(event) => setIsRight(event.target.checked)}
              {...props}
              required={false}
            />
            <span
              className={`absolute top-0 bottom-0 left-0 z-10 transition ease-out rounded-full h-full aspect-square bg-pink-500 ${
                isRight ? 'translate-x-3/4' : ''
              }`}
            ></span>
          </div>
        </div>
        <div className="px-4 py-3 text-left">{rightLabel}</div>
      </label>
    )
  },
)
Switch.displayName = 'Switch'
