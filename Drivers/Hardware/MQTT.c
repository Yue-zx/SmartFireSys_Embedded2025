#include "stm32h7xx_hal.h"
#include <stdio.h>
#include <string.h>
#include "MQTT.h"

// UART定义
extern UART_HandleTypeDef huart1;
#define WIFI_UART &huart1
#define AT_TIMEOUT 3000
#define MAX_RETRY 3


MQTT_Config mqtt_cfg = {0};

// 发送AT指令函数
HAL_StatusTypeDef sendATCommand(const char *cmd, const char *expect, uint32_t timeout) {
    uint8_t retry = 0;
    uint8_t rxBuf[128] = {0};

    for (retry = 0; retry < MAX_RETRY; retry++) {
        HAL_UART_Transmit(WIFI_UART, (uint8_t *)cmd, strlen(cmd), HAL_MAX_DELAY);

        memset(rxBuf, 0, sizeof(rxBuf));
        uint32_t startTick = HAL_GetTick();
        uint16_t index = 0;

        while ((HAL_GetTick() - startTick) < timeout) {
            if (HAL_UART_Receive(WIFI_UART, &rxBuf[index], 1, 10) == HAL_OK) {
                if (strstr((char *)rxBuf, expect) != NULL) {
                    return HAL_OK;
                }
                if (++index >= sizeof(rxBuf) - 1) break;
            }
        }
        HAL_Delay(200);  // 重试前延迟
    }

    return HAL_ERROR;
}

// ESP AT相关封装
HAL_StatusTypeDef ESP_AT_RST(void) {
    return sendATCommand("AT+RST\r\n", "ready", 5000);
}

HAL_StatusTypeDef ESP_AT_CWMODE(void) {
    return sendATCommand("AT+CWMODE=1\r\n", "OK", AT_TIMEOUT);
}

HAL_StatusTypeDef ESP_AT_CWJAP(const char *ssid, const char *password) {
    char cmd[128] = {0};
    snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, password);
    return sendATCommand(cmd, "OK", 10000);
}

HAL_StatusTypeDef ESP_AT_MQTTUSERCFG(const char *client_id, const char *username, const char *password) {
    char cmd[128] = {0};
    snprintf(cmd, sizeof(cmd),
             "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n",
             client_id, username, password);
    return sendATCommand(cmd, "OK", AT_TIMEOUT);
}

HAL_StatusTypeDef ESP_AT_MQTTCONN(void) {
    return sendATCommand("AT+MQTTCONN=0,\"bemfa.com\",9501,1\r\n", "+MQTTCONNECTED:0,1", 10000);
}

HAL_StatusTypeDef ESP_AT_MQTTSUB(const char *topic) {
    char cmd[128] = {0};
    snprintf(cmd, sizeof(cmd), "AT+MQTTSUB=0,\"%s\",1\r\n", topic);
    return sendATCommand(cmd, "OK", AT_TIMEOUT);
}

HAL_StatusTypeDef ESP_AT_MQTTPUB(const char *topic, const char *message) {
    char cmd[256] = {0};
    snprintf(cmd, sizeof(cmd), "AT+MQTTPUB=0,\"%s\",\"%s\",0,0\r\n", topic, message);
    return sendATCommand(cmd, "OK", AT_TIMEOUT);
}

// 初始化函数
HAL_StatusTypeDef MQTT_Init(const char *wifi_ssid, const char *wifi_password, const char *client_id) {
    strncpy(mqtt_cfg.wifi_ssid, wifi_ssid, sizeof(mqtt_cfg.wifi_ssid) - 1);
    strncpy(mqtt_cfg.wifi_password, wifi_password, sizeof(mqtt_cfg.wifi_password) - 1);
    strncpy(mqtt_cfg.client_id, client_id, sizeof(mqtt_cfg.client_id) - 1);

    ESP_AT_RST();
    HAL_Delay(500);
    ESP_AT_CWMODE();
    HAL_Delay(200);
    if (ESP_AT_CWJAP(mqtt_cfg.wifi_ssid, mqtt_cfg.wifi_password) != HAL_OK)
        return HAL_ERROR;
    HAL_Delay(200);
    if (ESP_AT_MQTTUSERCFG(mqtt_cfg.client_id, "", "") != HAL_OK)
        return HAL_ERROR;
    HAL_Delay(200);
    if (ESP_AT_MQTTCONN() != HAL_OK)
        return HAL_ERROR;

    mqtt_cfg.initialized = 1;
    mqtt_cfg.connected = 1;
    return HAL_OK;
}

// 自动重连检查
HAL_StatusTypeDef MQTT_CheckConnection(void) {
    if (!mqtt_cfg.initialized) return HAL_ERROR;

    if (sendATCommand("AT+MQTTCONN?\r\n", "+MQTTCONNECTED:0,1", 500) == HAL_OK) {
        mqtt_cfg.connected = 1;
        return HAL_OK;
    }

    if (sendATCommand("AT+CWJAP?\r\n", mqtt_cfg.wifi_ssid, 1000) != HAL_OK) {
        if (ESP_AT_CWJAP(mqtt_cfg.wifi_ssid, mqtt_cfg.wifi_password) != HAL_OK)
            return HAL_ERROR;
        HAL_Delay(500);
    }

    if (ESP_AT_MQTTCONN() == HAL_OK) {
        mqtt_cfg.connected = 1;
        return HAL_OK;
    }

    mqtt_cfg.connected = 0;
    return HAL_ERROR;
}

// 只负责发布数据
void MQTT_update(char *topic, char *value) {
    ESP_AT_MQTTPUB(topic, value);
}

