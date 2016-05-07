#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wcp-wrapper.h>
#include "M0518.h"
#include "define.h"
#include "i2c_rw.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Macro, type and constant definitions                                                                    */
/*---------------------------------------------------------------------------------------------------------*/

// #define DEBUG                       1

#define PLLCON_SETTING              CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK                   50000000

#define PWM_PERIOD_US               11000
#define PWM_LOW_BOUND_US            1200
#define PWM_HI_BLUND_US             1800
#define PWM_PERIOD_LIMIT_US         11000
#define PWM_LOW_BOUND_LIMIT_US      900
#define PWM_HI_BOUND_LIMIT_US       2200
#define PWM_GLOBAL_PRESCALE         5

#define Enable_PWM0_INT()       (PWM0->CAPIEN |= (0x15 << 8))
#define Disable_PWM0_INT()      (PWM0->CAPIEN &= (~(0x15 << 8)))
#define Enable_PWM1_INT()       (PWM1->CAPIEN |= (0x15 << 8))
#define Disable_PWM1_INT()      (PWM1->CAPIEN &= (~(0x15 << 8)))
#define Enable_I2C0_INT()       (I2C_EnableInt(I2C0))
#define Disable_I2C0_INT()      (I2C_DisableInt(I2C0))

#define PWM_GET_CMR(pwm, u32ChannelNum) ((pwm)->CMPBUF[(u32ChannelNum)])
#define PWM_GET_CNR(pwm, u32ChannelNum)  ((pwm)->PBUF[(((u32ChannelNum) >> 1) << 1)])
#define BPWM_GET_CMR(bpwm, u32ChannelNum) ((bpwm)->CMPBUF[(u32ChannelNum)])
#define BPWM_GET_CNR(bpwm) ((bpwm)->PBUF)

double PWM_Cycle_Per_US;
int PWM_NS_Per_Cycle;

int PWM_Cycle_Per_Period;
int PWM_Cycle_Low_Bound;
int PWM_Cycle_Hi_Bound;
int PWM_Cycle_Per_Period_Limit;
int PWM_Cycle_Low_Bound_Limit;
int PWM_Cycle_Hi_Bound_Limit;

I2C_SEND_T I2C_Send_Control;
I2C_RECV_T I2C_Recv_Control;

size_t buf_cap = 200;
size_t buf_len = 200;
uint8_t buf[200];

void PWM0_IRQHandler(void);
void PWM1_IRQHandler(void);
void I2C0_IRQHandler(void);
void SYS_Init(void);
void PWM_INIT( void );
void I2C_RESET(void);
void I2C_INIT(void);
void I2C_Slave_Go(I2C_T* port, uint32_t status);
void fc_log(char* msg);

#if defined(DEBUG)
unsigned PWM0_COUNTER = 0;
unsigned PWM0_COUNTER_LIMIT;
void draw_graph(int* arr, int n) {
    int max = 0;
    int temp;
    static char buf[100] = "";

    // printf("\tarray dump: \n\tn=%d\n", n);
    for (temp = 0; temp < n; ++temp) {
    //   printf("arr[%d] = \t%d\n", temp, arr[temp]);
        if (arr[temp] > max) {
            max = arr[temp];
        }
        buf[2 * temp] = '\t';
    }
    // putchar('\n');
    buf[2 * temp] = '\0';
    buf[2 * temp + 1] = '\0';

    while (max > 0) {
        for (temp = 0; temp < n; ++temp) {
            if (arr[temp] >= max) {
                buf[2 * temp + 1] = '*';
            } else {
                buf[2 * temp + 1] = ' ';
            }
        }
        printf("%s\t| max= %d\n", buf, max);
        max -= 1;
    }
    for (temp = 0; temp < n; ++temp) {
        printf("\t%d", arr[temp]);
    }
    putchar('\n');
    for (temp = 0; temp < n * 9; ++temp) {
        putchar('-');
    }
    putchar('\n');
    putchar('\n');
}

#endif

void PWM0_IRQHandler(void) {
    Disable_PWM0_INT();
    PWM0->CAPIF = 0x3f3f;

    unsigned int index;
    unsigned int falling_data, rising_data;
    unsigned int high_period, low_period, total_period;

    for (   index = 0;
            index < 6;
            index += 2   ) {

        falling_data = PWM_GET_CAPTURE_FALLING_DATA(PWM0, index);
        rising_data = PWM_GET_CAPTURE_RISING_DATA(PWM0, index);
        high_period = rising_data - falling_data;
        low_period = 0x10000 - rising_data;
        total_period = high_period + low_period;
        if (high_period < PWM_Cycle_Low_Bound && high_period >= PWM_Cycle_Low_Bound_Limit) {
            PWM_Cycle_Low_Bound = high_period;
        }
        if (high_period > PWM_Cycle_Hi_Bound && high_period <= PWM_Cycle_Hi_Bound_Limit) {
            PWM_Cycle_Hi_Bound = high_period;
        }
        if (total_period > PWM_Cycle_Per_Period && total_period <= PWM_Cycle_Per_Period_Limit) {
            PWM_Cycle_Per_Period = total_period;
        }

        if(high_period >= PWM_Cycle_Low_Bound && high_period <= PWM_Cycle_Hi_Bound) {

            BPWM_SET_CNR(BPWM0, index, PWM_Cycle_Per_Period);
            BPWM_SET_CMR(BPWM0, index, high_period);
        }
    }
    Enable_PWM0_INT();
}

