#pragma once

#include "esphome/core/component.h"
#include "esphome/core/defines.h"

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

#include <SX127x.h>

#define LORA_BW 125000
#define LORA_CODE_RATE 5
#define LORA_CRC_ENABLE true
#define LORA_FREQ 869525000
#define LORA_HDR_TYPE SX127X_HEADER_IMPLICIT
#define LORA_PREAMBLE_LEN 8
#define LORA_SPREAD_FACTOR 7
#define LORA_SYNC_WORD 0x4C

#define HDR_SIZE 2
#define PULSE_SIZE 4
#define MISC_SIZE 3
#if defined(CRC16_ENABLE)
	#define CRC_SIZE 2
#else
	#define CRC_SIZE 0
#endif /* CRC16_ENABLE */

#define DATA_SIZE (HDR_SIZE + PULSE_SIZE + MISC_SIZE)
#define PKT_SIZE (DATA_SIZE + CRC_SIZE)

#define ADC_MASK 0x3FF
#define VCC_SHIFT 0
#define TEMP_SHIFT 10

#define ADC_REF 3.36f
#define ADC_RES 1023

#define LORA_POLLING 100
#define LORA_START_INTERVAL 10000
#define LORA_WAIT 5

#define VOLTAGE_THRESHOLD 1.1

namespace esphome {
	namespace lora_pulse_counter
	{
		class LoraPulseCounter : public PollingComponent
		{
			private:
				SX127x lora;
				unsigned long lora_millis = 0;
				bool lora_on = false;
				int lora_rst = -1;
				int lora_ss = -1;

				sensor::Sensor *sensor_pulses;
				sensor::Sensor *sensor_rssi;
				sensor::Sensor *sensor_snr;
				sensor::Sensor *sensor_temperature;
				sensor::Sensor *sensor_voltage;
				binary_sensor::BinarySensor *sensor_voltage_low;

				void lora_start(void);
				void lora_rx(uint8_t *data, uint8_t len);

			public:
				void set_lora_rst(int lora_rst);
				void set_lora_ss(int lora_ss);

				void set_sensor_pulses(sensor::Sensor *sensor_pulses);
				void set_sensor_rssi(sensor::Sensor *sensor_rssi);
				void set_sensor_snr(sensor::Sensor *sensor_snr);
				void set_sensor_temperature(sensor::Sensor *sensor_temperature);
				void set_sensor_voltage(sensor::Sensor *sensor_voltage);
				void set_sensor_voltage_low(binary_sensor::BinarySensor *sensor_voltage_low);

				void dump_config(void) override;
				float get_setup_priority(void) const override;
				void setup(void) override;
				void update(void) override;
		};
	}
}
