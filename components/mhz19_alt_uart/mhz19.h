#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace mhz19 {

enum MHZ19ABCLogic { MHZ19_ABC_NONE = 0, MHZ19_ABC_ENABLED, MHZ19_ABC_DISABLED };

class MHZ19Component : public PollingComponent, public uart::UARTDevice {
 public:
  float get_setup_priority() const override;

  void setup() override;
  void update() override;
  void dump_config() override;


  void set_co2_sensor(sensor::Sensor *co2_sensor) { co2_sensor_ = co2_sensor; }
  void set_warmup_seconds(uint32_t seconds) { warmup_seconds_ = seconds; }

 protected:
  bool mhz19_write_command_(const uint8_t *command, uint8_t *response);
  sensor::Sensor *co2_sensor_{nullptr};
  uint32_t warmup_seconds_;
};

}  // namespace mhz19
}  // namespace esphome
