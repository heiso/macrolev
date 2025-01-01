import React, { type HTMLAttributes, type ReactNode } from 'react'

const EMPTY_HINT = 'n/a'

export type HintProps = React.PropsWithChildren<
  HTMLAttributes<HTMLElement> & {
    error?: string
    required?: boolean
    hint?: string | ReactNode
  }
>

export function Hint({ className, error, required, hint = EMPTY_HINT }: HintProps) {
  if (error) {
    return <div className={`text-sm text-red-400 ${className ?? ''}`}>{error}</div>
  }

  return (
    <div
      className={`text-sm text-gray-400 ${className ?? ''} ${
        hint === EMPTY_HINT ? 'opacity-0' : ''
      }`}
    >
      {hint}
    </div>
  )
}
