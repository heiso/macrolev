import { json } from '@remix-run/node'

export function loader() {
  const shouldRefreshIfVersionMismatch = Boolean(
    process.env.SHOULD_REFRESH_BROWSER_IF_VERSION_MISMATCH &&
      process.env.SHOULD_REFRESH_BROWSER_IF_VERSION_MISMATCH !== 'false',
  )

  return json({
    build: process.env.BUILD_VERSION,
    shouldRefreshIfVersionMismatch,
  })
}
