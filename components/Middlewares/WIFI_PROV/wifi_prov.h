#ifndef __WIFI_PROV_H__
#define __WIFI_PROV_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * On first boot (no Wi-Fi credentials stored yet), opens a SoftAP named
 * "PROV_XXXXXX" and waits for the ESP SoftAP Provisioning phone app to
 * submit the home router's SSID/password. On later boots, connects
 * directly using the credentials already stored in NVS.
 *
 * Blocks until an IP address has been obtained.
 */
void wifi_prov_connect(void);

#ifdef __cplusplus
}
#endif

#endif