/*
// void PWM0_IRQHandler(void) {
//     Disable_PWM0_INT();
//     //int capture_int_flag = PWM0->CAPIF & 0x3f3f;
//     #if defined(DEBUG)
//     if (PWM0_COUNTER > PWM0_COUNTER_LIMIT) {
//         printf("PWM0 IRQ on %x\n", capture_int_flag);
//     }
//     #endif
//     PWM0->CAPIF = 0x3f3f;
//     //capture_int_flag >>= 8;

//     unsigned int index, bitflag;
//     unsigned int falling_data, rising_data;
//     unsigned int high_period, low_period, total_period;

//     #if defined(DEBUG)
//     static int level[6] = {0, 0, 0, 0, 0, 0};
//         PWM0_COUNTER_LIMIT = 4 * (1000000 / PWM_PERIOD_US);
//         //printf("INT!\n");

//         // if (PWM0_COUNTER > PWM0_COUNTER_LIMIT) {
//         //     printf("BPWM STATUS:\n");
//         //     for (int index = 0; index < 6; ++index) {
//         //         printf(
//         //             "\tCH %d: \t%dus\tof\t%dus\n",
//         //             index,
//         //             BPWM_GET_CMR(BPWM0, index) / PWM_Cycle_Per_US,
//         //             BPWM_GET_CNR(BPWM0) / PWM_Cycle_Per_US
//         //         );
//         //     }
//         //     printf("\n");
//         // }
//     #endif

//     for (   index = 0, bitflag = 1;
//             index < 6; // && capture_int_flag & bitflag;
//             index += 2, bitflag <<= 2   ) {
//     // index = 0;
//     // bitflag = 1;
//     // while(index < 6 && capture_int_flag & bitflag) {

//         falling_data = PWM_GET_CAPTURE_FALLING_DATA(PWM0, index);
//         rising_data = PWM_GET_CAPTURE_RISING_DATA(PWM0, index);
//         high_period = rising_data - falling_data;
//         low_period = 0x10000 - rising_data;
//         total_period = high_period + low_period;
//         if (high_period < PWM_Cycle_Low_Bound && high_period >= PWM_Cycle_Low_Bound_Limit) {
//             PWM_Cycle_Low_Bound = high_period;
//         }
//         if (high_period > PWM_Cycle_Hi_Bound && high_period <= PWM_Cycle_Hi_Bound_Limit) {
//             PWM_Cycle_Hi_Bound = high_period;
//         }
//         if (total_period > PWM_Cycle_Per_Period && total_period <= PWM_Cycle_Per_Period_Limit) {
//             PWM_Cycle_Per_Period = total_period;
//         }

//         #if defined(DEBUG)
//         if (PWM0_COUNTER > PWM0_COUNTER_LIMIT) {
//             printf(
//                 "PWM0 CH %d \tRD: %d/%dus, \t\tFD: %d/%dus\n\t\t\tHP: %d/%dus, \t\tLP: %d/%dus\n\n",
//                 index,
//                 rising_data,
//                 rising_data / PWM_Cycle_Per_US,
//                 falling_data,
//                 falling_data / PWM_Cycle_Per_US,
//                 high_period,
//                 high_period / PWM_Cycle_Per_US,
//                 low_period,
//                 low_period / PWM_Cycle_Per_US
//             );
//         }
//         #endif

//         if(high_period >= PWM_Cycle_Low_Bound && high_period <= PWM_Cycle_Hi_Bound) {

//             #if defined(DEBUG)
//             if (PWM0_COUNTER > PWM0_COUNTER_LIMIT) {
//                 printf(
//                     "BPWM CH %d output changed from %dus of %dus to %dus of %dus\n\n",
//                     index,
//                     BPWM_GET_CMR(BPWM0, index) / PWM_Cycle_Per_US,
//                     BPWM_GET_CNR(BPWM0) / PWM_Cycle_Per_US,
//                     high_period / PWM_Cycle_Per_US,
//                     PWM_Cycle_Per_Period / PWM_Cycle_Per_US
//                 );
//             }
//             #endif

//             BPWM_SET_CNR(BPWM0, index, PWM_Cycle_Per_Period);
//             BPWM_SET_CMR(BPWM0, index, high_period);
//         }

//         // index += 2;
//         // bitflag <<= 2;
//     }

//     #if defined(DEBUG)
//     if (PWM0_COUNTER > PWM0_COUNTER_LIMIT) {
//         for(temp = 0; temp < 3; ++temp) {
//             level[temp] = (BPWM_GET_CMR(BPWM0, temp * 2) / PWM_Cycle_Per_US) / 100 - 10;
//             level[temp] <<= 2;
//         }
//         for(temp = 0; temp < 3; ++temp) {
//             level[temp + 3] = (BPWM_GET_CMR(BPWM1, temp * 2) / PWM_Cycle_Per_US) / 100 - 10;
//             level[temp + 3] <<= 2;
//         }
//         draw_graph(level, 6);
//     }
//     #endif
//     #if defined(DEBUG)
//     if (PWM0_COUNTER > PWM0_COUNTER_LIMIT) {
//         PWM0_COUNTER = 0;
//     }
//     ++PWM0_COUNTER;
//     #endif
//     Enable_PWM0_INT();
// }
*/
void PWM1_IRQHandler(void) {
    Disable_PWM1_INT();
    PWM1->CAPIF = 0x3f3f;

    int index;
    int falling_data, rising_data;
    int high_period, low_period, total_period;

    for (   index = 0;
            index < 6;
            index += 2 ) {

        falling_data = PWM_GET_CAPTURE_FALLING_DATA(PWM1, index);
        rising_data = PWM_GET_CAPTURE_RISING_DATA(PWM1, index);
        high_period = rising_data - falling_data;
        low_period = 0x10000 - rising_data;
        total_period = high_period + low_period;
        if (high_period < PWM_Cycle_Low_Bound && high_period >= PWM_Cycle_Low_Bound_Limit) {
            PWM_Cycle_Low_Bound = high_period;
        }
        if (high_period > PWM_Cycle_Hi_Bound && high_period <= PWM_Cycle_Hi_Bound_Limit) {
            PWM_Cycle_Hi_Bound = high_period;
        }
        if (total_period > PWM_Cycle_Per_Period && total_period <= PWM_Cycle_Per_Period_Limit) {
            PWM_Cycle_Per_Period = total_period;
        }

        if(high_period >= PWM_Cycle_Low_Bound && high_period <= PWM_Cycle_Hi_Bound) {
            BPWM_SET_CNR(BPWM1, index, PWM_Cycle_Per_Period);
            BPWM_SET_CMR(BPWM1, index, high_period);
        }
    }
    Enable_PWM1_INT();
}

