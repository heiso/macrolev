import { readFileSync, readdirSync, statSync, writeFileSync } from 'fs'
import { format, resolveConfig } from 'prettier'

const svgsDir = `svgs`
const outputPath = `public`
const outputTS = `svgs.ts`

;(async () => {
  const directories = readdirSync(svgsDir).filter((dir) =>
    statSync(`${svgsDir}/${dir}`).isDirectory(),
  )

  const filesByDir = directories.reduce<Record<string, string[]>>((acc, dir) => {
    const files = readdirSync(`${svgsDir}/${dir}`).filter((file) => file.endsWith('.svg'))
    acc[dir] = files

    return acc
  }, {})

  Object.keys(filesByDir).map(async (dir) => {
    const files = filesByDir[dir]

    const content = `
    <svg xmlns="http://www.w3.org/2000/svg">
      <defs>
        ${files
          .map((file) => {
            const content = readFileSync(`${svgsDir}/${dir}/${file}`).toString()
            return content
              .replace('<svg', `<symbol id="${file.replace('.svg', '')}"`)
              .replace('</svg>', '</symbol>')
              .replace(/fill="[#|a-z|0-9]*"/gi, '')
          })
          .join('\n')}
      </defs>
    </svg>
    `

    try {
      const formatted = await format(content, {
        ...(await resolveConfig(process.cwd())),
        parser: 'html',
      })
      writeFileSync(`${outputPath}/${dir}.svg`, formatted)
    } catch (err) {
      console.log(content)
    }
  })

  const content = Object.keys(filesByDir)
    .filter((dir) => filesByDir[dir].length > 0)
    .map((dir) => {
      return `
      export const ${dir}Svgs = [${filesByDir[dir]
        .map((file) => `'${file.replace('.svg', '')}'`)
        .join(',')}] as const
      export type ${dir.charAt(0).toUpperCase()}${dir.slice(1)}Svg = ${filesByDir[dir]
        .map((file) => `'${file.replace('.svg', '')}'`)
        .join('|')}
      `
    })
    .join('\n')

  try {
    const formatted = await format(content, {
      ...(await resolveConfig(process.cwd())),
      parser: 'typescript',
    })

    writeFileSync(outputTS, formatted)
  } catch (err) {
    console.log(content)
  }
})()
