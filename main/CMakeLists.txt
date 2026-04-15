idf_component_register(
    SRCS
        "main.cpp"
        "wifi.cpp"
        "webserver.cpp"
        "settings.cpp"
    INCLUDE_DIRS "."
    REQUIRES
        esp_wifi
        esp_http_server
        nvs_flash
        esp_netif
        driver
        mbedtls
)