void I2C0_IRQHandler(void)
{
    uint32_t u32Status;

    u32Status = I2C_GET_STATUS(I2C0);

    if(I2C_GET_TIMEOUT_FLAG(I2C0))
    {
        printf("timeout: %d\n", u32Status);
        /* Clear I2C0 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C0);
        return;
    }
    I2C_Slave_Go(I2C0, u32Status);
    return;
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for IRC22M clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external 12MHz XTAL, internal 22.1184MHz */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk | CLK_PWRCON_OSC22M_EN_Msk);

    /* Enable PLL and Set PLL frequency */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_PLL_STB_Msk | CLK_CLKSTATUS_XTL12M_STB_Msk | CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to PLL, STCLK to HCLK/2 */
    //CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_PLL, CLK_CLKDIV_HCLK(2));
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HXT, CLK_CLKDIV_HCLK(1));

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable PWM module clock */
    CLK_EnableModuleClock(PWM0_MODULE);
    CLK_EnableModuleClock(PWM1_MODULE);
    CLK_EnableModuleClock(BPWM0_MODULE);
    CLK_EnableModuleClock(BPWM1_MODULE);

    /* Enable I2C0 module clock */
    CLK_EnableModuleClock(I2C0_MODULE);

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));

    /* Select PWM module clock source */
    CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL3_PWM0_S_PCLK, 0);
    CLK_SetModuleClock(PWM1_MODULE, CLK_CLKSEL3_PWM1_S_PCLK, 0);
    CLK_SetModuleClock(BPWM0_MODULE, CLK_CLKSEL3_BPWM0_S_PCLK, 0);
    CLK_SetModuleClock(BPWM1_MODULE, CLK_CLKSEL3_BPWM1_S_PCLK, 0);

    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);
    SYS_ResetModule(PWM1_RST);
    SYS_ResetModule(BPWM0_RST);
    SYS_ResetModule(BPWM1_RST);

    // Reset I2C
    SYS_ResetModule(I2C0_RST);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();
    //PllClock        = PLL_CLOCK;            // PLL
    //SystemCoreClock = PLL_CLOCK / 1;        // HCLK
    //CyclesPerUs     = PLL_CLOCK / 1000000;  // For SYS_SysTickDelay()

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    //SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    //SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);

    /* Set GPA multi-function pins for PWM0 Channel 0 and PWM1 channel 0 */
    //SYS->GPA_MFP = (SYS->GPA_MFP & (~SYS_GPA_MFP_PA12_Msk));
    //SYS->GPA_MFP |= SYS_GPA_MFP_PA12_PWM0_CH0;

    //SYS->GPA_MFP = (SYS->GPA_MFP & (~SYS_GPA_MFP_PA2_Msk));
    //SYS->GPA_MFP |= SYS_GPA_MFP_PA2_PWM1_CH0;
    //SYS->ALT_MFP3 &= ~(SYS_ALT_MFP3_PA2_Msk);
    //SYS->ALT_MFP3 |= SYS_ALT_MFP3_PA2_PWM1_CH0;

    // SYS->ALT_MFP = 0x00000000;
    // SYS->ALT_MFP2 = 0x00000000;
    // SYS->ALT_MFP3 = SYS_ALT_MFP3_PF4_PWM1_CH4 | SYS_ALT_MFP3_PD15_BPWM0_CH4 | SYS_ALT_MFP3_PD14_BPWM0_CH5 | SYS_ALT_MFP3_PC3_BPWM0_CH3 | SYS_ALT_MFP3_PC2_BPWM0_CH2 | SYS_ALT_MFP3_PC1_BPWM0_CH1 | SYS_ALT_MFP3_PC0_BPWM0_CH0 | SYS_ALT_MFP3_PB11_PWM0_CH4 | SYS_ALT_MFP3_PA10_PWM1_CH2 | SYS_ALT_MFP3_PA2_PWM1_CH0;
    // SYS->ALT_MFP4 = SYS_ALT_MFP4_PC7_I2C0_SCL | SYS_ALT_MFP4_PC6_I2C0_SDA;
    // SYS->GPA_MFP = SYS_GPA_MFP_PA14_PWM0_CH2 | SYS_GPA_MFP_PA12_PWM0_CH0 | SYS_GPA_MFP_PA10_PWM1_CH2 | SYS_GPA_MFP_PA2_PWM1_CH0;
    // SYS->GPB_MFP = SYS_GPB_MFP_PB11_PWM0_CH4;
    // SYS->GPC_MFP = SYS_GPC_MFP_PC7_I2C0_SCL | SYS_GPC_MFP_PC6_I2C0_SDA | SYS_GPC_MFP_PC3_BPWM0_CH3 | SYS_GPC_MFP_PC2_BPWM0_CH2 | SYS_GPC_MFP_PC1_BPWM0_CH1 | SYS_GPC_MFP_PC0_BPWM0_CH0;
    // SYS->GPD_MFP = SYS_GPD_MFP_PD15_BPWM0_CH4 | SYS_GPD_MFP_PD14_BPWM0_CH5;
    // SYS->GPE_MFP = 0x00000000;
    // SYS->GPF_MFP = SYS_GPF_MFP_PF7_ICE_DAT | SYS_GPF_MFP_PF6_ICE_CLK | SYS_GPF_MFP_PF4_PWM1_CH4;

    SYS->ALT_MFP = 0x00000000;
    SYS->ALT_MFP2 = 0x00000000;
    SYS->ALT_MFP3 = SYS_ALT_MFP3_PF8_BPWM1_CH4 | SYS_ALT_MFP3_PF4_PWM1_CH4 | SYS_ALT_MFP3_PD15_BPWM0_CH4 | SYS_ALT_MFP3_PD7_BPWM1_CH0 | SYS_ALT_MFP3_PC2_BPWM0_CH2 | SYS_ALT_MFP3_PC0_BPWM0_CH0 | SYS_ALT_MFP3_PB11_PWM0_CH4 | SYS_ALT_MFP3_PB8_BPWM1_CH2 | SYS_ALT_MFP3_PA10_PWM1_CH2 | SYS_ALT_MFP3_PA2_PWM1_CH0;
    SYS->ALT_MFP4 = SYS_ALT_MFP4_PC7_I2C0_SCL | SYS_ALT_MFP4_PC6_I2C0_SDA;
    SYS->GPA_MFP = SYS_GPA_MFP_PA14_PWM0_CH2 | SYS_GPA_MFP_PA12_PWM0_CH0 | SYS_GPA_MFP_PA10_PWM1_CH2 | SYS_GPA_MFP_PA2_PWM1_CH0;
    SYS->GPB_MFP = SYS_GPB_MFP_PB11_PWM0_CH4 | SYS_GPB_MFP_PB8_BPWM1_CH2;
    SYS->GPC_MFP = SYS_GPC_MFP_PC7_I2C0_SCL | SYS_GPC_MFP_PC6_I2C0_SDA | SYS_GPC_MFP_PC2_BPWM0_CH2 | SYS_GPC_MFP_PC0_BPWM0_CH0;
    SYS->GPD_MFP = SYS_GPD_MFP_PD15_BPWM0_CH4 | SYS_GPD_MFP_PD7_BPWM1_CH0;
    SYS->GPE_MFP = 0x00000000;
    SYS->GPF_MFP = SYS_GPF_MFP_PF7_ICE_DAT | SYS_GPF_MFP_PF6_ICE_CLK | SYS_GPF_MFP_PF4_PWM1_CH4;

    #if defined(DEBUG)
    printf("Clock Dump:\n");
    printf("\tCPU Freq:\t%d\n", CLK_GetCPUFreq());
    printf("\tHCLK Freq:\t%d\n", CLK_GetHCLKFreq());
    printf("\tHXT Freq:\t%d\n", CLK_GetHXTFreq());
    printf("\tPCLK Freq:\t%d\n", CLK_GetPCLKFreq());
    printf("\tPLL Freq:\t%d\n", CLK_GetPLLClockFreq());
    #endif
}

