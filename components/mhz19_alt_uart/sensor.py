import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.automation import maybe_simple_id
from esphome.components import sensor, uart
from esphome.const import (
    CONF_CO2,
    CONF_ID,
    DEVICE_CLASS_CARBON_DIOXIDE,
    ICON_MOLECULE_CO2,
    STATE_CLASS_MEASUREMENT,
    UNIT_PARTS_PER_MILLION
)

DEPENDENCIES = ["uart"]

CONF_WARMUP_TIME = "warmup_time"

mhz19_ns = cg.esphome_ns.namespace("mhz19")
MHZ19Component = mhz19_ns.class_("MHZ19Component", cg.PollingComponent, uart.UARTDevice)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(MHZ19Component),
            cv.Required(CONF_CO2): sensor.sensor_schema(
                unit_of_measurement=UNIT_PARTS_PER_MILLION,
                icon=ICON_MOLECULE_CO2,
                accuracy_decimals=0,
                device_class=DEVICE_CLASS_CARBON_DIOXIDE,
                state_class=STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(
                CONF_WARMUP_TIME, default="75s"
            ): cv.positive_time_period_seconds,
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    if CONF_CO2 in config:
        sens = await sensor.new_sensor(config[CONF_CO2])
        cg.add(var.set_co2_sensor(sens))

    cg.add(var.set_warmup_seconds(config[CONF_WARMUP_TIME]))


# CALIBRATION_ACTION_SCHEMA = maybe_simple_id(
#     {
#         cv.Required(CONF_ID): cv.use_id(MHZ19Component),
#     }
# )

async def mhz19_calibration_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(action_id, template_arg, paren)
