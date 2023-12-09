import { createContext } from 'react'

const devices: HIDDevice[] = []
export const DevicesContext = createContext(devices)
