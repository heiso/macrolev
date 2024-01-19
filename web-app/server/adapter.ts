import type { AppLoadContext, ServerBuild } from '@remix-run/node'
import {
  createReadableStreamFromReadable,
  createRequestHandler as createRemixRequestHandler,
  writeReadableStreamToWritable,
} from '@remix-run/node'
import type * as koa from 'koa'

export type GetLoadContextFunction = (
  ctx: koa.Context,
  next: koa.Next,
) => Promise<AppLoadContext> | AppLoadContext

export type RequestHandler = (ctx: koa.Context, next: koa.Next) => Promise<void>

export function createRequestHandler({
  build,
  getLoadContext,
  mode = process.env.NODE_ENV,
}: {
  build: ServerBuild | (() => Promise<ServerBuild>)
  getLoadContext?: GetLoadContextFunction
  mode?: string
}): RequestHandler {
  let handleRequest = createRemixRequestHandler(build, mode)

  return async (ctx: koa.Context, next: koa.Next) => {
    let request = createRemixRequest(ctx, next)
    let loadContext = await getLoadContext?.(ctx, next)

    let response = await handleRequest(request, loadContext)

    await sendRemixResponse(ctx, response)
  }
}

export function createRemixHeaders(requestHeaders: koa.Request['headers']): Headers {
  let headers = new Headers()

  for (let [key, values] of Object.entries(requestHeaders)) {
    if (values) {
      if (Array.isArray(values)) {
        for (let value of values) {
          headers.append(key, value)
        }
      } else {
        headers.set(key, values)
      }
    }
  }

  return headers
}

export function createRemixRequest(ctx: koa.Context, next: koa.Next): Request {
  let url = new URL(`${ctx.protocol}://${ctx.host}${ctx.url}`)

  // Abort action/loaders once we can no longer write a response
  let controller = new AbortController()
  ctx.res.on('close', () => controller.abort())

  let init: RequestInit = {
    method: ctx.req.method,
    headers: createRemixHeaders(ctx.req.headers),
    signal: controller.signal,
  }

  if (ctx.req.method !== 'GET' && ctx.req.method !== 'HEAD') {
    init.body = createReadableStreamFromReadable(ctx.req)
    ;(init as { duplex: 'half' }).duplex = 'half'
  }

  return new Request(url.href, init)
}

export async function sendRemixResponse(ctx: koa.Context, nodeResponse: Response): Promise<void> {
  ctx.res.statusMessage = nodeResponse.statusText
  ctx.res.statusCode = nodeResponse.status

  for (let [key, value] of nodeResponse.headers.entries()) {
    ctx.res.appendHeader(key, value)
  }

  if (nodeResponse.body) {
    await writeReadableStreamToWritable(nodeResponse.body, ctx.res)
  } else {
    ctx.res.end()
  }
}
