#ifndef __MQTT_H__
#define __MQTT_H__

#include "stm32h7xx_hal.h"

// MQTT 配置结构体
typedef struct {
    char wifi_ssid[64];
    char wifi_password[64];
    char client_id[64];
    uint8_t initialized;
    uint8_t connected;
} MQTT_Config;

// 在 .c 文件中定义的全局配置变量
extern MQTT_Config mqtt_cfg;

// =============================
// AT指令发送及ESP控制函数
// =============================
HAL_StatusTypeDef sendATCommand(const char *cmd, const char *expect, uint32_t timeout);
HAL_StatusTypeDef ESP_AT_RST(void);
HAL_StatusTypeDef ESP_AT_CWMODE(void);
HAL_StatusTypeDef ESP_AT_CWJAP(const char *ssid, const char *password);
HAL_StatusTypeDef ESP_AT_MQTTUSERCFG(const char *client_id, const char *username, const char *password);
HAL_StatusTypeDef ESP_AT_MQTTCONN(void);
HAL_StatusTypeDef ESP_AT_MQTTSUB(const char *topic);
HAL_StatusTypeDef ESP_AT_MQTTPUB(const char *topic, const char *message);

// =============================
// MQTT 控制接口
// =============================

// 初始化 WiFi 和 MQTT
HAL_StatusTypeDef MQTT_Init(const char* wifi_ssid, const char* wifi_password, const char* client_id);

// 检查连接状态并自动重连（如有需要）
HAL_StatusTypeDef MQTT_CheckConnection(void);

// 发送 MQTT 消息（不自动重连）
void MQTT_update(char* topic, char* value);

#endif  // __MQTT_H__
