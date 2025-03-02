## 📶 `LoRa EBYTE E22900T22D & ESP32-S3 Transceiver`

<br>

<div align="center">
<h3>⚡LoRa ESP32-S3 Shield </h3>
<img src="./images/LoRa.jpg" alt="lora">
</div>

<br>

<details>
<summary> 🔶 LoRa USB Stick </summary>
<img src="./images/stick.jpg" alt="stick">
</details>

<br>
<br>

- ✅ LoRa Messaging Application

<br>

Used Technologies in The Project:

[![ESP32](https://img.shields.io/badge/ESP32%20s3-C70D2C?style=for-the-badge&logo=esphome&color=0F0F11)](./datasheet/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf)
[![Mosquitto](https://img.shields.io/badge/Mosquitto-000?style=for-the-badge&logo=eclipsemosquitto&logoColor=white&color=3C5280)](https://mosquitto.org/)
[![LoRa](https://img.shields.io/badge/LoRa-C70D2C?style=for-the-badge&logo=wikiquote&color=380953)](./datasheet/E22-900T22D_UserManual_EN_v1.3.pdf)
[![LoRa Shield](https://img.shields.io/badge/LoRa%20Shield-C70D2C?style=for-the-badge&&color=DA0E29)](https://fixaj.com/s3/)

<br>

> Docker Mosquitto [Usage](https://github.com/furkankayam/mqtt-broker-allow-anonymous)

<br>

##### ⚙️ WiFi and MQTT Settings

```ino
const char* WiFi_SSID = "<WIFI_NAME>";
const char* WiFi_PASS = "<WIFI_PASSWORD>";
const char* MQTT_SERVER = "<MQTT_SERVER_IP>";
const char* MQTT_CLIENT_ID = "route";
const char* MQTT_SUB_TOPIC = "route_subscribe";
```

<br>

# License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details
