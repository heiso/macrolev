import { routerPaths } from '../../routes.ts'
import { Logo } from '../components/logo.tsx'
import { Button } from '../ui/button.tsx'
import { Icon } from '../ui/icon.tsx'

export default function Index() {
  return (
    <div className="h-full flex flex-col gap-12 text-center justify-center p-20 w-[80ch] mx-auto">
      <div className="flex flex-row items-center justify-center">
        <Logo />
        <div className="text-center">
          <h1 className="text-3xl font-light text-gray-100">
            <span className="font-bold">Macro</span>lev
          </h1>

          <span className="text-lg font-semibold text-gray-400 transition hover:brightness-125">
            Open Source Keyboard
          </span>
        </div>
      </div>

      <p className="pb-1 text-6xl bg-gradient-to-r from-gray-50 to-pink-200 bg-clip-text text-transparent font-bold">
        An open source analog keyboard
      </p>

      <div className="flex flex-row justify-center gap-6">
        <Button
          as="link"
          to="https://github.com/heiso/macrolev"
          target="_blank"
          className="inline-flex gap-2"
        >
          <Icon id="github-logo" className="self-center fill-gray-200" />
          GitHub
        </Button>
        <Button primary as="link" to={routerPaths['/hid']}>
          Test your macrolev
        </Button>
      </div>
    </div>
  )
}
