import { Keyboard } from '../components/keyboard'
import { SwitchOffsetsConfiguration } from '../components/switch-offsets-configuration'

export default function Index() {
  return (
    <div className="p-4 sm:p-8 flex flex-col gap-8 items-center">
      <Keyboard />
      <SwitchOffsetsConfiguration />
    </div>
  )
}
