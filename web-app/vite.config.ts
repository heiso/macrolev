import { vitePlugin as remix } from '@remix-run/dev'
import { installGlobals } from '@remix-run/node'

import { defineConfig } from 'vite'

installGlobals()

const BASE_PATH = '/macrolev/'

export default defineConfig({
  base: BASE_PATH,
  server: {
    port: 3000,
  },
  plugins: [
    remix({
      ssr: false,
      basename: BASE_PATH,
    }),
  ],
})
