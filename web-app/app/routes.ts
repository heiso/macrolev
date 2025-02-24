import { type RouteConfig, index, layout, route } from '@react-router/dev/routes'

export const paths = {
  home: '/',
  oldConfigurator: '/configurator',
  config: '/config',
  build: '/build',
}

export default [
  layout('routes/layout.tsx', [
    index('routes/home.tsx'),
    route(paths.oldConfigurator, 'routes/old-configurator.tsx'),
    route(paths.build, 'routes/build.tsx'),
    route(paths.config, 'routes/config.tsx'),
  ]),
] satisfies RouteConfig
