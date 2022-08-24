#![no_std]
#![no_main]

mod usb_manager;

use core::cell::RefCell;

use adafruit_kb2040 as bsp;
use bsp::entry;
use bsp::hal::pac::interrupt;
use bsp::{
    hal::{self, clocks, pac, timer, usb, watchdog},
    XOSC_CRYSTAL_FREQ,
};
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
use usbd_hid::descriptor::{KeyboardReport, SerializedDescriptor};
use usbd_hid::hid_class::HIDClass;

static USB_MANAGER: Mutex<RefCell<Option<usb_manager::UsbManager<usb::UsbBus>>>> =
    Mutex::new(RefCell::new(None));

struct Resources<'a> {
    keyboard: HIDClass<'a, usb::UsbBus>,
    usb_device: UsbDevice<'a, usb::UsbBus>,
}

#[repr(u8)]
pub enum KeyCode {
    None = 0x0,
    ErrorRollOver,       //0x01
    POSTFail,            //0x02
    ErrorUndefined,      //0x03
    A,                   //0x04
    B,                   //0x05
    C,                   //0x06
    D,                   //0x07
    E,                   //0x08
    F,                   //0x09
    G,                   //0x0A
    H,                   //0x0B
    I,                   //0x0C
    J,                   //0x0D
    K,                   //0x0E
    L,                   //0x0F
    M,                   //0x10
    N,                   //0x11
    O,                   //0x12
    P,                   //0x13
    Q,                   //0x14
    R,                   //0x15
    S,                   //0x16
    U,                   //0x18
    T,                   //0x17
    V,                   //0x19
    W,                   //0x1A
    X,                   //0x1B
    Y,                   //0x1C
    Z,                   //0x1D
    Kb1,                 //0x1E
    Kb2,                 //0x1F
    Kb3,                 //0x20
    Kb4,                 //0x21
    Kb5,                 //0x22
    Kb6,                 //0x23
    Kb7,                 //0x24
    Kb8,                 //0x25
    Kb9,                 //0x26
    Kb0,                 //0x27
    Enter,               //0x28
    Escape,              //0x29
    Backspace,           //0x2A
    Tab,                 //0x2B
    Spacebar,            //0x2C
    Minus,               //0x2D
    Equals,              //0x2E
    LeftBracket,         //0x2F
    RightBracket,        //0x30
    BackslashANSI,       //0x31
    Hash,                //0x32
    Semicolon,           //0x33
    Apostrophy,          //0x34
    Grave,               //0x35
    Comma,               //0x36
    Dot,                 //0x37
    ForwardSlash,        //0x38
    CapsLock,            //0x39
    F1,                  //0x3A
    F2,                  //0x3B
    F3,                  //0x3C
    F4,                  //0x3D
    F5,                  //0x3E
    F6,                  //0x3F
    F7,                  //0x40
    F8,                  //0x41
    F9,                  //0x42
    F10,                 //0x43
    F11,                 //0x44
    F12,                 //0x45
    PrintScreen,         //0x46
    ScrollLock,          //0x47
    Pause,               //0x48
    Insert,              //0x49
    Home,                //0x4A
    PageUp,              //0x4B
    Delete,              //0x4C
    End,                 //0x4D
    PageDown,            //0x4E
    RightArrow,          //0x4F
    LeftArrow,           //0x50
    DownArrow,           //0x51
    UpArrow,             //0x52
    KpNumLock,           //0x53
    KpBackslash,         //0x54
    KpAsterisk,          //0x55
    KpMinus,             //0x56
    KpPlus,              //0x57
    KpEnter,             //0x58
    Kp1,                 //0x59
    Kp2,                 //0x5A
    Kp3,                 //0x5B
    Kp4,                 //0x5C
    Kp5,                 //0x5D
    Kp6,                 //0x5E
    Kp7,                 //0x5F
    Kp8,                 //0x60
    Kp9,                 //0x61
    Kp0,                 //0x62
    KpDot,               //0x63
    BackslashISO,        //0x64
    Application,         //0x65
    Power,               //0x66
    KpEquals,            //0x67
    F13,                 //0x68
    F14,                 //0x69
    F15,                 //0x6A
    F16,                 //0x6B
    F17,                 //0x6C
    F18,                 //0x6D
    F19,                 //0x6E
    F20,                 //0x6F
    F21,                 //0x70
    F22,                 //0x71
    F23,                 //0x72
    F24,                 //0x73
    Execute,             //0x74
    Help,                //0x75
    Menu,                //0x76
    Select,              //0x77
    Stop,                //0x78
    Again,               //0x79
    Undo,                //0x7A
    Cut,                 //0x7B
    Copy,                //0x7C
    Paste,               //0x7D
    Find,                //0x7E
    Mute,                //0x7F
    VolumeUp,            //0x80
    VolumeDown,          //0x81
    LockingCapsLock,     //0x82
    LockingNum,          //0x83
    LockingScrollLock,   //0x84
    KpComma,             //0x85
    KpEqualSign,         //0x86
    International1,      //0x87
    International2,      //0x88
    International3,      //0x89
    International4,      //0x8A
    International5,      //0x8B
    International6,      //0x8C
    International7,      //0x8D
    International8,      //0x8E
    International9,      //0x8F
    LANG1,               //0x90
    LANG2,               //0x91
    LANG3,               //0x92
    LANG4,               //0x93
    LANG5,               //0x94
    LANG6,               //0x95
    LANG7,               //0x96
    LANG8,               //0x97
    LANG9,               //0x98
    AlternateErase,      //0x99
    SysReq,              //0x9A
    Cancel,              //0x9B
    Clear,               //0x9C
    Prior,               //0x9D
    Return,              //0x9E
    Separator,           //0x9F
    Out,                 //0xA0
    Oper,                //0xA1
    ClearAgain,          //0xA2
    CrSelProps,          //0xA3
    ExSel,               //0xA4
    Kp00 = 0xB0,         //0xB0
    Kp000,               //0xB1
    ThousandsSeparator,  //0xB2
    DecimalSeparator,    //0xB3
    CurrencyUnit,        //0xB4
    CurrencySubunit,     //0xB5
    KpLeftBracket,       //0xB6
    KpRightBracket,      //0xB7
    KpLeftCurlyBracket,  //0xB8
    KpRightCurlyBracket, //0xB9
    KpTab,               //0xBA
    KpBackspace,         //0xBB
    KpA,                 //0xBC
    KpB,                 //0xBD
    KpC,                 //0xBE
    KpD,                 //0xBF
    KpE,                 //0xC0
    KpF,                 //0xC1
    KpXOR,               //0xC2
    KpCaret,             //0xC3
    KpPercent,           //0xC4
    KpLessThan,          //0xC5
    KpGreaterThan,       //0xC6
    Kpampersand,         //0xC7
    KpDoubleampersand,   //0xC8
    KpPipe,              //0xC9
    KpDoublePipe,        //0xCA
    KpColon,             //0xCB
    KpHash,              //0xCC
    KpSpace,             //0xCD
    KpAt,                //0xCE
    KpExclamation,       //0xCF
    KpMemoryStore,       //0xD0
    KpMemoryRecall,      //0xD1
    KpMemoryClear,       //0xD2
    KpMemoryAdd,         //0xD3
    KpMemorySubtract,    //0xD4
    KpMemoryMultiply,    //0xD5
    KpMemoryDivide,      //0xD6
    KpPlusMinus,         //0xD7
    KpClear,             //0xD8
    KpClearEntry,        //0xD9
    KpBinary,            //0xDA
    KpOctal,             //0xDB
    KpDecimal,           //0xDC
    KpHexadecimal,       //0xDD
    LeftControl = 0xE0,  //0xE0
    LeftShift,           //0xE1
    LeftAlt,             //0xE2
    LeftGUI,             //0xE3
    RightControl,        //0xE4
    RightShift,          //0xE5
    RightAlt,            //0xE6
    RightGUI,            //0xE7
    MediaPlayPause = 0xE8,
    MediaStopCD,       //0xE9
    MediaPreviousSong, //0xEA
    MediaNextSong,     //0xEB
    MediaEjectCD,      //0xEC
    MediaVolUp,        //0xED
    MediaVolDown,      //0xEE
    MediaMute,         //0xEF
    MediaWWW,          //0xF0
    MediaBack,         //0xF1
    MediaForward,      //0xF2
    MediaStop,         //0xF3
    MediaFind,         //0xF4
    MediaScrollUp,     //0xF5
    MediaScrollDown,   //0xF6
    MediaEdit,         //0xF7
    MediaSleep,        //0xF8
    MediaCoffee,       //0xF9
    MediaRefresh,      //0xFA
    MediaCalc,         //0xFB
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

