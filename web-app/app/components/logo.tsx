import { useEffect, useRef, useState, type HTMLAttributes, type SVGProps } from 'react'
import type { LogoSvg } from '../../svgs.ts'

const ROTATION_RANGE = 20
const DISTANCE = 10
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
      }, 1000)
    }
  }, [bump])

  return (
    <div
      ref={ref}
      className={`px-4 cursor-pointer transition duration-75 select-none w-fit ${
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
        <LogoSVG id="keys" className="stroke-gray-200 fill-transparent" />

        <LogoSVG
          id="legends"
          style={{
            transform: `translate3d(${rotation.dx}px,${rotation.dy}px,0)`,
          }}
          className={`fill-gray-200 stroke-gray-200 absolute top-0 left-0 right-0 bottom-0 z-10 ${
            bump ? 'hidden' : 'block'
          }`}
        />

        <LogoSVG
          id="bumped_legends"
          style={{
            transform: `translate3d(${rotation.dx}px,${rotation.dy}px,0)`,
          }}
          className={`fill-gray-200 stroke-gray-200 absolute top-0 left-0 right-0 bottom-0 z-15 ${
            bump ? 'block' : 'hidden'
          }`}
        />

        <LogoSVG
          id="blushes"
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
    <svg width="170" height="120" viewBox="0 0 170 120" {...props}>
      <use href={`logo.svg#${id}`} />
    </svg>
  )
}
