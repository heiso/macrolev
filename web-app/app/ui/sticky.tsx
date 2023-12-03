import { useEffect, useRef, useState, type HTMLAttributes, type PropsWithChildren } from 'react'

export type StickyProps = PropsWithChildren<HTMLAttributes<HTMLElement>>

export function Sticky({ children, className }: StickyProps) {
  const ref = useRef(null)
  const [isHitingViewportBorder, setIsHitingViewportBorder] = useState(false)

  useEffect(() => {
    let headerRef = ref.current

    const observer = new IntersectionObserver(
      ([entry]) => {
        return setIsHitingViewportBorder(
          entry.boundingClientRect.top < (entry.rootBounds?.top || 0),
        )
      },
      {
        root: null,
        rootMargin: '-1px',
        threshold: 1,
      },
    )

    if (headerRef) observer.observe(headerRef)

    return () => {
      if (headerRef) observer.unobserve(headerRef)
    }
  }, [ref])

  return (
    <div
      ref={ref}
      className={`z-50 ${className ?? ''} ${
        isHitingViewportBorder ? 'sticky top-0 md:rounded-t-none group/sticky stuck' : ''
      }`}
    >
      {children}
    </div>
  )
}
