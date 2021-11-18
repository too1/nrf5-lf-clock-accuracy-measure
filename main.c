/**
 * Copyright (c) 2014 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf.h"
#include "nrf_delay.h"
#include "app_error.h"
#include "bsp.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

static void my_timer_init(void)
{
    NRF_TIMER0->PRESCALER = 0;
    NRF_TIMER0->BITMODE = TIMER_BITMODE_BITMODE_32Bit <<TIMER_BITMODE_BITMODE_Pos;
    
    NRF_PPI->CH[0].EEP = (uint32_t)&NRF_RTC0->EVENTS_TICK;
    NRF_PPI->CH[0].TEP = (uint32_t)&NRF_TIMER0->TASKS_CAPTURE[0];
    NRF_PPI->CHENSET = (1 << 0);
    
    NRF_RTC0->EVTENSET = RTC_EVTENSET_TICK_Msk;
    NRF_RTC0->INTENSET = RTC_INTENSET_TICK_Msk;
    NRF_RTC0->PRESCALER = 0;
    NVIC_SetPriority(RTC0_IRQn, 7);
    NVIC_EnableIRQ(RTC0_IRQn);
    
    NRF_TIMER0->TASKS_START = 1;
    NRF_RTC0->TASKS_CLEAR = 1;
    NRF_RTC0->TASKS_START = 1;   
}

#define TEST_ITER_NUM       5
#define TEST_RUN_LENGTH     1000
#define TEST_INTERVAL_MS    800

volatile uint32_t test_buf[TEST_RUN_LENGTH+1];
volatile uint32_t test_buf_pos;
static volatile bool test_finished = true;
volatile int last_cc = 0;

static void run_test(void)
{
    test_buf_pos = 0;
    test_finished = false;
}

static void analyze_test_results()
{
    uint32_t min = 0xFFFFFFFF;
    uint32_t max = 0;
    uint32_t sum = 0;
    for(int i = 1; i < (TEST_RUN_LENGTH+1); i++)
    {
        if(test_buf[i] < min) min = test_buf[i];
        if(test_buf[i] > max) max = test_buf[i];
        sum += test_buf[i];
    }
    float avg = (float)sum / (float)TEST_RUN_LENGTH;
    float freq = 1000.0f / ((float)sum / (float)TEST_RUN_LENGTH / 16000.0f);
    float deviate = ((freq / 32768.0f) - 1.0f) * 1000000.0f;
    uint8_t log_buf[180];
    sprintf(log_buf, "Results: Min/Max: %i/%i, Avg: %.2f, Freq: %.1f, Dev_ppm: %.1f", min, max, avg, freq, deviate);
    NRF_LOG_INFO("%s", log_buf);
}

static void run_test_loop(void)
{
    for(int i = 0; i < TEST_ITER_NUM; i++)
    {
        run_test();
        while(!test_finished);
        analyze_test_results();
        nrf_delay_ms(TEST_INTERVAL_MS);
        NRF_LOG_FLUSH();
    }
}

int main(void)
{
    APP_ERROR_CHECK(NRF_LOG_INIT(NULL));
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    NRF_LOG_INFO("\n");
    NRF_LOG_INFO("RC accuracy test example started\n");

    NRF_CLOCK->TASKS_LFCLKSTOP = 1;
    NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_RC << CLOCK_LFCLKSRC_SRC_Pos;
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    while(NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
    NRF_LOG_INFO("LF clock started");
    NRF_RTC0->TASKS_STOP = 0;

    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while(NRF_CLOCK->EVENTS_HFCLKSTARTED == 0);
    NRF_LOG_INFO("LF clock started (RC osc)");
    
    my_timer_init();
    
    while (true)
    {
        // Run a number of tests with RC oscillator, without calibration
        run_test_loop();
        
#if 1
        // Run a number of tests with RC oscillator calibrated
        NRF_CLOCK->EVENTS_DONE = 0;
        NRF_CLOCK->TASKS_CAL = 1;
        while(NRF_CLOCK->EVENTS_DONE == 0);
        NRF_LOG_INFO("RC osc calibrated");
        
        run_test_loop();
#endif
        // Run a number of tests with external LF clock source
        NRF_LOG_INFO("Changing to external RC clock source...");
        NRF_CLOCK->TASKS_LFCLKSTOP = 1;
        NRF_CLOCK->LFCLKSRC = CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos;
        NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
        NRF_CLOCK->TASKS_LFCLKSTART = 1;
        while(NRF_CLOCK->EVENTS_LFCLKSTARTED == 0);
        NRF_LOG_INFO("Done");
        
        run_test_loop();        

        
        NRF_LOG_INFO("Tests done");
        NRF_LOG_FLUSH();
        while(1);
    }
}

void RTC0_IRQHandler(void)
{
    static int counter = 0;
    int cc;
    if(NRF_RTC0->EVENTS_TICK)
    {
        NRF_RTC0->EVENTS_TICK = 0;
        if(!test_finished)
        {
            cc = NRF_TIMER0->CC[0];
            test_buf[test_buf_pos++] = cc - last_cc;
            last_cc = cc;
            if(test_buf_pos >= (TEST_RUN_LENGTH + 1)) test_finished = true;
        }
    }
}


/** @} */
