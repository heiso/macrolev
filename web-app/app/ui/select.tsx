import { forwardRef, type SelectHTMLAttributes } from 'react'

export type Option = { value: string | number; label: string }
export type SelectProps = SelectHTMLAttributes<HTMLSelectElement> & {
  error?: string
  options?: Option[]
}

export const Select = forwardRef<HTMLSelectElement, SelectProps>(
  ({ error, className, options, children, ...props }, ref) => {
    return (
      <select
        className={`w-full border pl-4 pr-4 pt-3 pb-3 rounded-lg outline-none ${
          error
            ? 'text-red-500 placeholder:text-red-300 border-red-300 bg-red-100 hover:border-red-500'
            : 'text-pink-500 placeholder:text-pink-300 border-pink-500 focus:border-pink-500 hover:text-pink-500'
        } ${className ?? ''}`}
        ref={ref}
        {...props}
      >
        {options
          ? options.map(({ value, label }) => (
              <option key={value} value={value}>
                {label}
              </option>
            ))
          : children}
      </select>
    )
  },
)
Select.displayName = 'Select'
