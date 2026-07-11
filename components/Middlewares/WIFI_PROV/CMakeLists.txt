idf_component_register(SRCS "wifi_prov.c"
                       INCLUDE_DIRS "."
                       REQUIRES nvs_flash esp_wifi esp_event esp_netif network_provisioning)