void PWM_INIT( void ) {
    int temp;
    int clock_src;
    int clock_freq;

    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);
    SYS_ResetModule(PWM1_RST);
    SYS_ResetModule(BPWM0_RST);
    SYS_ResetModule(BPWM1_RST);

    //CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HXT, CLK_CLKDIV_HCLK(1));
    clock_src = CLK->CLKSEL3 & CLK_CLKSEL3_PWM0_S_Msk;

    if(clock_src == 0)
    {
        //clock source is from PLL clock
        clock_freq = CLK_GetPLLClockFreq();
        #if defined(DEBUG)
        printf("PWM Clock is set to PLL clock\n");
        #endif
    }
    else
    {
        //clock source is from PCLK
        //SystemCoreClockUpdate();
        //clock_freq = SystemCoreClock;
        clock_freq = CLK_GetPCLKFreq();
        #if defined(DEBUG)
        printf("PWM Clock is set to PCLK\n");
        #endif
    }

    #if defined(DEBUG)
    printf("PWM Clock Freq: %d\n", clock_freq);
    #endif

    // init clock
    PWM_Cycle_Per_US = (double)clock_freq / (double)(PWM_GLOBAL_PRESCALE + 1) / 1000.00 / 1000.00;
    PWM_NS_Per_Cycle = (int) (1000.00 / PWM_Cycle_Per_US);
    PWM_Cycle_Per_Period = (int) (PWM_Cycle_Per_US * PWM_PERIOD_LIMIT_US);
    PWM_Cycle_Low_Bound = (int) (PWM_Cycle_Per_US * PWM_LOW_BOUND_US);
    PWM_Cycle_Hi_Bound = (int) (PWM_Cycle_Per_US * PWM_HI_BLUND_US);
    PWM_Cycle_Per_Period_Limit = (int) (PWM_Cycle_Per_US * PWM_PERIOD_US);
    PWM_Cycle_Low_Bound_Limit = (int) (PWM_Cycle_Per_US * PWM_LOW_BOUND_LIMIT_US);
    PWM_Cycle_Hi_Bound_Limit = (int) (PWM_Cycle_Per_US * PWM_HI_BOUND_LIMIT_US);

    // for (temp = 0; temp < 6; ++temp) {
    //     BPWM_ConfigOutputChannel(BPWM0, temp, 250, 30);

    //     BPWM_SET_PRESCALER(BPWM0, temp, PWM_GLOBAL_PRESCALE);
    //     BPWM_SET_CNR(BPWM0, temp, PWM_Cycle_Per_Period);
    //     BPWM_SET_CMR(BPWM0, temp, PWM_Cycle_Low_Bound);
    //     #if defined(DEBUG)
    //     printf("BPWM0 CH %d CMR: %d, CNR: %d\n", temp, PWM_Cycle_Hi_Bound, PWM_Cycle_Per_Period);
    //     #endif
    // }
    // BPWM_EnableOutput(BPWM0, BPWM_CH_0_MASK | BPWM_CH_1_MASK | BPWM_CH_2_MASK | BPWM_CH_3_MASK | BPWM_CH_4_MASK | BPWM_CH_5_MASK);
    // BPWM_Start(BPWM0, BPWM_CH_0_MASK | BPWM_CH_1_MASK | BPWM_CH_2_MASK | BPWM_CH_3_MASK | BPWM_CH_4_MASK | BPWM_CH_5_MASK);

    // init pin
    for (temp = 0; temp < 6; temp+=2) {
        PWM_ConfigCaptureChannel(PWM0, temp, PWM_NS_Per_Cycle, 0);
        PWM_ConfigCaptureChannel(PWM1, temp, PWM_NS_Per_Cycle, 0);
        BPWM_ConfigOutputChannel(BPWM0, temp, 250, 30);
        BPWM_ConfigOutputChannel(BPWM1, temp, 250, 30);

        PWM_SET_PRESCALER(PWM0, temp, PWM_GLOBAL_PRESCALE);
        PWM_SET_PRESCALER(PWM1, temp, PWM_GLOBAL_PRESCALE);
        BPWM_SET_PRESCALER(BPWM0, temp, PWM_GLOBAL_PRESCALE);
        BPWM_SET_PRESCALER(BPWM1, temp, PWM_GLOBAL_PRESCALE);

        BPWM_SET_CNR(BPWM0, temp, PWM_Cycle_Per_Period);
        BPWM_SET_CMR(BPWM0, temp, PWM_Cycle_Low_Bound);
        BPWM_SET_CNR(BPWM1, temp, PWM_Cycle_Per_Period);
        BPWM_SET_CMR(BPWM1, temp, PWM_Cycle_Low_Bound);
    }

    // init capture
    PWM_Start(PWM0, PWM_CH_0_MASK | PWM_CH_2_MASK | PWM_CH_4_MASK);
    PWM_Start(PWM1, PWM_CH_0_MASK | PWM_CH_2_MASK | PWM_CH_4_MASK);

    PWM_EnableCapture(PWM0, PWM_CH_0_MASK | PWM_CH_2_MASK | PWM_CH_4_MASK);
    PWM_EnableCapture(PWM1, PWM_CH_0_MASK | PWM_CH_2_MASK | PWM_CH_4_MASK);

    PWM0->CAPCTL |= PWM_CAPCTL_FCRLDEN0_Msk | PWM_CAPCTL_FCRLDEN2_Msk | PWM_CAPCTL_FCRLDEN4_Msk;
    PWM1->CAPCTL |= PWM_CAPCTL_FCRLDEN0_Msk | PWM_CAPCTL_FCRLDEN2_Msk | PWM_CAPCTL_FCRLDEN4_Msk;

    while((PWM0->CNT[0]) == 0 || (PWM0->CNT[2]) == 0 || (PWM0->CNT[4]) == 0);
    while((PWM1->CNT[0]) == 0 || (PWM1->CNT[2]) == 0 || (PWM1->CNT[4]) == 0);

    // init output
    BPWM_EnableOutput(BPWM0, BPWM_CH_0_MASK | BPWM_CH_2_MASK | BPWM_CH_4_MASK);
    BPWM_EnableOutput(BPWM1, BPWM_CH_0_MASK | BPWM_CH_2_MASK | BPWM_CH_4_MASK);

    BPWM_Start(BPWM0, BPWM_CH_0_MASK | BPWM_CH_2_MASK | BPWM_CH_4_MASK);
    BPWM_Start(BPWM1, BPWM_CH_0_MASK | BPWM_CH_2_MASK | BPWM_CH_4_MASK);

    // init IRQ
    NVIC_EnableIRQ(PWM0_IRQn);
    NVIC_EnableIRQ(PWM1_IRQn);

    Enable_PWM0_INT();
    Enable_PWM1_INT();


    // (PWM0->CAPIEN |= 0x3f << 6);
    // (PWM1->CAPIEN |= 0x3f << 6);
    //(PWM0->CAPIEN &= ~(0x3f << 6))
    //(PWM1->CAPIEN &= ~(0x3f << 6))


}

