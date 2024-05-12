import {
  forwardRef,
  useState,
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
  label?: string
  hint?: HintProps['hint']
  errors?: string[]
}

export type FieldProps = InputHTMLAttributes<HTMLInputElement> &
  BaseFieldProps & {
    icon?: ReactNode
  }

export const Field = forwardRef<HTMLInputElement, FieldProps>(
  ({ label, hint, errors, ...props }, ref) => {
    return (
      <div className="flex flex-col gap-1 mb-1">
        <Label>{label}</Label>
        <Input ref={ref} hasError={Boolean(errors?.length)} {...props} />
        <Hint error={errors?.join('\n')} hint={hint} />
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
  ({ label, hint, errors, ...props }, ref) => {
    return (
      <div className="flex flex-col gap-1 mb-1">
        <Label className="cursor-pointer flex flex-row gap-2 select-none items-center">
          <Checkbox ref={ref} {...props} hasError={Boolean(errors?.length)} />
          <div>{label}</div>
        </Label>
        <Hint error={errors?.join('\n')} hint={hint} />
      </div>
    )
  },
)
CheckboxField.displayName = 'CheckboxField'

export type TextareaFieldProps = TextareaHTMLAttributes<HTMLTextAreaElement> & BaseFieldProps

export const TextareaField = forwardRef<HTMLTextAreaElement, TextareaFieldProps>(
  ({ label, hint, errors, ...props }, ref) => {
    return (
      <div className="flex flex-col gap-1 mb-1">
        <Label>{label}</Label>
        <Textarea ref={ref} hasError={Boolean(errors?.length)} {...props} />
        <Hint error={errors?.join('\n')} hint={hint} />
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
  ({ label, hint, options, errors, ...props }, ref) => {
    return (
      <div className="flex flex-col gap-1 mb-1">
        <Label>{label}</Label>
        <Select
          hasError={Boolean(errors?.length)}
          {...props}
          ref={ref}
          className="border border-gray-300 rounded px-2 py-1"
        >
          {options.map(({ value, label }) => (
            <option key={value} value={value}>
              {label}
            </option>
          ))}
        </Select>
        <Hint error={errors?.join('\n')} hint={hint} />
      </div>
    )
  },
)
SelectField.displayName = 'SelectField'

export type SelectMultipleFieldProps = BaseFieldProps & SelectMultipleProps

export function SelectMultipleField({ label, hint, errors, ...props }: SelectMultipleFieldProps) {
  return (
    <div className="flex flex-col gap-1 mb-1">
      <Label>{label}</Label>
      <SelectMultiple {...props} hasError={Boolean(errors?.length)} />
      <Hint error={errors?.join('\n')} hint={hint} />
    </div>
  )
}
