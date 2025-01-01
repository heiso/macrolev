import { Link as ReactRouterLink, type LinkProps as ReactRouterLinkProps } from 'react-router'

export type LinkProps = ReactRouterLinkProps

export function Link({ className, children, ...props }: LinkProps) {
  return (
    <ReactRouterLink
      className={`transition active:opacity-80 text-pink-500 hover:underline underline-offset-4 ${
        className ?? ''
      }`}
      {...props}
    >
      {children}
    </ReactRouterLink>
  )
}