void I2C_RESET(void) {
    // Reset I2C
    SYS_ResetModule(I2C0_RST);

    /* Open I2C module and set bus clock */
    I2C_Open(I2C0, 100000);

    /* Set I2C 4 Slave Addresses */
    I2C_SetSlaveAddr(I2C0, 0, 0x15, 0);   /* Slave Address : 0x15 */
    I2C_SetSlaveAddr(I2C0, 1, 0x35, 0);   /* Slave Address : 0x35 */
    I2C_SetSlaveAddr(I2C0, 2, 0x55, 0);   /* Slave Address : 0x55 */
    I2C_SetSlaveAddr(I2C0, 3, 0x75, 0);   /* Slave Address : 0x75 */

    /* Set I2C 4 Slave Addresses Mask */
    /*
    I2C_SetSlaveAddrMask(I2C0, 0, 0x01);
    I2C_SetSlaveAddrMask(I2C0, 1, 0x04);
    I2C_SetSlaveAddrMask(I2C0, 2, 0x01);
    I2C_SetSlaveAddrMask(I2C0, 3, 0x04);
    */


    //I2C_SEND_RESET(&I2C_Send_Control);
    //I2C_RECV_RESET(&I2C_Recv_Control);

    I2C_SET_FLAG(&I2C_Send_Control, I2C_FLAG_SEND_REPEAT);
    I2C_SET_FLAG(&I2C_Recv_Control, I2C_FLAG_FORCE_FLUSH);

    /* Enable I2C interrupt */
    I2C_EnableInt(I2C0);
    NVIC_EnableIRQ(I2C0_IRQn);

    I2C_SET_CONTROL_REG(I2C0, I2C_I2CON_SI_AA);

}

