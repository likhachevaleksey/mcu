#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include "adc-task.h"

const uint ADC_GPIO = 26;
const uint ADC_CHANNEL = 0;
const uint TEMP_SENS_GPIO = 4;

const float ADC_REF_VOLTAGE = 3.3f;  // Опорное напряжение 3.3 В
const uint16_t ADC_MAX_VALUE = 4095; // 2^12 - 1 = 4095

adc_task_state_t adc_state = ADC_TASK_STATE_IDLE;
uint64_t adc_ts;
uint ADC_TASK_MEAS_PERIOD_US = 100000;

void adc_task_init(){
    adc_init();
    adc_gpio_init( ADC_GPIO );
    adc_set_temp_sensor_enabled(true);
}

float adc_voltage(){

    adc_select_input(ADC_GPIO);
    uint16_t voltage_counts = adc_read();

    float voltage_V = (voltage_counts * ADC_REF_VOLTAGE) / ADC_MAX_VALUE;

    return voltage_V;
}

float adc_get_temp(){
    adc_select_input(TEMP_SENS_GPIO);
    uint16_t temp_counts = adc_read();
    float temp_V = (temp_counts * ADC_REF_VOLTAGE) / ADC_MAX_VALUE;


    float temp_C = 27.0f - (temp_V - 0.706f) / 0.001721f;;

    return temp_C;
}

void adc_task_set_state(adc_task_state_t state){
    if (adc_state != state) {
        adc_state = state;
    }
}

void adc_task_handle(){
    switch (adc_state)
    {
    case ADC_TASK_STATE_RUN:
        if (time_us_64() > adc_ts)
        {
            adc_ts = time_us_64() + ADC_TASK_MEAS_PERIOD_US;
            float voltage_V = adc_voltage();
            float temp_C = adc_get_temp();
            printf("%f %f\n", voltage_V, temp_C);
        }
        break;
    default:
        break;
    }
}
