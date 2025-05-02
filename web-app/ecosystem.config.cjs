// @ts-check

/**
 * @type {import('pm2/types').StartOptions}
 */
const common = {
  instances: 1,
  autorestart: false,
  log_date_format: 'HH:mm:ss',
}

/**
 * @type {{apps: import('pm2/types').StartOptions[]}}
 */
const config = {
  apps: [
    {
      ...common,
      name: 'icons',
      script: 'npx tsx generate-svg-icons-sprite.ts',
      watch: ['generate-svg-icons-sprite.ts', './svgs/**/*.svg'],
    },

    {
      ...common,
      name: 'app',
      watch: ['vite.config.ts'],
      script: 'npx react-router dev',
    },
  ],
}

module.exports = config
