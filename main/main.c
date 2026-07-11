#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "wifi_prov.h"
#include "mqtt_app.h"

void app_main(void)
{
    led_init();
    wifi_prov_connect();
    mqtt_app_start();

    printf("Hello world hnh===!\n");
    while(1) {
        LED_TOGGLE();
        vTaskDelay(500);
    }

}