void I2C_INIT(void)
{
    I2C_SEND_INIT(&I2C_Send_Control, 200);
    I2C_RECV_INIT(&I2C_Recv_Control, 200);

    fc_wcp_status_transfer = FREECOPTER_WCP_STATUS_TRANSFER_IDLE;
    memset(&fc_wcp_status_transfer_header, 0, sizeof(fc_wcp_status_transfer_header));
    fc_wcp_status_transfer_progress = 0;

    fc_wcp_status_timeout_count = 10000;
    fc_wcp_status_timeout_reload = 10000;

    fc_wcp_status_channels_control_expires = 1000000;
    fc_wcp_status_channels_control_expires = 1000000;
    memset(&fc_wcp_status_channels, 0, sizeof(fc_wcp_status_channels));

    I2C_RESET();
}

void I2C_Slave_Go(I2C_T* port, uint32_t status) {
    uint8_t temp_data;
    int res;

    #if defined(DEBUG)
    //printf("I2C_Slave_Go!\n");
    #endif

    if(status != 0x60 && status != 0x80 && status != 0xa0) {
        printf("I2C status 0x%X\n", status);
    }
    switch (status) {
        case I2C_STATUS_SLAVE_RESTART_STOP:       // slave receive finished
        //I2C_Flush_Recv_Data(&I2C_Recv_Control);
        I2C_RESET();
        break;

        case I2C_STATUS_SLAVE_RECV_DATA_NACK:
        fc_log("I2C_STATUS_SLAVE_RECV_DATA_NACK");
        // I2C_Flush_Recv_Data(&I2C_Recv_Control);
        I2C_SET_CONTROL_REG(port, I2C_I2CON_SI_AA);
        break;

        // case I2C_STATUS_SLAVE_SEND_DATA_NACK:
        //      // master indecate to interrupt send
        // I2C_SET_CONTROL_REG(port, I2C_I2CON_SI_AA);
        // break;

        // case I2C_STATUS_SLAVE_SEND_LAST_DATA_ACK: // seems never occurs
        // printf("I2C_STATUS_SLAVE_SEND_LAST_DATA_ACK\n");
        // I2C_SET_CONTROL_REG(port, I2C_I2CON_SI_AA);
        // I2C_Flush_Send_Data(&I2C_Send_Control);
        // break;

        case I2C_STATUS_SLAVE_SEND_ADDR_ACK:
        case I2C_STATUS_SLAVE_SEND_DATA_ACK:
        case I2C_STATUS_SLAVE_SEND_DATA_NACK:
        case I2C_STATUS_SLAVE_SEND_LAST_DATA_ACK:
        res = I2C_Send_Byte(&I2C_Send_Control, port, &temp_data);
        ++fc_wcp_status_transfer_progress;
        fc_wcp_status_timeout_count = fc_wcp_status_timeout_reload;

        switch (fc_wcp_status_transfer) {

            case FREECOPTER_WCP_STATUS_TRANSFER_SEND_HEADER:
            if(fc_wcp_status_transfer_progress == 4) {
                fc_wcp_status_transfer_progress = 0;
                fc_wcp_status_transfer = FREECOPTER_WCP_STATUS_TRANSFER_SEND;

                I2C_Set_Send_Data(&I2C_Send_Control, &fc_wcp_status_channels, sizeof(fc_wcp_status_channels));
                I2C_Flush_Send_Data(&I2C_Send_Control);
            }
            break;
            case FREECOPTER_WCP_STATUS_TRANSFER_SEND:
            if(fc_wcp_status_transfer_progress == fc_wcp_status_transfer_header.data_length) {
                fc_wcp_status_transfer = FREECOPTER_WCP_STATUS_TRANSFER_IDLE;
            }
            break;
            case FREECOPTER_WCP_STATUS_TRANSFER_IDLE:
            case FREECOPTER_WCP_STATUS_TRANSFER_RECV:
            fc_log("sending in recv/idle status, unknow behavior!");
            break;
            default:
            fc_log("fc_wcp_status_transfer not set");
            break;
        }
        if ( res == I2C_NOTICE_LAST_DATA_SENT) {
            I2C_SET_CONTROL_REG(port, I2C_I2CON_SI);
        } else {
            I2C_SET_CONTROL_REG(port, I2C_I2CON_SI_AA);
        }
        break;

        case I2C_STATUS_SLAVE_RECV_ADDR_ACK:
        I2C_SET_CONTROL_REG(port, I2C_I2CON_SI_AA);
        break;

        case I2C_STATUS_SLAVE_RECV_DATA_ACK:
        I2C_Recv_Byte(&I2C_Recv_Control, port, &temp_data);
        ++fc_wcp_status_transfer_progress;
        fc_wcp_status_timeout_count = fc_wcp_status_timeout_reload;
        switch (fc_wcp_status_transfer) {
            case FREECOPTER_WCP_STATUS_TRANSFER_IDLE:
            case FREECOPTER_WCP_STATUS_TRANSFER_SEND_HEADER:
            case FREECOPTER_WCP_STATUS_TRANSFER_SEND:

            fc_wcp_status_transfer_progress = 0;
            fc_wcp_status_transfer_header.command = temp_data;
            if (fc_wcp_status_transfer_header.command == FREECOPTER_WCP_COMMAND_SET_STATUS) {
                fc_wcp_status_transfer = FREECOPTER_WCP_STATUS_TRANSFER_RECV_HEADER;
            } else {
                fc_wcp_status_transfer = FREECOPTER_WCP_STATUS_TRANSFER_SEND_HEADER;
                fc_wcp_status_transfer_header.data_length = sizeof(FREECOPTER_WCP_STATUS_T);
                I2C_Set_Send_Data(&I2C_Send_Control, &fc_wcp_status_transfer_header.data_length, 4);
                I2C_Flush_Send_Data(&I2C_Send_Control);
            }
            I2C_RECV_RESET(&I2C_Recv_Control);
            I2C_SEND_RESET(&I2C_Send_Control);
            break;

            case FREECOPTER_WCP_STATUS_TRANSFER_RECV_HEADER:
            if (fc_wcp_status_transfer_progress == 4) {
                fc_wcp_status_transfer_progress = 0;
                fc_wcp_status_transfer_header.data_length = *(uint32_t*)I2C_Recv_Control->pending_data_buf;

                fc_wcp_status_transfer = FREECOPTER_WCP_STATUS_TRANSFER_RECV;
                I2C_RESET(&I2C_Recv_Control);
            }
            break;

            case FREECOPTER_WCP_STATUS_TRANSFER_RECV:
            if (fc_wcp_status_transfer_progress >= fc_wcp_status_transfer_header.data_length) {
                I2C_Flush_Recv_Data(&I2C_Recv_Control);
                fc_wcp_status_transfer_progress = 0;
                fc_wcp_status_transfer = FREECOPTER_WCP_STATUS_TRANSFER_IDLE;
                fc_wcp_update_channel_status(&I2C_Recv_Control);
            }
            break;
            default:
            fc_log("fc_wcp_status_transfer not set");
            break;
        }
        I2C_SET_CONTROL_REG(port, I2C_I2CON_SI_AA);
        break;

        case I2C_STATUS_SLAVE_RECV_ARBI_LOST:
        case I2C_STATUS_SLAVE_SEND_ARBI_LOST:

        case I2C_STATUS_GC_ADDR_ACK:
        case I2C_STATUS_GC_ARBI_LOST:
        case I2C_STATUS_GC_DATA_ACK:
        case I2C_STATUS_GC_DATA_NACK:

        case I2C_STATUS_BUS_ERROR:
        case I2C_STATUS_BUS_FREE:

        case I2C_STATUS_MASTER_START:
        case I2C_STATUS_MASTER_RESTART:
        case I2C_STATUS_MASTER_SEND_ADDR_ACK:
        case I2C_STATUS_MASTER_SEND_ADDR_NACK:
        case I2C_STATUS_MASTER_SEND_DATA_ACK:
        case I2C_STATUS_MASTER_SEND_DATA_NACK:
        case I2C_STATUS_MASTER_ARBI_LOST:
        case I2C_STATUS_MASTER_RECV_ADDR_ACK:
        case I2C_STATUS_MASTER_RECV_ADDR_NACK:
        case I2C_STATUS_MASTER_RECV_DATA_ACK:
        case I2C_STATUS_MASTER_RECV_DATA_NACK:

        default:
        printf("I2C status 0x%X\n", status);
        I2C_RESET();
        break;
    }
}

