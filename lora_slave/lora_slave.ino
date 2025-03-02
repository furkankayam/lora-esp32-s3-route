/*
    Author: Mehmet Furkan KAYA
    Description: Slave2
*/

#include <LoRa_E22.h>


#define M0 4
#define M1 6
#define TX 17
#define RX 18
#define led 42
#define address 2
#define channel 36

 
HardwareSerial mySerial(1);
LoRa_E22 e22(TX, RX, &mySerial, UART_BPS_RATE_9600);

 
typedef struct {
  char route[200];
} MASTER_DATA;

MASTER_DATA masterData;


boolean LORA_AVAILABLE = false;

void loraE22Settings(void);
void loraListen(void);
void printInfo(void);
 
 
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
}
 
void loop() {
  loraListen();
  if(LORA_AVAILABLE == true){
    printInfo();    
    LORA_AVAILABLE = false;
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
  configuration.TRANSMISSION_MODE.WORTransceiverControl = WOR_TRANSMITTER;
  configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
  configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION;
  configuration.TRANSMISSION_MODE.enableRepeater = REPEATER_DISABLED;
  configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
 
  ResponseStatus rs = e22.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
  c.close();
}

// LoRa Listen
void loraListen(){
  while (e22.available()  > 0) {
    ResponseStructContainer rsc = e22.receiveMessage(sizeof(MASTER_DATA));
    masterData = *(MASTER_DATA*) rsc.data;

    rsc.close();

    LORA_AVAILABLE = true;
  }
}

// Print Info
void printInfo(){
  Serial.println("\n************************************");
  Serial.println("RECEIVE DATA: " + String(masterData.route));
  Serial.println("\n************************************");
}