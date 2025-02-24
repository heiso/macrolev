import { Logo } from '../components/logo'
import { paths } from '../routes'
import { Button } from '../ui/button'
import { Icon } from '../ui/icon'

export default function Index() {
  return (
    <div className="h-full flex flex-col text-center justify-center">
      <div className="flex items-center justify-center gap-12 flex-1 scale-75 sm:scale-100">
        <Logo />
        <div className="text-center flex flex-col gap-2">
          <h1 className="text-3xl font-light text-gray-100">
            <span className="font-bold">Macro</span>lev
          </h1>

          <span className="text-lg font-light text-gray-400 transition hover:brightness-125 leading-tight">
            Open Source <br />
            Analog Keyboard
          </span>
        </div>
      </div>

      <div className="flex flex-col sm:flex-row justify-center gap-8 p-8 pb-16">
        <Button
          as="link"
          to="https://github.com/heiso/macrolev"
          target="_blank"
          className="inline-flex gap-2 justify-center items-center"
        >
          <Icon id="github-logo" className="self-center fill-gray-200" />
          GitHub
        </Button>

        <Button as="link" to={paths.build}>
          [WIP] Build your macrolev
        </Button>

        <Button primary as="link" to={paths.oldConfigurator}>
          Configure your macrolev
        </Button>
      </div>
    </div>
  )
}
