#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "driver/i2c_master.h"

#include "debug.h"
#include "vl53l0x.h"

uint32_t timerCount = 0;
uint32_t tick = 0;

static bool IRAM_ATTR timer_alarm_ms_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    tick++;

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = edata->alarm_value + 1000, //Alarm every 1ms
    };

    gptimer_set_alarm_action(timer, &alarm_config);

    return false;
}


void Timer_Init()
{
    gptimer_handle_t gptimer = NULL;

    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, //1MHz, 1us
    };

    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_event_callbacks_t callbacks = {
        .on_alarm = timer_alarm_ms_cb,
    };

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &callbacks, NULL));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    gptimer_alarm_config_t alarm_config1 = {
        .alarm_count = 1000, //Alarm every 1ms
    };

    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config1));
    ESP_ERROR_CHECK(gptimer_start(gptimer));

    debug("TIMER Initialized\r\n");
}

void app_main(void)
{
    Timer_Init();

    I2C_Init();

    vl53l0x_t* vl = vl53l0x_config(I2C_NUM_0, GPIO_NUM_5, GPIO_NUM_4, 0, VL53_SENSOR_ADDR, 0);

    /*if(I2C_Probe())
    {*/
        vl53l0x_init(vl);

        debug("VL53 Initialized\r\n");

        while(1)
        {
            if(tick - timerCount > 500)
            {
                timerCount = tick;

                uint16_t measurement = vl53l0x_readRangeSingleMillimeters(vl);

                printf("Measurement: %umm\r\n", measurement);
            }

            vTaskDelay(1); //Watchdog reset issue
        }  
    //}
}