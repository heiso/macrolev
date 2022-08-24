use esp_idf_sys as _; // If using the `binstart` feature of `esp-idf-sys`, always keep this module imported
use std::thread;
use std::time::Duration;
use embedded_hal::adc::OneShot;
use esp_idf_hal::peripherals;
use esp_idf_hal::adc;

fn main() -> Result<(), esp_idf_sys::EspError> {
  // Temporary. Will disappear once ESP-IDF 4.4 is released, but for now it is necessary to call this function once,
  // or else some patches to the runtime implemented by esp-idf-sys might not link properly.
  esp_idf_sys::link_patches();

  let peripherals = peripherals::Peripherals::take().unwrap();

  let mut powered_adc1 = adc::PoweredAdc::new(
    peripherals.adc1,
    adc::config::Config::new().calibration(true),
  )?;

  let mut gpio36 = peripherals.pins.gpio36.into_analog_atten_11db()?;

  loop {
    if let Ok(reading) = powered_adc1.read(&mut gpio36) {
      println!("{}", reading);
    }

    thread::sleep(Duration::from_millis(2));
  }
}