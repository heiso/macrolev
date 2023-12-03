import { conform, type FieldConfig } from '@conform-to/react'
import {
  forwardRef,
  useState,
  type HTMLInputTypeAttribute,
  type InputHTMLAttributes,
  type ReactNode,
  type SelectHTMLAttributes,
  type TextareaHTMLAttributes,
} from 'react'
import { Hint, type HintProps } from '../ui/hint.tsx'
import { Icon } from '../ui/icon.tsx'
import { Checkbox, Input } from '../ui/input.tsx'
import { Label } from '../ui/label.tsx'
import { SelectMultiple, type SelectMultipleProps } from '../ui/select-multiple.tsx'
import { Select, type Option } from '../ui/select.tsx'
import { Textarea } from '../ui/textarea.tsx'

export type BaseFieldProps = {
  field?: FieldConfig<unknown>
  label?: string
  type?: HTMLInputTypeAttribute
  hint?: HintProps['hint']
}

export type FieldProps = InputHTMLAttributes<HTMLInputElement> &
  BaseFieldProps & {
    icon?: ReactNode
  }

export const Field = forwardRef<HTMLInputElement, FieldProps>(
  ({ label, field, type, hint, ...props }, ref) => {
    return (
      <div className="flex flex-col gap-1 mb-1">
        <Label>{label}</Label>
        <Input
          ref={ref}
          error={field?.error}
          {...(field && conform.input(field, { type }))}
          {...props}
        />
        <Hint error={field?.error} hint={hint} />
      </div>
    )
  },
)
Field.displayName = 'Field'

export const PasswordField = forwardRef<HTMLInputElement, FieldProps>(({ ...props }, ref) => {
  const [show, setShow] = useState(false)

  return (
    <Field
      ref={ref}
      {...props}
      type={show ? 'text' : 'password'}
      icon={
        show ? (
          <Icon onClick={() => setShow(!show)} id="eye-open" className="cursor-pointer" />
        ) : (
          <Icon onClick={() => setShow(!show)} id="eye-closed" className="cursor-pointer" />
        )
      }
    />
  )
})
PasswordField.displayName = 'PasswordField'

export const CheckboxField = forwardRef<HTMLInputElement, FieldProps>(
  ({ label, field, hint, ...props }, ref) => {
    return (
      <div className="flex flex-col gap-1 mb-1">
        <Label className="cursor-pointer flex flex-row gap-2 select-none items-center">
          <Checkbox
            ref={ref}
            {...props}
            error={field?.error}
            {...(field && conform.input(field))}
          />
          <div>{label}</div>
        </Label>
        <Hint error={field?.error} hint={hint} />
      </div>
    )
  },
)
CheckboxField.displayName = 'CheckboxField'

export type TextareaFieldProps = TextareaHTMLAttributes<HTMLTextAreaElement> & BaseFieldProps

export const TextareaField = forwardRef<HTMLTextAreaElement, TextareaFieldProps>(
  ({ label, field, type, hint, ...props }, ref) => {
    return (
      <div className="flex flex-col gap-1 mb-1">
        <Label>{label}</Label>
        <Textarea
          ref={ref}
          error={field?.error}
          {...(field && conform.input(field, { type }))}
          {...props}
        />
        <Hint error={field?.error} hint={hint} />
      </div>
    )
  },
)
TextareaField.displayName = 'TextareaField'

export type SelectFieldProps = Omit<SelectHTMLAttributes<HTMLSelectElement>, 'children'> &
  BaseFieldProps & {
    options: Option[]
  }

export const SelectField = forwardRef<HTMLSelectElement, SelectFieldProps>(
  ({ label, field, hint, options, ...props }, ref) => {
    return (
      <div className="flex flex-col gap-1 mb-1">
        <Label>{label}</Label>
        <Select
          {...props}
          ref={ref}
          className="border border-gray-300 rounded px-2 py-1"
          {...(field && conform.input(field))}
        >
          {options.map(({ value, label }) => (
            <option key={value} value={value}>
              {label}
            </option>
          ))}
        </Select>
        <Hint error={field?.error} hint={hint} />
      </div>
    )
  },
)
SelectField.displayName = 'SelectField'

export type SelectMultipleFieldProps = BaseFieldProps & SelectMultipleProps

export function SelectMultipleField({ label, field, hint, ...props }: SelectMultipleFieldProps) {
  return (
    <div className="flex flex-col gap-1 mb-1">
      <Label>{label}</Label>
      <SelectMultiple {...props} error={field?.error} {...(field && conform.input(field))} />
      <Hint error={field?.error} hint={hint} />
    </div>
  )
}
