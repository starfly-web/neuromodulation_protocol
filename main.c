// main.c
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "neuromod.h"

void app_main(void) {
    ESP_LOGI("MAIN", "Neuromod v0.1 starting");
    
    // Example usage of the public API
    set_audio_frequency(7000);      // Set frequency to 7 kHz
    set_session_duration(30);       // Set session to 30 minutes
    set_tens_delay(10);             // Set TENS delay to 10 ms
    
    ESP_LOGI("MAIN", "Configuration complete, ready to start session");
    
    // In a real application, you would wait for a trigger to start the session
    // For demonstration, we'll start immediately after a short delay
    vTaskDelay(pdMS_TO_TICKS(2000));
    
    start_session();
    
    // Main loop - monitor system status
    while (1) {
        // TODO: Add system monitoring, fault detection, or UI updates here
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}