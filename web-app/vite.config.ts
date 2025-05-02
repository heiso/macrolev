import { reactRouter } from '@react-router/dev/vite'
import tailwindcss from '@tailwindcss/vite'
import autoprefixer from 'autoprefixer'
import { defineConfig } from 'vite'
import tsconfigPaths from 'vite-tsconfig-paths'

export default defineConfig({
  server: {
    port: 3000,
  },
  base: '/macrolev/',
  css: {
    postcss: {
      plugins: [autoprefixer],
    },
  },
  plugins: [reactRouter(), tsconfigPaths(), tailwindcss()],
})
