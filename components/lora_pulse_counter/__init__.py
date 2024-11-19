import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

MULTI_CONF = True

CONF_LPC_ID = "lpc_id"
CONF_LORA_RST = "lora_rst"
CONF_LORA_SS = "lora_ss"

lora_pulse_counter_ns = cg.esphome_ns.namespace("lora_pulse_counter")

LoraPulseCounter = lora_pulse_counter_ns.class_("LoraPulseCounter", cg.PollingComponent)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(LoraPulseCounter),
        cv.Optional(CONF_LORA_RST): cv.int_range(min=0),
        cv.Optional(CONF_LORA_SS): cv.int_range(min=0),
    }
).extend(cv.polling_component_schema("100ms"))


async def to_code(config):
    cg.add_library("SPI", None)
    cg.add_library("LoRaRF", None)

    lpc = cg.new_Pvariable(config[CONF_ID])

    await cg.register_component(lpc, config)

    if lora_rst := config.get(CONF_LORA_RST):
        cg.add(lpc.set_lora_rst(lora_rst))

    if lora_ss := config.get(CONF_LORA_SS):
        cg.add(lpc.set_lora_ss(lora_ss))