    let usb_bus = UsbBusAllocator::new(usb::UsbBus::new(
        pac.USBCTRL_REGS,
        pac.USBCTRL_DPRAM,
        clocks.usb_clock,
        true,
        &mut pac.RESETS,
    ));

    Resources::keyboard = HIDClass::new(&usb_bus, KeyboardReport::desc(), 20);

    let mut keyboard = HIDClass::new(&usb_bus, KeyboardReport::desc(), 20);

    // // Create a USB device with a fake VID and PID
    // resources.usb_device = UsbDeviceBuilder::new(&usb_bus, UsbVidPid(0x16c0, 0x27dd))
    //     .manufacturer("Adafruit")
    //     .product("Macropad")
    //     .serial_number("TEST")
    //     .device_class(0x00) // from: https://www.usb.org/defined-class-codes
    //     .build();

    cortex_m::interrupt::free(|cs| {
        static mut USB_BUS: Option<UsbBusAllocator<usb::UsbBus>> = None;

        unsafe {
            USB_BUS = Some(UsbBusAllocator::new(usb::UsbBus::new(
                pac.USBCTRL_REGS,
                pac.USBCTRL_DPRAM,
                clocks.usb_clock,
                true,
                &mut pac.RESETS,
            )));

            USB_MANAGER
                .borrow(cs)
                .replace(Some(usb_manager::UsbManager::new(
                    USB_BUS.as_ref().unwrap(),
                )));
        }
    });

