#include "pwm-helper.h"

PWM_HISTORY_T pwm_history;

int pwm_channel_value_check_dither(uint32_t channel_n, int value) {
    return 0;
    int ave = 0;
    int i;
    for (i = 0; i < PWM_CHANNEL_HISTORY_STEP - 1; ++i) {
        ave += pwm_history[channel_n][i];
        pwm_history[channel_n][i] = pwm_history[channel_n][i + 1];
    }
    ave += pwm_history[channel_n][PWM_CHANNEL_HISTORY_STEP - 1];
    pwm_history[channel_n][PWM_CHANNEL_HISTORY_STEP - 1] = value;

    ave /= PWM_CHANNEL_HISTORY_STEP;
    if (value > ave && value - ave > PWM_CHANNEL_VALUE_ACCEPT_VALUE) {
        return 1;
    }
    if (ave > value && ave - value > PWM_CHANNEL_VALUE_ACCEPT_VALUE) {
        return -1;
    }
    return 0;
}
