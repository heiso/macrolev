import { Link as RemixLink, type LinkProps as RemixLinkProps } from '@remix-run/react'

export type LinkProps = RemixLinkProps

export function Link({ className, children, ...props }: LinkProps) {
  return (
    <RemixLink
      className={`transition active:opacity-80 text-pink-500 hover:underline underline-offset-4 ${
        className ?? ''
      }`}
      {...props}
    >
      {children}
    </RemixLink>
  )
}
