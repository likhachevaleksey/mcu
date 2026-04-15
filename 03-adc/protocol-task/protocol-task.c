#include "protocol-task.h"
#include "pico/stdlib.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

static api_t* api = {0};
static int commands_count = 0;

void protocol_task_init(api_t* device_api){
    // Сохраняем указатель на массив команд
    api = device_api;
    
    // Подсчитываем количество команд
    commands_count = 0;
    
    if (api != NULL) {
        // Проходим по массиву, пока не встретим пустую структуру (command_name == NULL)
        while (api[commands_count].command_name != NULL) {
            commands_count++;
        }
    }
}

void protocol_task_handle(char* command_string)
{
    // Добавляем в обработчик проверку на то, что `command_string` не равно `NULL`.
    // Если `command_string` равно `NULL`, то выйти из обработчика: строка команды
    // еще не получена;
    if (!command_string)
    {
        return;  // строка команды еще не получена, выходим
    }

    // логика обработки полученной строки. Делим ее на команду и аргументы:
    const char* command_name = command_string;
    const char* command_args = NULL;

    char* space_symbol = strchr(command_string, ' ');

    if (space_symbol)
    {
        *space_symbol = '\0';
        command_args = space_symbol + 1;
    }
    else
    {
        command_args = "";
    }

    // Добавляем вывод найденных имени команды и ее аргументов;
    printf("Command: '%s', Args: '%s'\r\n", command_name, command_args);

    // в цикле проходим по массиву команд `api` и ищем совпадение имени команды;
    for (int i = 0; i < commands_count; i++)
    {
        // определяем совпадает ли команда с именем команды в массиве `api`
        // если не совпадает, переходим к следующей итерации
        if (strcmp(api[i].command_name, command_name) != 0)
        {
            continue;  // не совпадает, проверяем следующую команду
        }
        
        // мы нашли команду, вызываем callback найденной команды
        api[i].command_callback(command_args);
        return;
    }
    
    // выводим ошибку, если команда не была найдена в списке команд
    printf("Error: Unknown command '%s'\r\n", command_name);
    return;
}