#include <SoftwareSerial.h>
const String PHONE_1 = "+8801753001421";

#define rxPin 2
#define txPin 3
SoftwareSerial sim800L(rxPin, txPin);
#define gas_sensor_pin 5
boolean gas_flag = 0;  //--> gas_flag = 0 means no gas detected
#define buzzer_pin 4

void setup() {
  Serial.begin(115200);  //--> Begin serial communication: Arduino IDE (Serial Monitor)
  sim800L.begin(9600);   //--> Begin serial communication: SIM800L
  pinMode(gas_sensor_pin, INPUT);
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, LOW);
  Serial.println("Initializing...");
  sim800L.println("AT");  //--> Once the handshake test is successful, it will back to OK
  delay(1000);
  sim800L.println("AT+CMGF=1");
  delay(1000);
}

void loop() {
  while (sim800L.available()) {
    Serial.println(sim800L.readString());
  }
  int gas_value = digitalRead(gas_sensor_pin);
  //--> The gas is detected, trigger Alarm and send sms
  if (gas_value == LOW) {
    digitalWrite(buzzer_pin, HIGH);
    if (gas_flag == 0) {
      Serial.println("Gas is Detected.");
      gas_flag = 1;
      send_multi_sms();
      make_multi_call();
    }
  }
  //No gas is detected, turn OFF Alarm
  else {
    digitalWrite(buzzer_pin, LOW);
    gas_flag = 0;
  }
}

void send_multi_sms() {
  if (PHONE_1 != "") {
    Serial.print("Phone 1 : ");
    send_sms("You are in Danger...Gas is Detected", PHONE_1);
  }
}

void make_multi_call() {
  if (PHONE_1 != "") {
    Serial.print("Phone 1 : ");
    make_call(PHONE_1);
  }
}

void send_sms(String text, String phone) {
  Serial.println("sending sms....");
  delay(50);
  sim800L.print("AT+CMGF=1\r");
  delay(1000);
  sim800L.print("AT+CMGS=\"" + phone + "\"\r");
  delay(1000);
  sim800L.print(text);
  delay(100);
  sim800L.write(0x1A);  //ascii code for ctrl-26 //Serial2.println((char)26); //ascii code for ctrl-26
  delay(5000);
}

void make_call(String phone) {
  Serial.println("calling....");
  sim800L.println("ATD" + phone + ";");
  delay(20000);  //20 sec delay
  sim800L.println("ATH");
  delay(1000);  //1 sec delay
}
