import { Link, Outlet } from '@remix-run/react'
import { version } from '../../package.json'
import { routerPaths } from '../../routes.ts'
import { Icon } from '../ui/icon.tsx'

export default function Index() {
  return (
    <div className="h-full w-full flex flex-col">
      <header className="w-full px-6 py-4 flex flex-row items-center justify-between">
        <Link
          to={routerPaths['/']}
          className="text-xl font-light text-gray-400 transition hover:text-gray-100 active:opacity-80"
        >
          <span className="font-bold text-gray-200">Macrolev</span>
          <small className="ml-2 self-end text-sm font-extrabold text-pink-50-200">
            v{version}
          </small>
        </Link>

        <div className="flex flex-row items-center gap-6">
          <Link
            to="https://github.com/heiso/macrolev"
            target="_blank"
            className="fill-gray-400 transition hover:fill-gray-200 active:fill-pink-200"
          >
            <Icon id="github-logo" />
          </Link>
        </div>
      </header>

      <main className="w-full h-full">
        <Outlet />
      </main>
    </div>
  )
}
