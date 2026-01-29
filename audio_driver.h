// audio_driver.h
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool audio_hw_init(void);
bool audio_hw_set_frequency(uint32_t f_hz);
bool audio_hw_start(void);
bool audio_hw_stop(void);

#ifdef __cplusplus
}
#endif