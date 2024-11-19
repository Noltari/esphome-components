#include "lora_pulse_counter.h"
#include "esphome/core/log.h"

namespace esphome {
	namespace lora_pulse_counter {
		static const char *TAG = "lora_pulse_counter.component";

		void LoraPulseCounter::lora_start(void)
		{
			this->lora_millis = millis();

			if (this->lora.begin(this->lora_ss, this->lora_rst, -1, -1, -1))
			{
				this->lora_on = true;

				ESP_LOGI("LoRa", "LoRa started succesfully");

				this->lora.setFrequency(LORA_FREQ);

				this->lora.setRxGain(LORA_RX_GAIN_BOOSTED, SX127X_RX_GAIN_AUTO);

				this->lora.setSpreadingFactor(LORA_SPREAD_FACTOR);
				this->lora.setBandwidth(LORA_BW);
				this->lora.setCodeRate(LORA_CODE_RATE);

				this->lora.setHeaderType(LORA_HDR_TYPE);
				this->lora.setPreambleLength(LORA_PREAMBLE_LEN);
				this->lora.setPayloadLength(PKT_SIZE);
				this->lora.setCrcEnable(LORA_CRC_ENABLE);

				this->lora.setSyncWord(LORA_SYNC_WORD);

				this->lora.request(SX127X_RX_CONTINUOUS);
			}
			else
			{
				ESP_LOGE("LoRa", "LoRa start error!");
			}
		}

		void LoraPulseCounter::lora_rx(uint8_t *data, uint8_t len)
		{
			int off = 0;

			ESP_LOGD("LoRa", "LoRa: RX len=%d bytes=[%02X %02X %02X %02X %02X %02X %02X %02X %02X]",
				len, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8]);

			if (data[off++] == 'L' && data[off++] == 'A')
			{
				/* Pulse counter */
				uint32_t pulse_cnt = (data[off++] << 24);
				pulse_cnt |= (data[off++] << 16);
				pulse_cnt |= (data[off++] << 8);
				pulse_cnt |= (data[off++] << 0);

				/* Misc */
				uint32_t misc = (data[off++] << 16);
				misc |= (data[off++] << 8);
				misc |= (data[off++] << 0);

				float cell_volts = (((misc >> VCC_SHIFT) & ADC_MASK) * ADC_REF) / ADC_RES;
				bool cell_volts_low = (cell_volts <= VOLTAGE_THRESHOLD);
				int temp = (misc >> TEMP_SHIFT) & ADC_MASK;

				/* LoRa signal */
				int16_t rssi = this->lora.packetRssi();
				float snr = this->lora.snr();

				/* CRC16 */
				#if defined(CRC16_ENABLE)
					uint16_t crc16_calc = esphome::crc16(data, DATA_SIZE);
					uint16_t crc16_lora = (data[DATA_SIZE] << 8) | (data[DATA_SIZE + 1] << 0);
					if (crc16_lora != crc16_calc)
					{
						ESP_LOGE("LoRa", "LoRa: CRC16 error RX=%04X vs Calc=%04X", crc16_lora, crc16_calc);
						return;
					}
				#endif /* CRC16_ENABLE */

				if (this->sensor_pulses != nullptr)
					this->sensor_pulses->publish_state(pulse_cnt);
				if (this->sensor_rssi != nullptr)
					this->sensor_rssi->publish_state(rssi);
				if (this->sensor_snr != nullptr)
					this->sensor_snr->publish_state(snr);
				if (this->sensor_temperature != nullptr)
					this->sensor_temperature->publish_state(temp);
				if (this->sensor_voltage != nullptr)
					this->sensor_voltage->publish_state(cell_volts);
				if (this->sensor_voltage_low != nullptr)
					this->sensor_voltage_low->publish_state(cell_volts_low);
			}
		}

		void LoraPulseCounter::set_lora_rst(int lora_rst)
		{
			this->lora_rst = lora_rst;
		}

		void LoraPulseCounter::set_lora_ss(int lora_ss)
		{
			this->lora_ss = lora_ss;
		}

		void LoraPulseCounter::set_sensor_pulses(sensor::Sensor *sensor_pulses)
		{
			this->sensor_pulses = sensor_pulses;
		}

		void LoraPulseCounter::set_sensor_rssi(sensor::Sensor *sensor_rssi)
		{
			this->sensor_rssi = sensor_rssi;
		}

		void LoraPulseCounter::set_sensor_snr(sensor::Sensor *sensor_snr)
		{
			this->sensor_snr = sensor_snr;
		}

		void LoraPulseCounter::set_sensor_temperature(sensor::Sensor *sensor_temperature)
		{
			this->sensor_temperature = sensor_temperature;
		}

		void LoraPulseCounter::set_sensor_voltage(sensor::Sensor *sensor_voltage)
		{
			this->sensor_voltage = sensor_voltage;
		}

		void LoraPulseCounter::set_sensor_voltage_low(binary_sensor::BinarySensor *sensor_voltage_low)
		{
			this->sensor_voltage_low = sensor_voltage_low;
		}

		void LoraPulseCounter::dump_config(void)
		{
			if (this->sensor_pulses != nullptr)
				LOG_SENSOR("  ", "Sensor", this->sensor_pulses);
			if (this->sensor_rssi != nullptr)
				LOG_SENSOR("  ", "Sensor", this->sensor_rssi);
			if (this->sensor_snr != nullptr)
				LOG_SENSOR("  ", "Sensor", this->sensor_snr);
			if (this->sensor_temperature != nullptr)
				LOG_SENSOR("  ", "Sensor", this->sensor_temperature);
			if (this->sensor_voltage != nullptr)
				LOG_SENSOR("  ", "Sensor", this->sensor_voltage);
			if (this->sensor_voltage_low != nullptr)
				LOG_BINARY_SENSOR("  ", "Binary Sensor", this->sensor_voltage_low);
		}

		float LoraPulseCounter::get_setup_priority(void) const
		{
			return setup_priority::LATE;
		}

		void LoraPulseCounter::setup(void)
		{
			this->lora_start();
		}

		void LoraPulseCounter::update(void)
		{
			if (!this->lora_on)
			{
				if (millis() > this->lora_millis + LORA_START_INTERVAL)
				{
					this->lora_start();
				}
			}
			else
			{
				this->lora.wait(LORA_WAIT);

				uint8_t status = this->lora.status();
				if (status == SX127X_STATUS_RX_DONE)
				{
					int len = this->lora.available();

					if (len > 0)
					{
						uint8_t data[PKT_SIZE];

						ESP_LOGD("LoRa", "LoRa: RX len=%d", len);

						if (len > PKT_SIZE)
						{
							len = PKT_SIZE;
						}

						this->lora.read(data, len);
						this->lora_rx(data, len);
					}
				}

				this->lora.request(SX127X_RX_CONTINUOUS);
			}
		}
	}
}
