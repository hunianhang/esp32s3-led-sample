#ifndef __MQTT_APP_H__
#define __MQTT_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Connects to the MQTT broker configured in mqtt_secrets.h.
 * Call after Wi-Fi is connected (e.g. after wifi_prov_connect()). */
void mqtt_app_start(void);

/* Reports the "temperature" Thing Model property to OneNET.
 * No-op if the client hasn't connected yet. */
void mqtt_app_report_temperature(float celsius);

#ifdef __cplusplus
}
#endif

#endif
