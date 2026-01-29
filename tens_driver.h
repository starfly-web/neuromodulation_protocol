// tens_driver.h
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool tens_hw_init(void);
bool tens_hw_start_burst(void);
bool tens_hw_stop_burst(void);

#ifdef __cplusplus
}
#endif