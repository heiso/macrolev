import type { Config } from 'tailwindcss'

export default {
  content: ['./index.html', './app/**/*.{js,ts,jsx,tsx,mdx}'],
  theme: {
    extend: {
      fontFamily: {
        AtkinsonHyperlegible: ['Atkinson Hyperlegible', 'Atkinson Hyperlegible Fallback'],
      },
    },
  },
  plugins: [],
} satisfies Config
