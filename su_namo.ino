#include <ESP8266WiFi.h> 
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define esp 2
#define rele 14
#define rele_k 12
#define rele_b 15
#define led 16
#define ONE_WIRE_BUS 4

#define YOURIP

int laikas = 30000; //laikas kas kiek siuncia duomenis ir refreshinas.
int temperatura = 36; //temperatura kada isijungia katilo siurbli.
int virimas = 90; //max temperatura kai issijungia boilerio siurblys, kad neuzvirtu.

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

//termodavikliu adresai

byte katilas[8] =  {0x28, 0x61, 0x64, 0x08, 0xEA, 0x7c, 0x98, 0x7D};
byte boileris[8] = {0x10, 0xB1, 0xAF, 0xD9, 0x02, 0x08, 0x00, 0x3C};
byte namo_sensorius[8] = { 0x28, 0xB7, 0x61, 0xC9, 0x02, 0x00, 0x00, 0xA5 };
byte grindu_sensorius[8] = { 0x28, 0x61, 0x64, 0x08, 0xEA, 0x25, 0x4A, 0x53 };

const char* ssid = ""; // enter your WiFi SSID
const char* wifi_password = ""; // enter your WiFi password
 
// Make sure to update this for your own MQTT Broker!
const char* mqtt_server = YOURIP;
const char* mqtt_topic = "namas/katiline/katilas";
const char* mqtt_topic1 = "namas/katiline/boileris";
const char* mqtt_topick = "namas/katiline/katilas_siurbl"; 
const char* mqtt_topicb = "namas/katiline/boileris_siurbl";
const char* mqtt_topic_zeme = "namas/katiline/t_i_grindis";
const char* mqtt_topic_namas = "namas/svetaine/t_oro";
const char* mqtt_username = "mosquito";
const char* mqtt_password = "mosquito";
const char* clientID = "Dallas";


  
WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient);
void setup(void)
{
 
  Serial.print("Connecting to ");
  Serial.println(ssid);
  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led,LOW);
    delay(250);
    digitalWrite(led,HIGH);
    delay(250);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
 
   client.setServer(YOURIP, YOURPORT);        // Modify with your mqtt server ip and port
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
    digitalWrite(esp,LOW);
  }
}

void loop(void)
{
  digitalWrite(led,LOW);
  // parequestina temperaturos ir prisijungia prie mqtt.
  sensors.requestTemperatures();
  int temp = (sensors.getTempC(katilas));
  int temp1 = (sensors.getTempC(boileris)) + 9;
  int namas = (sensors.getTempC(namo_sensorius));
  int zeme = (sensors.getTempC(grindu_sensorius));
    char buf [2];
    unsigned int p=2;
    client.connect(clientID, mqtt_username, mqtt_password);
    delay(10);
   
    // Jei neveikia bent vienas is termo sensoriu.
    if(temp == -127 || temp1 == -127){
      digitalWrite(rele_k,LOW);
      digitalWrite(rele_b,LOW);
      digitalWrite(led,HIGH);
      delay(250);
      digitalWrite(led,LOW);
      delay(250);
      digitalWrite(led,HIGH);
      delay(250);
      digitalWrite(led,LOW);
      delay(250);
      if (temp == -127){
        sprintf (buf, "%02i", temp);
        client.publish(mqtt_topic,"Error :(");
        Serial.print("Error :(");
        Serial.println(); 
      }
      else{
        sprintf (buf, "%02i", temp);
        client.publish(mqtt_topic,(const uint8_t*)buf,2);
        Serial.print(temp);
        Serial.println();
      }
      if (temp1 == -127){
        sprintf (buf, "%02i", temp1);
        client.publish(mqtt_topic1,"Error :(");
        Serial.print("Error :(");
        Serial.println(); 
      }
      else{
        sprintf (buf, "%02i", temp1);
        client.publish(mqtt_topic1,(const uint8_t*)buf,2);
        Serial.print(temp1);
        Serial.println();
      }
    client.publish(mqtt_topick,"ON");
    client.publish(mqtt_topicb,"ON"); 
    delay(laikas);
    }
    // Kai sensoriai veikia teisingai.
    else{
    Serial.print ("Namo temp: ");
    Serial.print (namas);
    Serial.println();
    Serial.print ("I grindis temp: ");
    Serial.print (zeme);
    Serial.println();
    
    sprintf (buf, "%02i", temp);
    client.publish(mqtt_topic,(const uint8_t*)buf,2);
    sprintf (buf, "%02i", temp1);
    client.publish(mqtt_topic1,(const uint8_t*)buf,2);
    sprintf (buf, "%02i", namas);
    client.publish(mqtt_topic_namas,(const uint8_t*)buf,2);
    sprintf (buf, "%02i", zeme);
    client.publish(mqtt_topic_zeme,(const uint8_t*)buf,2);
    
    if (temp1 >= virimas){
Serial.print(temp);
Serial.println();
Serial.print(temp1);
Serial.println();
digitalWrite(rele_b,LOW);
client.publish(mqtt_topicb,"ON");
    if (temp > temperatura){
        digitalWrite(rele_k,LOW);
        client.publish(mqtt_topick,"ON");
    }
    else {
        digitalWrite(rele_k,HIGH);
        client.publish(mqtt_topick,"OFF");
    }
delay(laikas);
}

else{
if (temp >= temperatura && temp1 > temp){
digitalWrite(rele_k,LOW);
digitalWrite(rele_b,HIGH);
Serial.print(temp);
Serial.println();
Serial.print(temp1);
Serial.println();
client.publish(mqtt_topick,"ON");
client.publish(mqtt_topicb,"OFF");
}

else if (temp < temperatura && temp1 > temp){
digitalWrite(rele_k,HIGH);
digitalWrite(rele_b,HIGH);
Serial.print(temp);
Serial.println();
Serial.print(temp1);
Serial.println();
client.publish(mqtt_topick,"OFF");
client.publish(mqtt_topicb,"OFF");
}

else if (temp < temperatura && temp1 < temp){
digitalWrite(rele_k,HIGH);
digitalWrite(rele_b,LOW);
Serial.print(temp);
Serial.println();
Serial.print(temp1);
Serial.println();
client.publish(mqtt_topick,"OFF");
client.publish(mqtt_topicb,"ON");
}

else if (temp >= temperatura && temp1 < temp){
digitalWrite(rele_k,LOW);
digitalWrite(rele_b,LOW);
Serial.print(temp);
Serial.println();
Serial.print(temp1);
Serial.println();
client.publish(mqtt_topick,"ON");
client.publish(mqtt_topicb,"ON");
}
delay(laikas);
}
}
}
