#include "mhz19.h"
#include "esphome/core/log.h"

namespace esphome {
namespace mhz19 {

static const char *const TAG = "mhz19";

static const uint8_t REQUEST_LENGTH = 5;
static const uint8_t RESPONSE_LENGTH = 16;
static const uint8_t GET_PPM[] = {0x64, 0x69, 0x03, 0x5E, 0x4E};
bool initial = true;

uint8_t mhz19_checksum(const uint8_t *command) {
  uint8_t sum = 0;
  for (uint8_t i = 0; i < RESPONSE_LENGTH-2; i++) {
    sum += command[i];
  }
  return sum;
}

void MHZ19Component::setup() {
  return;
}

void MHZ19Component::update() {
  uint32_t now_ms = millis();
  uint32_t warmup_ms = this->warmup_seconds_ * 1000;
  if (now_ms < warmup_ms) {
    ESP_LOGW(TAG, "MHZ19 warming up, %ds left", (warmup_ms - now_ms) / 1000);
    this->status_set_warning();
    return;
  }

  uint8_t response[RESPONSE_LENGTH];
  if (!this->mhz19_write_command_(GET_PPM, response)) {
    ESP_LOGW(TAG, "Reading data from MHZ19 failed!");
    this->status_set_warning();
    return;
  }

  if (response[0] != 0x64 || response[1] != 0x69) {
    ESP_LOGW(TAG, "Invalid preamble from MHZ19!");
    this->status_set_warning();
    return;
  }

  uint8_t checksum = mhz19_checksum(response);
  if (response[15] != checksum) {
    ESP_LOGW(TAG, "MHZ19 Checksum doesn't match: 0x%02X!=0x%02X", response[15], checksum);
    this->status_set_warning();
    return;
  }

  this->status_clear_warning();
  const uint16_t ppm = (uint16_t(response[6]*256) + response[7]);

  ESP_LOGD(TAG, "MHZ19 Received CO₂=%uppm", ppm);
  if (this->co2_sensor_ != nullptr)
    this->co2_sensor_->publish_state(ppm);
}

bool MHZ19Component::mhz19_write_command_(const uint8_t *command, uint8_t *response) {  
  if(!this->available()) {
    printf("========= UNAVAILABLE =========\n");
    return false;
  } else {
    printf("AVAILABLE: ");
  }

  // Read from RX Buffer
  this->read_array(response, RESPONSE_LENGTH);

  // Empty RX Buffer
  this->write_array(command, REQUEST_LENGTH);
  this->flush();
  if (response == nullptr)
    return true;
  return this->read_array(response, RESPONSE_LENGTH);
}
float MHZ19Component::get_setup_priority() const { return setup_priority::DATA; }
void MHZ19Component::dump_config() {
  ESP_LOGCONFIG(TAG, "MH-Z19:");
  LOG_SENSOR("  ", "CO2", this->co2_sensor_);
  this->check_uart_settings(9600);

  ESP_LOGCONFIG(TAG, "  Warmup seconds: %ds", this->warmup_seconds_);
}

}  // namespace mhz19
}  // namespace esphome