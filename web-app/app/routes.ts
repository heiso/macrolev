import { type RouteConfig, index, layout, route } from '@react-router/dev/routes'

export const paths = {
  home: '/',
  config: '/config',
  configurator: '/configurator',
  layoutEditor: '/layout-editor',
}

export default [
  layout('routes/layout.tsx', [
    index('routes/home.tsx'),
    route(paths.configurator, 'routes/configurator.tsx'),
    route(paths.config, 'routes/config.tsx'),
    route(paths.layoutEditor, 'routes/layout-editor.tsx'),
  ]),
] satisfies RouteConfig
