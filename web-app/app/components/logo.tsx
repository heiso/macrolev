import { useEffect, useRef, useState, type HTMLAttributes, type SVGProps } from 'react'
import type { LogoSvg } from '../../svgs.ts'

const ROTATION_RANGE = 15
const DISTANCE = 5
const RANGE = 500

type LogoProps = HTMLAttributes<HTMLDivElement>
export function Logo({ className, ...props }: LogoProps) {
  const [rotation, setRotation] = useState({ x: 0, y: 0, dx: 0, dy: 0 })
  const ref = useRef<HTMLDivElement>(null)
  const timeoutRef = useRef<NodeJS.Timeout>()
  const [bump, setBump] = useState(false)

  useEffect(() => {
    if (ref.current) {
      const boundingClientRect = ref.current.getBoundingClientRect()

      const newOrigin = {
        x: boundingClientRect.left + boundingClientRect.width / 2,
        y: boundingClientRect.top + boundingClientRect.height / 2,
      }

      const handleMouseMove: (event: MouseEvent) => any = (event) => {
        const cursorPos = { x: event.clientX, y: event.clientY }

        const newCursorPos = {
          x: cursorPos.x - newOrigin.x,
          y: -(cursorPos.y - newOrigin.y),
        }

        const bounded = {
          x: Math[newCursorPos.x >= 0 ? 'min' : 'max'](
            newCursorPos.x,
            (newCursorPos.x >= 0 ? 1 : -1) * RANGE,
          ),
          y: Math[newCursorPos.y >= 0 ? 'min' : 'max'](
            newCursorPos.y,
            (newCursorPos.y >= 0 ? 1 : -1) * RANGE,
          ),
        }

        const rotation = {
          x: (bounded.y * ROTATION_RANGE) / RANGE,
          y: (bounded.x * ROTATION_RANGE) / RANGE,
        }

        const translate3d = {
          x: (bounded.x * DISTANCE) / RANGE,
          y: -((bounded.y * DISTANCE) / RANGE),
        }

        setRotation({ ...rotation, dx: translate3d.x, dy: translate3d.y })
      }

      window.addEventListener('mousemove', handleMouseMove)

      return () => {
        window.removeEventListener('mousemove', handleMouseMove)
      }
    }
  }, [])

  useEffect(() => {
    if (bump && !timeoutRef.current) {
      timeoutRef.current = setTimeout(() => {
        setBump(false)
        timeoutRef.current = undefined
      }, 500)
    }
  }, [bump])

  return (
    <div
      ref={ref}
      className={`cursor-pointer transition duration-75 select-none w-fit ${
        bump ? 'scale-110' : ''
      } ${className ?? ''}`}
      onClick={() => setBump(true)}
      {...props}
    >
      <div
        className="relative"
        style={{
          transform: `rotateX(${rotation.x}deg) rotateY(${rotation.y}deg)`,
        }}
      >
        <LogoSVG id="U_U" className={`fill-gray-200 ${bump ? 'hidden' : 'block'}`} />

        <LogoSVG id="O_O" className={`fill-gray-200 ${bump ? 'block' : 'hidden'}`} />

        <LogoSVG
          id="w"
          style={{
            transform: `translate3d(${rotation.dx}px,${rotation.dy}px,0)`,
          }}
          className="fill-gray-200 absolute top-0 left-0 right-0 bottom-0 z-15"
        />

        <LogoSVG
          id="blushes-a"
          style={{
            transform: `translate3d(${rotation.dx * 2}px,${rotation.dy * 2}px,0)`,
          }}
          className={`fill-pink-500 absolute top-0 left-0 right-0 bottom-0 z-10 ${
            bump ? 'hidden' : 'block'
          }`}
        />

        <LogoSVG
          id="blushes-b"
          style={{
            transform: `translate3d(${rotation.dx * 2}px,${rotation.dy * 2}px,0)`,
          }}
          className={`fill-pink-500 absolute top-0 left-0 right-0 bottom-0 z-10 ${
            bump ? 'block' : 'hidden'
          }`}
        />
      </div>
    </div>
  )
}

type LogoSVGProps = SVGProps<SVGSVGElement> & { id: LogoSvg }

function LogoSVG({ id, ...props }: LogoSVGProps) {
  return (
    <svg width="226" height="108" viewBox="0 0 226 108" {...props}>
      <use href={`/logo.svg#${id}`} />
    </svg>
  )
}
