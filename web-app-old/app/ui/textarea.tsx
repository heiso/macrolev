import { forwardRef, type TextareaHTMLAttributes } from 'react'

export type TextareaProps = TextareaHTMLAttributes<HTMLTextAreaElement> & { hasError?: boolean }

export const Textarea = forwardRef<HTMLTextAreaElement, TextareaProps>(
  ({ hasError, className, value, ...props }, ref) => {
    return (
      <textarea
        className={`w-full pl-4 pr-4 pt-3 pb-3 rounded-lg outline-none resize-none ${
          hasError
            ? 'bg-red-100 text-red-500 placeholder:text-red-300 hover:border-red-500'
            : 'bg-pink-100 text-pink-500 placeholder:text-pink-500  hover:text-pink-500 hover:border-pink-500 focus:border-pink-500'
        } ${className ?? ''}`}
        ref={ref}
        {...props}
      >
        {value}
      </textarea>
    )
  },
)
Textarea.displayName = 'Textarea'
