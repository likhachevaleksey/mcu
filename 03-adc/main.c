#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdio-task/stdio-task.h"
#include "protocol-task/protocol-task.h"
#include "led-task/led-task.h"
#include "adc-task/adc-task.h"
#include "hardware/gpio.h"

#define DEVICE_NAME "my-pico-device"
#define DEVICE_VRSN "v0.0.1"

uint32_t global_variable = 0;

const uint32_t constant_variable = 42;

void version_callback(const char* args)
{
	printf("device name: '%s', firmware version: %s\n", DEVICE_NAME, DEVICE_VRSN);
}

void led_on_callback(){
    led_task_state_set(LED_STATE_ON);
    led_task_handle();
}
void led_off_callback(){
    led_task_state_set(LED_STATE_OFF);
    led_task_handle();
}
void led_blink_callback(){
    led_task_state_set(LED_STATE_BLINK);
    led_task_handle();
}
void led_blink_set_period_ms_callback(const char* args){
    uint period_ms = 0;
    sscanf(args, "%u", &period_ms);
    if (period_ms ==0){
        printf("Error: period_ms cannot be zero");
    }
    else{
        led_task_set_blink_period_ms(period_ms);
    }

}
void help_callback();

void mem_callback(const char* args)
{
    uint32_t address = 0;
    
    // Парсим аргумент как hex число
    if (sscanf(args, "%x", &address) == 1)
    {
        // Читаем значение по адресу
        uint32_t value = *(volatile uint32_t*)address;
        printf("Memory at address 0x%08X: 0x%08X (%u)\n", address, value, value);
    }
    else
    {
        printf("Error: Invalid address. Usage: mem <hex_address>\n");
        printf("Example: mem 0x20000000\n");
    }
}
void wmem_callback(const char* args)
{
    uint32_t address = 0;
    uint32_t value = 0;
    
    // Парсим два аргумента как hex числа
    if (sscanf(args, "%x %u", &address, &value) == 2)
    {
        // Записываем значение по адресу
        *(volatile uint32_t*)address = value;
        printf("Written %u (0x%08X) to address 0x%08X\n", value, value, address);
        
        // Проверяем, что записалось корректно
        uint32_t verify = *(volatile uint32_t*)address;
        if (verify == value) {
            printf("Verification successful: 0x%08X\n", verify);
        } else {
            printf("Verification failed! Read: 0x%08X\n", verify);
        }
    }
    else
    {
        printf("Error: Invalid arguments. Usage: wmem <hex_address> <hex_value>\n");
        printf("Example: wmem 0xd0000000 0x00000001\n");
    }
}
void get_adc_callback(){
    float voltage_V = adc_voltage();
    printf("%f\n", voltage_V);
}
void get_temp_callback(){
    float temp_C = adc_get_temp();
    printf("%f\n", temp_C);
}
void tm_start_callback(){
    adc_task_set_state(ADC_TASK_STATE_RUN);
}
void tm_stop_callback(){
    adc_task_set_state(ADC_TASK_STATE_IDLE);
}

api_t device_api[] =
{
	{"version", version_callback, "get device name and firmware version"},
    {"on", led_on_callback, "switch on led"},
    {"off", led_off_callback, "switch off led"},
    {"blink", led_blink_callback, "start blinking with default period"},
    {"set_period", led_blink_set_period_ms_callback, "set blinking period in milliseconds"},
    {"help", help_callback, "print all available commands with description"},
    {"mem", mem_callback, "read memory at address (hex). Usage: mem <address>"},
    {"wmem", wmem_callback, "write memory at address (hex). Usage: wmem <address> <value>"},
    {"get_adc", get_adc_callback, "getting voltage from adc 26"},
    {"get_temp", get_temp_callback, "getting temperture from adc 4"},
    {"tm_start", tm_start_callback, "start voltage and temperature measurement continuously"},
    {"tm_stop", tm_stop_callback, "stop voltage and temperature measurement continuously"},
	{NULL, NULL, NULL},
};

void help_callback(){
    printf("\n=== Available Commands ===\n");
    
    // Проходим по всем командам в массиве device_api
    for (int i = 0; device_api[i].command_name != NULL; i++)
    {
        // Выводим имя команды и её описание
        printf("Command  %-20s - %s \n", 
               device_api[i].command_name, 
               device_api[i].command_help);
    }
    
    printf("==========================\n");
}


int main(){
    stdio_init_all();
    stdio_task_init();
    led_task_init();
    protocol_task_init(device_api);
    adc_task_init();
    while(1)
    {
        // Всегда обрабатываем состояние LED (для мигания)
        led_task_handle();
        adc_task_handle();
        
        // Обрабатываем команды только когда они приходят
        char* command = stdio_task_handle();
        if (command != NULL) {
            protocol_task_handle(command);
        }
        // protocol_task_handle(stdio_task_handle());
    }
}