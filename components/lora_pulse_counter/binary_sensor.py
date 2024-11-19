import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import DEVICE_CLASS_BATTERY, ENTITY_CATEGORY_NONE

from . import CONF_LPC_ID, LoraPulseCounter

DEPENDENCIES = ["lora_pulse_counter"]

CONF_VOLTAGE_LOW = "voltage_low"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_LPC_ID): cv.use_id(LoraPulseCounter),
        cv.Optional(CONF_VOLTAGE_LOW): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_BATTERY,
            entity_category=ENTITY_CATEGORY_NONE,
        ),
    }
)


async def to_code(config):
    lpc = await cg.get_variable(config[CONF_LPC_ID])

    if conf_voltage_low := config.get(CONF_VOLTAGE_LOW):
        sensor_voltage_low = await binary_sensor.new_binary_sensor(conf_voltage_low)
        cg.add(lpc.set_sensor_voltage_low(sensor_voltage_low))
