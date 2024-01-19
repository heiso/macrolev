import { Outlet, Link as RemixLink } from '@remix-run/react'
import { version } from '../../package.json'
import { routerPaths } from '../../routes.ts'
import { Icon } from '../ui/icon.tsx'

export default function Index() {
  return (
    <div className="h-full w-full flex flex-col">
      <header className="w-full px-6 py-4 flex flex-row items-center justify-between mb-10">
        <RemixLink
          to={routerPaths['/']}
          className="text-xl font-light text-gray-400 transition hover:text-gray-100 active:opacity-80"
        >
          <span className="font-bold text-gray-200">Macrolev</span>
          <small className="ml-2 self-end text-sm font-extrabold text-pink-50-200">
            v{version}
          </small>
        </RemixLink>

        <div className="flex flex-row items-center gap-6">
          {/* <NavLink
            to={routerPaths['/hid']}
            className={({ isActive }) =>
              `transition hover:text-gray-100 active:text-pink-200 ${
                isActive ? 'text-gray-100 pointer-events-none' : 'text-gray-400'
              }`
            }
          >
            Keyboard
          </NavLink> */}
          {/* <div className="h-6 w-0.5 bg-slate-700"></div> */}
          <RemixLink
            to="https://github.com/heiso/macrolev"
            target="_blank"
            className="fill-gray-400 transition hover:fill-gray-200 active:fill-pink-200"
          >
            <Icon id="github-logo" />
          </RemixLink>
        </div>
      </header>

      <main className="w-full h-full p-6">
        <Outlet />
      </main>
    </div>
  )
}
