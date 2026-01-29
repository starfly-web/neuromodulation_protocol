// neuromod.c
#include "neuromod.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_system.h"

static const char *TAG = "NEUROMOD";

typedef enum {
    SESSION_STATE_IDLE = 0,
    SESSION_STATE_RUNNING,
    SESSION_STATE_FAULT
} session_state_t;

static session_state_t g_state = SESSION_STATE_IDLE;

// Configurable parameters
static uint32_t g_audio_freq_hz = 7000; // example default
static uint32_t g_session_duration_s = 30 * 60;
static uint32_t g_tens_delay_ms = 10; // STDP-critical delay

// Timing
static int64_t g_session_start_us = 0;

// Timers
static esp_timer_handle_t audio_event_timer = NULL;
static esp_timer_handle_t tens_event_timer = NULL;
static esp_timer_handle_t session_end_timer = NULL;

// Hardware abstraction stubs
// You'll later wire these to DAC/I2S, TENS driver, etc.

// Called when an "audio event" should occur
static void audio_event_start(void) {
    // TODO: start or mark audio stimulus (tone, burst, etc.)
    // For narrowband audio, you might configure I2S / DAC here.
    ESP_LOGD(TAG, "Audio event START (f = %u Hz)", g_audio_freq_hz);
}

// Called when the audio event ends (if pulsed)
static void audio_event_stop(void) {
    // TODO: stop audio stimulus if using pulsed mode
    ESP_LOGD(TAG, "Audio event STOP");
}

// Called when a TENS burst should start
static void tens_burst_start(void) {
    // TODO: enable TENS output, start burst timing
    ESP_LOGD(TAG, "TENS burst START (+%u ms)", g_tens_delay_ms);
}

// Called when a TENS burst ends (burst duration is firmware-controlled)
static void tens_burst_stop(void) {
    // TODO: disable TENS output
    ESP_LOGD(TAG, "TENS burst STOP");
}

// Timer callbacks (sound first → TENS second)

// Audio event timer callback
static void audio_timer_cb(void *arg) {
    if (g_state != SESSION_STATE_RUNNING) return;
    
    audio_event_start();
    
    // Schedule TENS burst after the fixed delay
    uint64_t delay_us = (uint64_t)g_tens_delay_ms * 1000ULL;
    esp_timer_start_once(tens_event_timer, delay_us);
    
    // Schedule next audio event
    schedule_next_pair();
}

// TENS event timer callback
static void tens_timer_cb(void *arg) {
    if (g_state != SESSION_STATE_RUNNING) return;
    
    tens_burst_start();
    tens_burst_stop();
}

// Session end timer callback
static void session_end_cb(void *arg) {
    ESP_LOGI(TAG, "Session duration reached, stopping session");
    stop_session();
}

// Scheduling logic (pre-computed cadence placeholder)
// Right now this uses a simple fixed repetition period; you can later replace it with a precomputed schedule.

// Example: repetition period between audio events (in ms)
#define PAIR_PERIOD_MS 500 // placeholder; tune per protocol

static void schedule_next_pair(void) {
    if (g_state != SESSION_STATE_RUNNING) return;
    
    uint64_t period_us = (uint64_t)PAIR_PERIOD_MS * 1000ULL;
    esp_timer_start_once(audio_event_timer, period_us);
}

// Public API implementation

static void create_timers_if_needed(void) {
    if (!audio_event_timer) {
        const esp_timer_create_args_t audio_args = {
            .callback = &audio_timer_cb,
            .arg = NULL,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "audio_evt"
        };
        esp_timer_create(&audio_args, &audio_event_timer);
    }
    
    if (!tens_event_timer) {
        const esp_timer_create_args_t tens_args = {
            .callback = &tens_timer_cb,
            .arg = NULL,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "tens_evt"
        };
        esp_timer_create(&tens_args, &tens_event_timer);
    }
    
    if (!session_end_timer) {
        const esp_timer_create_args_t end_args = {
            .callback = &session_end_cb,
            .arg = NULL,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "session_end"
        };
        esp_timer_create(&end_args, &session_end_timer);
    }
}

void set_audio_frequency(uint32_t f_hz) {
    // Could add range checks here
    g_audio_freq_hz = f_hz;
    ESP_LOGI(TAG, "Audio frequency set to %u Hz", g_audio_freq_hz);
}

void set_session_duration(uint32_t minutes) {
    if (minutes == 0) minutes = 1;
    if (minutes > 60) minutes = 60; // v0.1 max
    g_session_duration_s = minutes * 60;
    ESP_LOGI(TAG, "Session duration set to %u min", minutes);
}

void set_tens_delay(uint32_t delay_ms) {
    // v0.1: enforce around 10 ms with small tolerance if you want
    g_tens_delay_ms = delay_ms;
    ESP_LOGI(TAG, "TENS delay set to %u ms", g_tens_delay_ms);
}

void start_session(void) {
    if (g_state == SESSION_STATE_RUNNING) {
        ESP_LOGW(TAG, "Session already running");
        return;
    }
    
    create_timers_if_needed();
    
    g_state = SESSION_STATE_RUNNING;
    g_session_start_us = esp_timer_get_time();
    
    ESP_LOGI(TAG, "Starting session (duration %u s, delay %u ms)", 
             g_session_duration_s, g_tens_delay_ms);
    
    // Schedule first audio event
    schedule_next_pair();
    
    // Schedule session end
    uint64_t end_us = (uint64_t)g_session_duration_s * 1000000ULL;
    esp_timer_start_once(session_end_timer, end_us);
}

void stop_session(void) {
    if (g_state == SESSION_STATE_IDLE) {
        ESP_LOGW(TAG, "Session not running");
        return;
    }
    
    g_state = SESSION_STATE_IDLE;
    
    if (audio_event_timer) esp_timer_stop(audio_event_timer);
    if (tens_event_timer) esp_timer_stop(tens_event_timer);
    if (session_end_timer) esp_timer_stop(session_end_timer);
    
    ESP_LOGI(TAG, "Session stopped");
}

// Fault handling (engineering-level guardrail hook)
void fault_handler(void) {
    ESP_LOGE(TAG, "FAULT detected, entering safe state");
    
    g_state = SESSION_STATE_FAULT;
    
    // Stop all timers
    if (audio_event_timer) esp_timer_stop(audio_event_timer);
    if (tens_event_timer) esp_timer_stop(tens_event_timer);
    if (session_end_timer) esp_timer_stop(session_end_timer);
    
    // TODO: trigger hardware safety shutdown (disable outputs)
    // TODO: log fault details
    
    ESP_LOGE(TAG, "System in safe state. Manual reset required.");
}