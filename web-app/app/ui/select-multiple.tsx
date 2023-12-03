import { useCallback, useEffect, useMemo, useRef, useState, type SelectHTMLAttributes } from 'react'
import { Icon } from './icon.tsx'
import { Checkbox } from './input.tsx'
import { Label } from './label.tsx'
import { type Option } from './select.tsx'

export type SelectMultipleProps = Pick<
  SelectHTMLAttributes<HTMLSelectElement>,
  'placeholder' | 'defaultValue' | 'className' | 'name'
> & {
  options: Option[]
  error?: string
}

export function SelectMultiple({
  className,
  options,
  defaultValue,
  name,
  placeholder,
}: SelectMultipleProps) {
  const indexedOptions = useMemo(
    () => options.map((option, index) => ({ ...option, id: index })),
    [options],
  )
  const [filteredOptions, setFilteredOptions] = useState(indexedOptions)
  const [focusedOptionIndex, setFocusedOptionIndex] = useState<number>()
  const [selectedOptionIds, setSelectedOptionIds] = useState<
    (typeof indexedOptions)[number]['id'][]
  >(() => {
    if (defaultValue != undefined) {
      const values = Array.isArray(defaultValue)
        ? (defaultValue as string[])
        : [defaultValue.toString()]

      return values.reduce<(typeof indexedOptions)[number]['id'][]>((acc, value) => {
        const option = indexedOptions.find((option) => option.value.toString() === value)
        if (option) {
          acc.push(option.id)
        }
        return acc
      }, [])
    }
    return []
  })
  const [isOpen, setIsOpen] = useState(false)
  const containerRef = useRef<HTMLDivElement>(null)
  const inputRef = useRef<HTMLInputElement>(null)
  const selectedOptions = useMemo(
    () =>
      selectedOptionIds.reduce<typeof indexedOptions>((acc, id) => {
        const option = indexedOptions.find((option) => option.id === id)
        if (option) {
          acc.push(option)
        }
        return acc
      }, []),
    [selectedOptionIds, indexedOptions],
  )

  const open = useCallback(() => {
    if (!isOpen) {
      setIsOpen(true)
      const timeout = setTimeout(() => {
        if (inputRef.current) {
          inputRef.current.focus()
        }
        clearTimeout(timeout)
      }, 100)
    }
  }, [isOpen, inputRef])

  const close = useCallback(() => {
    if (inputRef.current) {
      inputRef.current.blur()
      inputRef.current.value = ''
    }
    setFocusedOptionIndex(undefined)
    setFilteredOptions(indexedOptions)
    setIsOpen(false)
  }, [inputRef, indexedOptions])

  const moveDown = useCallback(() => {
    if (filteredOptions.length === 0) return

    setFocusedOptionIndex(
      focusedOptionIndex != undefined ? (focusedOptionIndex + 1) % filteredOptions.length : 0,
    )
  }, [filteredOptions, focusedOptionIndex])

  const moveUp = useCallback(() => {
    if (filteredOptions.length === 0) return

    setFocusedOptionIndex(
      focusedOptionIndex != undefined && focusedOptionIndex > 0
        ? (focusedOptionIndex - 1) % filteredOptions.length
        : filteredOptions.length - 1,
    )
  }, [filteredOptions, focusedOptionIndex])

  const select = useCallback(() => {
    if (focusedOptionIndex != undefined) {
      if (selectedOptionIds.includes(filteredOptions[focusedOptionIndex].id)) {
        setSelectedOptionIds(
          selectedOptionIds.filter((id) => id !== filteredOptions[focusedOptionIndex].id),
        )
      } else {
        setSelectedOptionIds([...selectedOptionIds, filteredOptions[focusedOptionIndex].id])
      }
    }
  }, [focusedOptionIndex, filteredOptions, selectedOptionIds])

  useEffect(() => {
    function onMouseDown(event: MouseEvent) {
      if (event.target instanceof Node) {
        if (!containerRef.current?.contains(event.target)) {
          close()
        }
      }
    }

    document.addEventListener('mousedown', onMouseDown)

    return () => {
      document.removeEventListener('mousedown', onMouseDown)
    }
  })

  return (
    <div
      className={`select-none ${className ?? ''}}`}
      onKeyDown={(event) => {
        if (event.key === 'ArrowUp') {
          event.preventDefault()
          event.stopPropagation()

          moveUp()
        }

        if (event.key === 'ArrowDown') {
          event.preventDefault()
          event.stopPropagation()

          moveDown()
        }

        if (event.key === 'Enter' || event.key === 'ArrowRight') {
          event.preventDefault()
          event.stopPropagation()

          select()
        }

        if (event.key === 'Escape') {
          event.preventDefault()
          event.stopPropagation()

          close()
        }
      }}
    >
      <div ref={containerRef} className="relative">
        <div
          className={`flex flex-row items-center pl-2 pr-11 py-2 border rounded-lg cursor-pointer text-pink-500 hover:border-pink-500 focus:border-pink-500 ${
            selectedOptionIds.length > 0 ? 'bg-pink-100' : 'bg-white'
          }`}
          onMouseDown={() => open()}
        >
          <div className="flex flex-row flex-wrap gap-2">
            {selectedOptions.map((option) => (
              <div
                key={option.id}
                className="rounded-md px-2 py-1 text-sm bg-pink-500 text-pink-50 capitalize flex flex-row items-center gap-1 leading-[1.125rem]"
              >
                {option.label}
                <Icon
                  id="cross-2"
                  className="fill-pink-50 transition hover:bg-pink-800 rounded-full"
                  onMouseDown={(event) => {
                    event.preventDefault()
                    event.stopPropagation()

                    setSelectedOptionIds(selectedOptionIds.filter((id) => id !== option.id))
                  }}
                />
              </div>
            ))}
          </div>
          {selectedOptions.length === 0 && (
            <span className="text-pink-300 text-base leading-[1.125rem] py-1 pl-2">
              {placeholder}
            </span>
          )}
          {selectedOptionIds.length > 0 ? (
            <div className="absolute top-0 right-0 bottom-0 z-10 px-4 py-3 flex justify-center items-center">
              <Icon
                id="cross-2"
                className="transition-colors fill-pink-500 hover:fill-pink-800 cursor-pointer"
                onMouseDown={(event) => {
                  event.preventDefault()
                  event.stopPropagation()

                  setSelectedOptionIds([])
                  close()
                }}
              />
            </div>
          ) : (
            <div className="absolute top-0 right-0 bottom-0 z-10 px-4 py-3 flex justify-center items-center">
              <Icon id="chevron-down" className="fill-pink-500" />
            </div>
          )}
        </div>

        <div
          className={`absolute left-0 right-0 top-full z-10 bg-white mt-1 rounded-md ${
            isOpen ? 'block' : 'hidden'
          }`}
        >
          <div className="relative">
            <input
              className={`w-full h-full px-4 py-3 pr-11 appearance-none outline-none disabled:pointer-events-none border-b border-pink-300 text-pink-500 hover:border-pink-500 focus:border-pink-500 placeholder:text-pink-300 bg-transparent`}
              placeholder="Search"
              ref={inputRef}
              onChange={(event) => {
                setFilteredOptions(
                  indexedOptions.filter(({ label }) =>
                    label.toLowerCase().includes(event.currentTarget.value.toLowerCase()),
                  ),
                )
                setFocusedOptionIndex(undefined)
              }}
            />
            <div
              className={`absolute top-0 right-0 bottom-0 z-10 px-4 py-3 select-none flex justify-center items-center`}
            >
              <Icon id="magnifying-glass" className="fill-pink-500" />
            </div>
          </div>
          <div className="flex flex-col overflow-x-hidden overflow-y-scroll">
            {filteredOptions.map((option) => {
              const focus =
                focusedOptionIndex != undefined
                  ? filteredOptions[focusedOptionIndex].value === option.value
                  : false
              return (
                <Label
                  key={option.id}
                  className={`cursor-pointer flex flex-row gap-2 select-none items-center capitalize ${
                    focus ? 'bg-pink-100' : ''
                  } text-pink-500 ${
                    focusedOptionIndex != undefined && !focus
                      ? 'hover:bg-pink-50'
                      : 'hover:bg-pink-100'
                  } px-3 py-2 first:rounded-t-md last:rounded-b-md`}
                >
                  <Checkbox
                    name={name}
                    value={option.value}
                    checked={selectedOptionIds.includes(option.id)}
                    onChange={(event) => {
                      if (event.currentTarget.checked) {
                        setSelectedOptionIds([...selectedOptionIds, option.id])
                      } else {
                        setSelectedOptionIds(selectedOptionIds.filter((id) => id !== option.id))
                      }
                    }}
                  />
                  <div>{option.label}</div>
                </Label>
              )
            })}
            {filteredOptions.length === 0 && (
              <div className="text-pink-500 text-xs text-center px-3 py-3">Nothing found X_X</div>
            )}
          </div>
        </div>
      </div>
    </div>
  )
}
