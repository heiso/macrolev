import { z } from 'zod'

export const id = z.number().safe().positive()

export const email = z
  .string()
  .email()
  .transform((value) => value.toLowerCase())

export function oneOfEnum<TEnum extends string>(values: TEnum[]) {
  return z.enum([values[0], ...values.slice(1)])
}
