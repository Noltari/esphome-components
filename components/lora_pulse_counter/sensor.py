import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    DEVICE_CLASS_SIGNAL_STRENGTH,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    ENTITY_CATEGORY_DIAGNOSTIC,
    ENTITY_CATEGORY_NONE,
    ICON_COUNTER,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_CELSIUS,
    UNIT_DECIBEL,
    UNIT_DECIBEL_MILLIWATT,
    UNIT_EMPTY,
    UNIT_VOLT,
)

from . import CONF_LPC_ID, LoraPulseCounter

DEPENDENCIES = ["lora_pulse_counter"]

CONF_PULSES = "pulses"
CONF_RSSI = "rssi"
CONF_SNR = "snr"
CONF_TEMPERATURE = "temperature"
CONF_VOLTAGE = "voltage"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_LPC_ID): cv.use_id(LoraPulseCounter),
        cv.Optional(CONF_PULSES): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_COUNTER,
            accuracy_decimals=0,
            state_class=STATE_CLASS_TOTAL_INCREASING,
            entity_category=ENTITY_CATEGORY_NONE,
        ),
        cv.Optional(CONF_RSSI): sensor.sensor_schema(
            unit_of_measurement=UNIT_DECIBEL_MILLIWATT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_SNR): sensor.sensor_schema(
            unit_of_measurement=UNIT_DECIBEL,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_SIGNAL_STRENGTH,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_NONE,
        ),
        cv.Optional(CONF_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_NONE,
        ),
    }
)


async def to_code(config):
    lpc = await cg.get_variable(config[CONF_LPC_ID])

    if conf_pulses := config.get(CONF_PULSES):
        sensor_pulses = await sensor.new_sensor(conf_pulses)
        cg.add(lpc.set_sensor_pulses(sensor_pulses))

    if conf_rssi := config.get(CONF_RSSI):
        sensor_rssi = await sensor.new_sensor(conf_rssi)
        cg.add(lpc.set_sensor_rssi(sensor_rssi))

    if conf_snr := config.get(CONF_SNR):
        sensor_snr = await sensor.new_sensor(conf_snr)
        cg.add(lpc.set_sensor_snr(sensor_snr))

    if conf_temperature := config.get(CONF_TEMPERATURE):
        sensor_temperature = await sensor.new_sensor(conf_temperature)
        cg.add(lpc.set_sensor_temperature(sensor_temperature))

    if conf_voltage := config.get(CONF_VOLTAGE):
        sensor_voltage = await sensor.new_sensor(conf_voltage)
        cg.add(lpc.set_sensor_voltage(sensor_voltage))
