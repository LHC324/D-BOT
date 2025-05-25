/**
  *   Copyright (C) 2025 dingmos. All rights reserved.
  * @file    network.cpp
  * @author  dingmos
*/
#include "hal.h"
#include "WiFiManager.h"
#include "nvs.h"


WiFiManager wm;

// 参数名、标签、默认值、最大长度
static WiFiManagerParameter game_ctrlr_addr("game_ctrlr_addr", 
                    "手柄蓝牙地址", "04:0A:11:11:90:10", 18);

static WiFiManagerParameter mqtt_host("mqtt_host", "MQTT 服务器地址", "192.168.1.12", 32);
static WiFiManagerParameter mqtt_port("mqtt_port", "MQTT 端口号", "1883", 8);
static WiFiManagerParameter mqtt_topic_prefix("mqtt_topic_prefix", 
                            "MQTT 消息前缀", "dingmos", 32);

void config_mode_cb(WiFiManager *wifi_manager)
{
    log_i("进入网络配置模式...");
    log_i("%s", WiFi.softAPIP().toString().c_str());

    log_i("请连接 WiFi %s 进行配置", wifi_manager->getConfigPortalSSID().c_str());
}

static void save_mqtt_config(void)
{
    String host = String(mqtt_host.getValue());
    int port = atoi(mqtt_host.getValue());
    String prefix = String(mqtt_topic_prefix.getValue());

    log_i("save mqtt server: %s:%d", host.c_str(), port);
    nvs_set_mqtt_config(host, port, String(""),  String(""), prefix);
}

void save_config_cb()
{
    log_i("save game ctrlr addr %s", game_ctrlr_addr.getValue());
    nvs_set_string(GAME_CTRLR, GAME_CTRLR_ADDR_KEY, 
                    game_ctrlr_addr.getValue());

    save_mqtt_config();
    
    log_i("配置已保存，准备重启");
    ESP.restart();
}

int HAL::network_init(void)
{
    String macAddress = WiFi.macAddress();
    macAddress.replace(":", "");
    String apName = "DBOT_" + macAddress.substring(macAddress.length() - 6);

    wm.addParameter(&game_ctrlr_addr);
    wm.addParameter(&mqtt_host);
    wm.addParameter(&mqtt_port);
    wm.addParameter(&mqtt_topic_prefix);
    wm.setAPCallback(config_mode_cb);
    wm.setSaveConfigCallback(save_config_cb);
    wm.setConfigPortalTimeout(300);

    if (!wm.autoConnect(apName.c_str())) {
        log_e("配置超时，重启设备");
        ESP.restart();
    }

    log_i("网络连接成功！");
    return 0;
}

std::string HAL::get_wifi_ssid(void)
{
    return std::string(wm.getWiFiSSID().c_str());
}

std::string HAL::get_wifi_passwd(void)
{
    return std::string(wm.getWiFiPass().c_str());
}