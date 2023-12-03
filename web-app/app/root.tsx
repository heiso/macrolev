import { json, type LinksFunction, type MetaFunction } from '@remix-run/node'
import {
  isRouteErrorResponse,
  Link,
  Links,
  LiveReload,
  Meta,
  Outlet,
  Scripts,
  ScrollRestoration,
  useLoaderData,
  useRouteError,
} from '@remix-run/react'
import type { PropsWithChildren } from 'react'
import { routerPaths } from '../routes.ts'
import fontStyleSheetUrl from './font.css'
import tailwindStylesheetUrl from './tailwind.css'

type ENV = {
  ENV: string
  BUILD_VERSION: string
}

declare global {
  var ENV: ENV
  interface Window {
    ENV: ENV
  }
}

export function loader() {
  return json({
    ENV: {
      ENV: process.env.ENV,
      BUILD_VERSION: process.env.BUILD_VERSION,
    },
  })
}

export const meta: MetaFunction = () => {
  return [{ title: 'UwU Stack App' }]
}

export const links: LinksFunction = () => {
  return [
    // Preload CSS as a resource to avoid render blocking
    { rel: 'preload', href: fontStyleSheetUrl, as: 'style' },
    { rel: 'preload', href: tailwindStylesheetUrl, as: 'style' },
    {
      rel: 'alternate icon',
      type: 'image/png',
      href: '/favicon-32x32.png',
    },
    { rel: 'apple-touch-icon', href: '/apple-touch-icon.png' },
    {
      rel: 'manifest',
      href: '/site.webmanifest',
      crossOrigin: 'use-credentials',
    } as const, // necessary to make typescript happy
    { rel: 'stylesheet', href: fontStyleSheetUrl },
    { rel: 'stylesheet', href: tailwindStylesheetUrl },
  ]
}

type DocumentProps = PropsWithChildren
function Document({ children }: DocumentProps) {
  return (
    <html
      lang="fr"
      className="w-full max-w-7xl mx-auto h-full bg-slate-800 text-gray-200 selection:bg-pink-500 selection:text-pink-50"
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
        <LiveReload />
      </body>
    </html>
  )
}

export function ErrorBoundary() {
  const error = useRouteError()

  return (
    <Document>
      <h1 className="text-center">Oops! I did it again</h1>

      {isRouteErrorResponse(error) && <div>Status: {error.status}</div>}

      <Link className="hover:underline" to={routerPaths['/']}>
        Go back
      </Link>
    </Document>
  )
}

export default function App() {
  const { ENV } = useLoaderData<typeof loader>()

  return (
    <Document>
      <Outlet />
      <script
        dangerouslySetInnerHTML={{
          __html: `window.ENV = ${JSON.stringify(ENV)}`,
        }}
      />
    </Document>
  )
}