void fc_log(char* msg) {
    #if defined(DEBUG)
    printf("fc-log: %s\n", msg);
    #else
    #endif
}

int32_t main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();
    I2C_INIT();
    // iniialize pwm after i2c
    PWM_INIT();
    #if defined(DEBUG)
    printf("main: 0x%X started.\n", &main);

    //Disable_PWM0_INT();
    //Disable_PWM1_INT();

    //BPWM_SET_CNR(BPWM0, 0, PWM_Cycle_Per_Period);
    //BPWM_SET_CMR(BPWM0, 0, 2000 * PWM_Cycle_Per_US);
    //BPWM_SET_CMR(BPWM0, 1, 1500 * PWM_Cycle_Per_US);
    //printf("press any key to get status message\n");
    #endif

    while (1) {
        fc_wcp_loop();
    //     if (I2C_TEST_FLAG(&I2C_Recv_Control, I2C_FLAG_FLUSH)) {
    //
    //         I2C_Get_Recv_Data(&I2C_Recv_Control, buf + 7, &buf_len);
    //         --buf_len;
    //         memcpy(buf, "echo: [", 7);
    //         buf_len += 7;
    //         buf[buf_len] = ']';
    //         buf[buf_len + 1] = '\0';
    //         buf_len += 2;
    //
    //         if (buf_len > 12) {
    //             //printf("%s\n", buf);
    //             I2C_Set_Send_Data(&I2C_Send_Control, buf, &buf_len);
    //             I2C_Flush_Send_Data(&I2C_Send_Control);
    //         }
    //         buf_len = buf_cap;
    //     }
    // }

    /*
    Don't care about close pwm and i2c
    */
}