    let timer = timer::Timer::new(pac.TIMER, &mut pac.RESETS);
    let mut delay = timer.count_down();

    loop {
        info!("test");
        delay.start(2500.milliseconds());
        let _ = nb::block!(delay.wait());
        // if countdown.wait().is_ok() {
        cortex_m::interrupt::free(|cs| {
            let mut usb_ref = USB_MANAGER.borrow(cs).borrow_mut();
            if let Some(usb) = usb_ref.as_mut() {
                usb.keyboard_borrow_mut()
                    .push_input(&KeyboardReport {
                        modifier: 0,
                        leds: 0,
                        reserved: 0,
                        keycodes: [0, 0, 0, 0, 0, 4],
                    })
                    .ok()
                    .unwrap();
            }
        });
    }
}

#[allow(non_snake_case)]
#[interrupt]
fn USBCTRL_IRQ() {
    cortex_m::interrupt::free(|cs| {
        let mut usb_ref = USB_MANAGER.borrow(cs).borrow_mut();
        if let Some(usb) = usb_ref.as_mut() {
            usb.service_irq();
        }
    });

    // cortex_m::interrupt::free(|cs| {
    //     let mut usb_device_ref = USB_DEVICE.borrow(cs).borrow_mut();
    //     let mut keyboard_ref = KEYBOARD.borrow(cs).borrow_mut();

    //     // Must be called more often than once every 10ms to handle events and stay USB compilant,
    //     // or from a device-specific interrupt handler.
    //     if let Some(usb_device) = usb_device_ref.as_mut() && let Some(keyboard) = keyboard_ref.as_mut() {
    //       usb_device.poll(&mut [keyboard]);
    //     }
    // });
}
