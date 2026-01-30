// audio_driver.c
#include "audio_driver.h"
#include "esp_log.h"
#include "driver/i2s.h"
#include <math.h>

static const char *TAG = "AUDIO_DRV";

// Audio hardware configuration
static i2s_port_t audio_i2s_port = I2S_NUM_0;
static uint32_t current_frequency = 7000;
static uint16_t *audio_buffer = NULL;
static float phase_accumulator = 0.0f;

bool audio_hw_init(void) {
    ESP_LOGI(TAG, "Audio hardware initialization");
    
    // Initialize I2S for audio output
    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = 44100,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count = 8,
        .dma_buf_len = 1024,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };
    
    i2s_pin_config_t pin_config = {
        .bck_io_num = 26,
        .ws_io_num = 25,
        .data_out_num = 22,
        .data_in_num = I2S_PIN_NO_CHANGE
    };
    
    esp_err_t err = i2s_driver_install(audio_i2s_port, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to install I2S driver: %s", esp_err_to_name(err));
        return false;
    }
    
    err = i2s_set_pin(audio_i2s_port, &pin_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set I2S pins: %s", esp_err_to_name(err));
        return false;
    }
    
    // Allocate audio buffer
    audio_buffer = malloc(1024 * sizeof(uint16_t));
    if (!audio_buffer) {
        ESP_LOGE(TAG, "Failed to allocate audio buffer");
        return false;
    }
    
    // Reset phase accumulator
    phase_accumulator = 0.0f;
    
    ESP_LOGI(TAG, "Audio hardware initialized successfully");
    return true;
}

bool audio_hw_set_frequency(uint32_t f_hz) {
    ESP_LOGI(TAG, "Setting audio frequency to %u Hz", f_hz);
    
    if (f_hz < 1000 || f_hz > 10000) {
        ESP_LOGW(TAG, "Frequency %u Hz outside recommended range (1000-10000 Hz)", f_hz);
    }
    
    current_frequency = f_hz;
    
    // Update I2S sample rate if needed (optional optimization)
    esp_err_t err = i2s_set_sample_rates(audio_i2s_port, 44100);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set sample rate: %s", esp_err_to_name(err));
        return false;
    }
    
    return true;
}

bool audio_hw_start(void) {
    ESP_LOGI(TAG, "Starting audio output");
    
    if (!audio_buffer) {
        ESP_LOGE(TAG, "Audio buffer not allocated");
        return false;
    }
    
    // Generate and play sine wave
    float sample_rate = 44100.0f;
    float phase_increment = 2.0f * M_PI * current_frequency / sample_rate;
    
    // Generate sine wave samples
    for (int i = 0; i < 1024; i++) {
        float sample = sinf(phase_accumulator) * 16384.0f; // Scale to 16-bit range
        audio_buffer[i] = (uint16_t)(sample + 32768.0f); // Add DC offset
        phase_accumulator += phase_increment;
        if (phase_accumulator >= 2.0f * M_PI) {
            phase_accumulator -= 2.0f * M_PI;
        }
    }
    
    // Write audio data to I2S
    size_t bytes_written;
    esp_err_t err = i2s_write(audio_i2s_port, audio_buffer, 1024 * sizeof(uint16_t), &bytes_written, portMAX_DELAY);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write audio data: %s", esp_err_to_name(err));
        return false;
    }
    
    return true;
}

bool audio_hw_stop(void) {
    ESP_LOGI(TAG, "Stopping audio output");
    
    // Stop I2S audio output
    esp_err_t err = i2s_stop(audio_i2s_port);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop I2S: %s", esp_err_to_name(err));
        return false;
    }
    
    return true;
}
