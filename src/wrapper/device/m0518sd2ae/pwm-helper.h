#ifndef FREECOPTER_PWM_HELPER_H
#define FREECOPTER_PWM_HELPER_H

#include <stdint.h>

#define PWM_CHANNEL_N                       6
#define PWM_CHANNEL_HISTORY_STEP            8

// used by the update function to indecate that weathe the new value should be passed.
// the new value will be converted to an value between range 1-1000, this setting is the
// largest difference allowed between the new value and the average of the old values.
#define PWM_CHANNEL_VALUE_ACCEPT_VALUE      50

typedef uint32_t PWM_HISTORY_T[PWM_CHANNEL_N][PWM_CHANNEL_HISTORY_STEP];

extern PWM_HISTORY_T pwm_history;

// update channel value,
// returns 0 to pass
// returns 1 means the new value is too large, while returns 0 means the new value is too small.
int pwm_channel_value_check_dither(uint32_t channel_n, int value);
#endif
