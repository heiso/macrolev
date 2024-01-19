import { installGlobals, type ServerBuild } from '@remix-run/node'
import { secondsInHour, secondsInYear, secondsToMilliseconds } from 'date-fns'
import Koa, { type Context } from 'koa'
import connect from 'koa-connect'
import serve from 'koa-static'
import { createRequestHandler } from './adapter.js'
import './process.js'

installGlobals()

declare module '@remix-run/server-runtime' {
  export interface AppLoadContext extends Context {}
}

;(async () => {
  const app = new Koa()

  const viteDevServer =
    process.env.NODE_ENV === 'development'
      ? await import('vite').then((vite) =>
          vite.createServer({
            server: { middlewareMode: true },
          }),
        )
      : undefined

  app.use(function setClientIP(ctx, next) {
    if (ctx.request.headers['fly-client-ip']) {
      ctx.request.ip = Array.isArray(ctx.request.headers['fly-client-ip'])
        ? ctx.request.headers['fly-client-ip'][0]
        : ctx.request.headers['fly-client-ip']
    }
    return next()
  })

  // No ending slashes for SEO reasons
  // https://github.com/epicweb-dev/epic-stack/discussions/108
  app.use(function removeTrailingSlash(ctx, next) {
    if (ctx.request.path.endsWith('/') && ctx.request.path.length > 1) {
      const query = ctx.request.url.slice(ctx.request.path.length)
      const safepath = ctx.request.path.slice(0, -1).replace(/\/+/g, '/')
      ctx.status = 301
      ctx.redirect(safepath + query)
    } else {
      return next()
    }
  })

  // handle asset requests
  if (viteDevServer) {
    app.use(connect(viteDevServer.middlewares))
  } else {
    app.use(
      serve('build/client/assets', {
        immutable: true,
        maxAge: secondsToMilliseconds(secondsInYear),
      }),
    )

    app.use(
      serve('build/client/fonts', {
        immutable: true,
        maxAge: secondsToMilliseconds(secondsInYear),
      }),
    )

    app.use(
      serve('build/client', {
        immutable: false,
        maxAge: secondsToMilliseconds(secondsInHour),
      }),
    )
  }

  const build = (
    viteDevServer
      ? () => viteDevServer.ssrLoadModule('virtual:remix/server-build')
      : // @ts-expect-error
        await import('../vite-server/index.js')
  ) as ServerBuild | (() => Promise<ServerBuild>)

  app.use(createRequestHandler({ build, mode: process.env.NODE_ENV, getLoadContext: (ctx) => ctx }))

  app.listen({ port: Number(process.env.PORT) }, () => {
    console.log(`🚀 To infinity...and beyond!`)
  })
})()
