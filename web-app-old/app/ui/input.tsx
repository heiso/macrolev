import {
  forwardRef,
  useEffect,
  useImperativeHandle,
  useRef,
  useState,
  type InputHTMLAttributes,
  type ReactNode,
} from 'react'
import { Icon } from './icon.tsx'

export type InputProps = InputHTMLAttributes<HTMLInputElement> & {
  hasError?: boolean
  icon?: ReactNode
}

export const defaultStyles =
  'text-pink-500 placeholder:text-pink-300 hover:border-pink-500 focus:border-pink-500'
export const errorStyles =
  'text-red-500 placeholder:text-red-300 hover:border-red-500 focus:border-red-500 bg-red-100 selection:bg-red-500 selection:text-red-50'

export const Input = forwardRef<HTMLInputElement, InputProps>(
  ({ hasError, className, icon, ...props }, ref) => {
    const innerRef = useRef<HTMLInputElement>(null)
    useImperativeHandle(ref, () => innerRef.current!, [])
    const [isFilled, setIsFilled] = useState(Boolean(props.defaultValue))

    useEffect(() => {
      setIsFilled(Boolean(innerRef?.current?.value))
    }, [innerRef?.current?.value])

    return (
      <div className={`relative  ${className ?? ''}`}>
        <input
          className={`w-full h-full px-4 py-3 ${
            icon ? 'pr-11' : ''
          } appearance-none outline-none disabled:pointer-events-none border rounded-lg ${
            hasError ? 'bg-red-100' : isFilled ? 'bg-pink-100' : 'bg-white'
          } ${hasError ? errorStyles : defaultStyles}`}
          ref={innerRef}
          {...props}
        />
        {icon && (
          <div
            className={`absolute top-0 right-0 bottom-0 z-10 px-4 py-3 select-none ${
              hasError ? 'text-red-500 fill-red-500' : 'text-pink-500 fill-pink-500'
            } flex justify-center items-center`}
          >
            {icon}
          </div>
        )}
      </div>
    )
  },
)
Input.displayName = 'Input'

export const Checkbox = forwardRef<HTMLInputElement, InputProps>(
  ({ hasError, className, ...props }, ref) => {
    return (
      <div className="relative flex flex-row items-center justify-center">
        <input
          className={`peer appearance-none h-4 w-4 border rounded-sm outline-none cursor-pointer border-pink-500 hover:border-pink-500 focus:border-pink-500 checked:bg-pink-500 ${
            className ?? ''
          }`}
          ref={ref}
          {...props}
          type="checkbox"
        />
        <Icon
          id="check"
          className="absolute z-10 top-0 bottom-0 left-0 right-0 transition opacity-0 peer-checked:opacity-100 stroke-1 stroke-pink-100 fill-pink-100"
        />
      </div>
    )
  },
)
Checkbox.displayName = 'Checkbox'
