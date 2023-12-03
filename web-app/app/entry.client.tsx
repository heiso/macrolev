import { RemixBrowser } from '@remix-run/react'
import { startTransition, StrictMode } from 'react'
import { hydrateRoot } from 'react-dom/client'
import { routerPaths } from '../routes.ts'

startTransition(() => {
  hydrateRoot(
    document,
    <StrictMode>
      <RemixBrowser />
    </StrictMode>,
  )
})

window.addEventListener('focus', async () => {
  const response = await fetch(routerPaths['/health'])
  if (response.status === 200) {
    const json = await response.json()
    if (json.build !== window.ENV.BUILD_VERSION && json.shouldRefreshIfVersionMismatch) {
      window.location.reload()
    }
  }
})
