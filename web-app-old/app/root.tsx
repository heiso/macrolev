import type { LinksFunction } from '@remix-run/node'
import {
  Links,
  Meta,
  Outlet,
  Scripts,
  ScrollRestoration,
  type MetaFunction,
} from '@remix-run/react'
import type { PropsWithChildren } from 'react'
import './font.css'
import './tailwind.css'

export const meta: MetaFunction = () => {
  return [{ title: 'Macrolev' }]
}

export const links: LinksFunction = () => {
  return [
    {
      rel: 'alternate icon',
      type: 'image/png',
      href: 'favicon-32x32.png',
    },
    { rel: 'apple-touch-icon', href: 'apple-touch-icon.png' },
    {
      rel: 'manifest',
      href: 'site.webmanifest',
      crossOrigin: 'use-credentials',
    } as const, // necessary to make typescript happy
  ]
}

type DocumentProps = PropsWithChildren
function Document({ children }: DocumentProps) {
  return (
    <html
      lang="fr"
      className="w-full h-full bg-slate-800 text-gray-200 selection:bg-pink-500 selection:text-pink-50"
    >
      <head>
        <meta charSet="utf-8" />
        <meta name="viewport" content="width=device-width,initial-scale=1" />
        <Meta />
        <Links />
      </head>
      <body className="w-full h-full">
        {children}
        <ScrollRestoration />
        <Scripts />
      </body>
    </html>
  )
}

export default function App() {
  return (
    <Document>
      <Outlet />
    </Document>
  )
}
