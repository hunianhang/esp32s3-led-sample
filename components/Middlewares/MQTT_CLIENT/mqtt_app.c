#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_random.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "mqtt_client.h"

#include "mqtt_app.h"
#include "mqtt_secrets.h"

static const char *TAG = "mqtt_app";

/* OneNET Thing Model (物模型) topics: $sys/{product-id}/{device-name}/thing/...
 * "temperature" below must match the identifier you defined for this
 * property in the OneNET console's Thing Model (TSL) editor - rename it
 * here if you used a different identifier. */
#define MQTT_TOPIC_PROPERTY_POST       "$sys/" MQTT_USERNAME "/" MQTT_CLIENT_ID "/thing/property/post"
#define MQTT_TOPIC_PROPERTY_POST_REPLY "$sys/" MQTT_USERNAME "/" MQTT_CLIENT_ID "/thing/property/post/reply"

/* Stand-in for the real BLE sensor reading, until module A is wired in. */
#define TEMPERATURE_REPORT_INTERVAL_MS 3000
#define TEMPERATURE_MIN 25.0f
#define TEMPERATURE_MAX 44.0f

static esp_mqtt_client_handle_t s_client = NULL;

static float random_temperature(void)
{
    float span = TEMPERATURE_MAX - TEMPERATURE_MIN;
    return TEMPERATURE_MIN + ((float)esp_random() / (float)UINT32_MAX) * span;
}

static void temperature_report_task(void *arg)
{
    while (1) {
        mqtt_app_report_temperature(random_temperature());
        vTaskDelay(pdMS_TO_TICKS(TEMPERATURE_REPORT_INTERVAL_MS));
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "Connected to broker");
        esp_mqtt_client_subscribe(client, MQTT_TOPIC_PROPERTY_POST_REPLY, 1);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "Disconnected from broker");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "Subscribed, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "Published, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "Data received, topic=%.*s, data=%.*s",
                 event->topic_len, event->topic, event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT error, type=%d", event->error_handle->error_type);
        break;
    default:
        break;
    }
}

void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
        .credentials.client_id = MQTT_CLIENT_ID,
        .credentials.username = MQTT_USERNAME,
        .credentials.authentication.password = MQTT_PASSWORD,
    };

    s_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(s_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(s_client);

    xTaskCreate(temperature_report_task, "temp_report", 4096, NULL, 5, NULL);
}

void mqtt_app_report_temperature(float celsius)
{
    if (s_client == NULL) {
        return;
    }

    static uint32_t msg_id = 1;
    char payload[128];
    snprintf(payload, sizeof(payload),
             "{\"id\":\"%" PRIu32 "\",\"version\":\"1.0\",\"params\":{\"temperature\":{\"value\":%.1f}}}",
             msg_id++, celsius);

    esp_mqtt_client_publish(s_client, MQTT_TOPIC_PROPERTY_POST, payload, 0, 1, 0);
}
