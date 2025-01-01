import type { Config } from '@react-router/dev/config'

export default {
  ssr: false,
  basename: '/macrolev/',
  future: {
    unstable_optimizeDeps: true,
  },
} satisfies Config
