# UwU Stack &middot; ![logo](./public/favicon-32x32.png)

## Install and first run

```bash
npm i
cp .env.tpl .env
npm run up
npm run reset-database
npm run log
```

Then go to http://localhost:3000

## DevEnv actions

Start

```bash
npm run up
```

Show logs

```bash
npm run log
```

Stop

```bash
npm run down
```

Restart

```bash
npm run restart
```

Truncate Database, Migrate and apply seeds

```bash
npm run reset-database
```

Run Lint

```bash
npm run lint
```

Run tests

```bash
npm run test
```

## TODO

- ✅ ...Done
- ⌛️ ...In Progress
- 🚨 ...Bug
- 🚧 ...Improvment

- full sqlite option
- redis/postgres option
- auth flow
- ui
