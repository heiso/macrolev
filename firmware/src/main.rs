#![no_std]
#![no_main]

use core::cell::RefCell;

use adafruit_kb2040 as bsp;
use bsp::entry;
use bsp::hal::clocks;
use bsp::hal::pac::interrupt;
use bsp::XOSC_CRYSTAL_FREQ;

use bsp::hal::timer;
use bsp::hal::usb;
use bsp::hal::watchdog;
use bsp::pac;
use cortex_m::interrupt::Mutex;
use cortex_m::prelude::_embedded_hal_timer_CountDown;
use defmt::*;
use defmt_rtt as _;
use embedded_time::duration::Extensions;
use panic_probe as _;

use usb_device::class_prelude::*;
use usb_device::prelude::*;
use usbd_hid::descriptor::KeyboardReport;
use usbd_hid::descriptor::SerializedDescriptor;
use usbd_hid::hid_class::HIDClass;

static USB_MANAGER: Mutex<RefCell<Option<UsbManager>>> = Mutex::new(RefCell::new(None));

struct UsbManager<'a> {
    keyboard: HIDClass<'a, usb::UsbBus>,
    usb_device: UsbDevice<'a, usb::UsbBus>,
}
impl<'a> UsbManager<'a> {
    pub fn new(usb_bus: usb::UsbBus) -> UsbManager<'a> {
        static mut USB_BUS_ALLOC: Option<UsbBusAllocator<usb::UsbBus>> = None;

        unsafe {
            USB_BUS_ALLOC = Some(UsbBusAllocator::new(usb_bus));
        }

        UsbManager {
            keyboard: HIDClass::new(
                unsafe { USB_BUS_ALLOC.as_ref().unwrap() },
                KeyboardReport::desc(),
                20,
            ),

            usb_device: UsbDeviceBuilder::new(
                unsafe { USB_BUS_ALLOC.as_ref().unwrap() },
                UsbVidPid(0x16c0, 0x27dd),
            )
            .manufacturer("Adafruit")
            .product("Macropad")
            .serial_number("TEST")
            .device_class(0x00) // from: https://www.usb.org/defined-class-codes
            .build(),
        }
    }

    pub fn poll(&mut self) {
        self.usb_device.poll(&mut [&mut self.keyboard]);
    }
}

#[entry]
fn main() -> ! {
    info!("Program start");
    let mut pac = pac::Peripherals::take().unwrap();
    let mut watchdog = watchdog::Watchdog::new(pac.WATCHDOG);

    unsafe {
        pac::NVIC::unmask(pac::Interrupt::USBCTRL_IRQ);
    }

    let clocks = clocks::init_clocks_and_plls(
        XOSC_CRYSTAL_FREQ,
        pac.XOSC,
        pac.CLOCKS,
        pac.PLL_SYS,
        pac.PLL_USB,
        &mut pac.RESETS,
        &mut watchdog,
    )
    .ok()
    .unwrap();

    let usb_bus = usb::UsbBus::new(
        pac.USBCTRL_REGS,
        pac.USBCTRL_DPRAM,
        clocks.usb_clock,
        true,
        &mut pac.RESETS,
    );

    cortex_m::interrupt::free(|cs| {
        USB_MANAGER
            .borrow(cs)
            .replace(Some(UsbManager::new(usb_bus)))
    });

    let timer = timer::Timer::new(pac.TIMER, &mut pac.RESETS);

    let mut fast_countdown = timer.count_down();
    fast_countdown.start(1.milliseconds());

    let mut slow_countdown = timer.count_down();
    slow_countdown.start(2000.milliseconds());

    loop {
        if slow_countdown.wait().is_ok() {
            cortex_m::interrupt::free(|cs| {
                let usb_manager = USB_MANAGER.borrow(cs).take().unwrap();

                usb_manager
                    .keyboard
                    .push_input(&KeyboardReport {
                        modifier: 0,
                        leds: 0,
                        reserved: 0,
                        keycodes: [0, 0, 0, 0, 0, 4],
                    })
                    .ok()
                    .unwrap();
            });
        }
    }
}

#[allow(non_snake_case)]
#[interrupt]
fn USBCTRL_IRQ() {
    cortex_m::interrupt::free(|cs| {
        if let Some(usb_manager) = USB_MANAGER.borrow(cs).borrow_mut().as_mut() {
            usb_manager.poll();
        }
    });
}
