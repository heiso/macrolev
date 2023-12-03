/** @type {import('@remix-run/dev').AppConfig} */
export default {
  ignoredRouteFiles: ['**/.*'],
  serverBuildPath: './build/app/index.js',
  serverModuleFormat: 'esm',
  tailwind: true,
  postcss: true,
  watchPaths: ['./tailwind.config.ts'],
}
