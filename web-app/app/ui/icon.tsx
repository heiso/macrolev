import type { SVGProps } from 'react'
import type { IconsSvg } from '../../svgs.ts'

export type IconProps = SVGProps<SVGSVGElement> & { id: IconsSvg }

export function Icon({ id, className, ...props }: IconProps) {
  return (
    <svg width="15" height="15" viewBox="0 0 15 15" className={`${className}`} {...props}>
      <use href={`/icons.svg#${id}`} />
    </svg>
  )
}
