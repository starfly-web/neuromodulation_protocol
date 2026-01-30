// tens_driver.c
#include "tens_driver.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

static const char *TAG = "TENS_DRV";

// TENS hardware configuration
static ledc_channel_t tens_channel = LEDC_CHANNEL_0;
static ledc_timer_t tens_timer = LEDC_TIMER_0;
static gpio_num_t tens_enable_pin = 19;
static uint32_t current_level = 512; // 50% duty for medium current

bool tens_hw_init(void) {
    ESP_LOGI(TAG, "TENS hardware initialization");
    
    // Initialize LEDC (PWM) for TENS output
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .timer_num = tens_timer,
        .freq_hz = 1000,  // 1kHz carrier frequency
        .clk_cfg = LEDC_AUTO_CLK,
    };
    
    esp_err_t err = ledc_timer_config(&ledc_timer);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC timer: %s", esp_err_to_name(err));
        return false;
    }
    
    ledc_channel_config_t ledc_channel = {
        .gpio_num = 18,    // TENS output pin
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = tens_channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = tens_timer,
        .duty = 0,         // Start with 0% duty
        .hpoint = 0,
    };
    
    err = ledc_channel_config(&ledc_channel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure LEDC channel: %s", esp_err_to_name(err));
        return false;
    }
    
    // Configure TENS enable pin
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << tens_enable_pin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en = 0,
        .intr_type = GPIO_INTR_DISABLE,
    };
    
    err = gpio_config(&io_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure TENS enable pin: %s", esp_err_to_name(err));
        return false;
    }
    
    // Initialize with TENS disabled
    gpio_set_level(tens_enable_pin, 0);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, tens_channel, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, tens_channel);
    
    ESP_LOGI(TAG, "TENS hardware initialized successfully");
    return true;
}

bool tens_hw_start_burst(void) {
    ESP_LOGI(TAG, "Starting TENS burst");
    
    // Enable TENS output
    gpio_set_level(tens_enable_pin, 1);
    
    // Set PWM duty for current control (0-1023 range)
    // This controls the current level for the TENS stimulation
    ledc_set_duty(LEDC_LOW_SPEED_MODE, tens_channel, current_level);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, tens_channel);
    
    ESP_LOGD(TAG, "TENS burst started with current level %u", current_level);
    return true;
}

bool tens_hw_stop_burst(void) {
    ESP_LOGI(TAG, "Stopping TENS burst");
    
    // Disable TENS output and ensure charge balance
    ledc_set_duty(LEDC_LOW_SPEED_MODE, tens_channel, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, tens_channel);
    gpio_set_level(tens_enable_pin, 0);
    
    ESP_LOGD(TAG, "TENS burst stopped, outputs disabled");
    return true;
}

bool tens_hw_set_current(uint32_t level) {
    ESP_LOGI(TAG, "Setting TENS current level to %u", level);
    
    if (level > 1023) {
        ESP_LOGW(TAG, "Current level %u exceeds maximum (1023), clamping", level);
        level = 1023;
    }
    
    current_level = level;
    
    // Update current level if TENS is active
    if (gpio_get_level(tens_enable_pin) == 1) {
        ledc_set_duty(LEDC_LOW_SPEED_MODE, tens_channel, current_level);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, tens_channel);
    }
    
    return true;
}

bool tens_hw_get_status(void) {
    return (gpio_get_level(tens_enable_pin) == 1);
}
