import { readConfig } from '@remix-run/dev/dist/config.js'
import type { ConfigRoute, RouteManifest } from '@remix-run/dev/dist/config/routes.js'
import { writeFileSync } from 'fs'
import { format, resolveConfig } from 'prettier'

const outputPath = `routes.ts`

function buildPath(routes: RouteManifest, route: ConfigRoute): string {
  const result = []
  if (route.parentId) {
    result.push(buildPath(routes, routes[route.parentId]))
  }
  if (route.path) {
    result.push(route.path)
  }

  return result.join('/')
}

function buildParams(path: string, input: boolean = false) {
  return path
    .replace(/'/g, '')
    .split('/')
    .filter((part) => part.startsWith(':'))
    .map((part) => part.replace(':', ''))
    .map(
      (param) =>
        `${param}: string${input ? ' | number' : ''}${param.endsWith('?') ? ' | undefined' : ''}`,
    )
    .join(',')
}

;(async () => {
  const { routes } = await readConfig()
  const paths = [
    ...new Set(
      Object.keys(routes).map((key) => {
        let path = buildPath(routes, routes[key])
        if (path === '') {
          path = '/'
        }
        return `'${path}'`
      }),
    ),
  ]

  const content = `
    export const routerPaths = {${paths
      .map((path) => {
        if (!path.includes('/:')) {
          return `${path}: ${path}`
        }

        return `${path}(params: {${buildParams(path, true)}}) {
          return buildPath(${path}, params)
        }`
      })
      .join(',')}} as const

    export const unsafeRouterPaths = {${paths
      .map((path) => {
        return `${path}(params: Record<string, unknown>) {
          return unsafeBuildPath(${path}, params)
        }`
      })
      .join(',')}} as const

    export type RouterPath = keyof typeof routerPaths

    export type RouterParams = {${paths.map((path) => `${path}: {${buildParams(path)}}`).join(',')}}

    type InputParams = {${paths.map((path) => `${path}: {${buildParams(path, true)}}`).join(',')}}
    
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    function buildPath<TRouterPath extends RouterPath>(route: TRouterPath, params: InputParams[TRouterPath]) {
      return route
        .replace(/'/g, '')
        .split('/')
        .map((part) =>
          part.startsWith(':') ? params[part.replace(':', '').replace('?', '') as keyof InputParams[TRouterPath]] : part,
        )
        .filter((part) => part !== undefined)
        .join('/')
    }

    function unsafeBuildPath<TRouterPath extends RouterPath>(
      route: TRouterPath,
      params: Record<string, unknown>,
    ) {
      return route
        .replace(/'/g, '')
        .split('/')
        .map((part) => (part.startsWith(':') ? params[part.replace(':', '').replace('?', '')] : part))
        .filter((part) => part !== undefined)
        .join('/')
    }
  `

  try {
    const formatted = await format(content, {
      ...(await resolveConfig(process.cwd())),
      parser: 'typescript',
    })

    writeFileSync(outputPath, formatted)
  } catch (err) {
    console.log(content)
  }
})()
