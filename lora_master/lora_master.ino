/*
    Author: Mehmet Furkan KAYA
    Description: Master1
*/

#include <WiFi.h>
#include <LoRa_E22.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <HardwareSerial.h>


const char* WiFi_SSID = "mrfurkankaya";
const char* WiFi_PASS = "1234567888";
const char* MQTT_SERVER = "192.168.3.21";
const char* MQTT_CLIENT_ID = "route";
const char* MQTT_SUB_TOPIC = "route_subscribe";


WiFiClient wifiInstance;
PubSubClient mqtt(wifiInstance);


#define M0 4
#define M1 6
#define TX 17
#define RX 18
#define led 42
#define address 1
#define channel 36
#define sendToAddress 2


typedef struct {
  char route[200];
} MASTER_DATA;

MASTER_DATA masterData;


HardwareSerial mySerial(1);
LoRa_E22 e22(TX, RX, &mySerial, UART_BPS_RATE_9600);


void wifiConnect(void);
void mqttConnect(void);
void wifiMqttCheck(void);
void callback(char* topic, byte* message, unsigned int length);
void publishMessage(const char* topic, const char* message);
void loraE22Settings(void);
void loraSend(void);


void setup() {
  Serial.begin(9600);
  while (!Serial) { ; }
  delay(200);

  e22.begin();

  pinMode(M0, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(led, OUTPUT);

  loraE22Settings();

  digitalWrite(M0, LOW);
  digitalWrite(M1, LOW);

  delay(500);

  wifiConnect();
  mqtt.setServer(MQTT_SERVER, 1883);
  mqtt.setCallback(callback);
  mqttConnect();
}

void loop() {
  wifiMqttCheck();
}

// JSON
String toJson() {
  StaticJsonDocument<200> doc;
  JsonArray routeArray = doc.createNestedArray("route");
  routeArray.add(masterData.route);

  String jsonMessage;
  serializeJson(doc, jsonMessage);
  return jsonMessage;
}

// WiFi
void wifiConnect(void) {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WiFi_SSID, WiFi_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

// MQTT
void mqttConnect() {
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqtt.connect(MQTT_CLIENT_ID)) {
      Serial.println("connected");
      mqtt.subscribe(MQTT_SUB_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      delay(2000);
    }
  }
}

// Check WiFi and MQTT Connection
void wifiMqttCheck(void) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost, reconnecting...");
    wifiConnect();
  }

  if (!mqtt.connected()) {
    Serial.println("MQTT connection lost, reconnecting...");
    mqttConnect();
  }

  mqtt.loop();
}

// MQTT Subscribe
void callback(char* topic, byte* message, unsigned int length) {
  char messageContent[400];
  if (length >= 400) {
    Serial.println("Error: Message too long.");
    return;
  }

  for (unsigned int i = 0; i < length; i++) {
    messageContent[i] = (char)message[i];
  }
  messageContent[length] = '\0';

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, messageContent);

  if (error) {
    Serial.print("JSON parsing error: ");
    Serial.println(error.c_str());
    return;
  }

  JsonArray route = doc["route"].as<JsonArray>();

  if (!route.isNull()) {
    String routeString = "";
    for (JsonVariant item : route) {
      if (item.is<JsonObject>()) {
        for (JsonPair keyValue : item.as<JsonObject>()) {
          if (!routeString.isEmpty()) {
            routeString += "|";
          }
          routeString += keyValue.key().c_str();
          routeString += ":";
          routeString += keyValue.value().as<const char*>();
        }
      }
    }

    strncpy(masterData.route, routeString.c_str(), sizeof(masterData.route) - 1);
    masterData.route[sizeof(masterData.route) - 1] = '\0';

    Serial.println("Parsed Route: " + String(masterData.route));
    loraSend();
  }
}

// LoRa E22 Settings
void loraE22Settings() {
  digitalWrite(M0, LOW);
  digitalWrite(M1, HIGH);

  ResponseStructContainer c;
  c = e22.getConfiguration();
  Configuration configuration = *(Configuration*)c.data;

  configuration.ADDL = lowByte(address);
  configuration.ADDH = highByte(address);
  configuration.CHAN = channel;
  configuration.NETID = 0x00;

  configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
  configuration.OPTION.subPacketSetting = SPS_240_00;
  configuration.OPTION.transmissionPower = POWER_22;

  configuration.SPED.uartBaudRate = UART_BPS_9600;
  configuration.SPED.uartParity = MODE_00_8N1;
  configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;
  configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
  configuration.TRANSMISSION_MODE.WORTransceiverControl = WOR_RECEIVER;
  configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
  configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION;
  configuration.TRANSMISSION_MODE.enableRepeater = REPEATER_DISABLED;
  configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;

  ResponseStatus rs = e22.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
  c.close();
}

// LoRa Send
void loraSend(){
  ResponseStatus rs = e22.sendFixedMessage(highByte(sendToAddress), lowByte(sendToAddress), channel, &masterData, sizeof(MASTER_DATA));
  Serial.println(rs.getResponseDescription());
}
