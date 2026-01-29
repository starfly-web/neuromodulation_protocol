// neuromod.h
#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void set_audio_frequency(uint32_t f_hz);
void set_session_duration(uint32_t minutes);
void set_tens_delay(uint32_t delay_ms); // default 10 ms
void start_session(void);
void stop_session(void);
void fault_handler(void);

#ifdef __cplusplus
}
#endif