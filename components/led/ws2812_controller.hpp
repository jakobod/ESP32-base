#ifndef WS2812_CONTROLLER_HPP
#define WS2812_CONTROLLER_HPP

extern "C" {
#include "driver/rmt.h"
}

enum class color {
  green = 2*8,
  red   = 1*8,
  blue  = 0*8,
};

template <uint16_t num_leds = 1, gpio_num_t gpio_num = GPIO_NUM_18>
class ws2812_controller {
  constexpr static uint8_t bits_per_led_command = 24;
  constexpr static uint16_t buffer_items = num_leds*bits_per_led_command;

  // These values are determined by measuring pulse timing with logic analyzer and adjusting to match datasheet. 
  constexpr static uint16_t T0H = 14;  // 0 bit high time
  constexpr static uint16_t T1H = 52;  // 1 bit high time
  constexpr static uint16_t TL = 52;  // low time for either bit

  uint32_t leds[num_leds];
  rmt_item32_t led_data_buffer[buffer_items];

public:
  ws2812_controller() {
    rmt_config_t config;
    config.rmt_mode = RMT_MODE_TX;
    config.channel = RMT_CHANNEL_0;
    config.gpio_num = gpio_num;
    config.mem_block_num = 3;
    config.tx_config.loop_en = false;
    config.tx_config.carrier_en = false;
    config.tx_config.idle_output_en = true;
    config.tx_config.idle_level = static_cast<rmt_idle_level_t>(0);
    config.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));
  }
  ~ws2812_controller() = default;

  void write(int idx, uint8_t red, uint8_t green, uint8_t blue) {
    if (idx >= num_leds) return; // guard

    leds[idx] = (red << 16 | green << 8 | blue);
    update();
  }

  void update() {
    for (uint32_t led = 0; led < num_leds; led++) {
      uint32_t bits_to_send = leds[led];
      uint32_t mask = 1 << (bits_per_led_command - 1);
      for (uint32_t bit = 0; bit < bits_per_led_command; bit++) {
        uint32_t bit_is_set = bits_to_send & mask;
        led_data_buffer[led * bits_per_led_command + bit] = bit_is_set ?
                                                        (rmt_item32_t){{{T1H, 1, TL, 0}}} : 
                                                        (rmt_item32_t){{{T0H, 1, TL, 0}}};
        mask >>= 1;
      }
    }
    
    ESP_ERROR_CHECK(rmt_write_items(RMT_CHANNEL_0, led_data_buffer, buffer_items, false));
    ESP_ERROR_CHECK(rmt_wait_tx_done(RMT_CHANNEL_0, portMAX_DELAY));
  }
};

#endif // WS2812_CONTROLLER_HPP