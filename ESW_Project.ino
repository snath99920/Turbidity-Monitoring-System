#include "WiFi.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include <EEPROM.h>

char* wifi_ssid[3] = {"JioFi3_126C36","Dolt","Honor 10 Lite"};
char* wifi_pwd[3] = {"4p7ce3t8vv","asdf1234","ISSdikkk"};
//String cse_ip = "192.168.43.59";
String cse_ip = "139.59.42.21";

StaticJsonBuffer<200> jsonBuffer;
JsonObject& user_data = jsonBuffer.createObject();
JsonObject& temp_user_data = jsonBuffer.createObject();
JsonObject& sensor_data = jsonBuffer.createObject();

String cse_port = "8080";
String server = "http://"+cse_ip+":"+cse_port+"/~/in-cse/in-name/";

String createCI(String server, String ae, String cnt, String val)
{
  HTTPClient http;
  http.begin(server + ae + "/" + cnt + "/");
//  String postRequest = String() + "POST " + url + " HTTP/1.1\r\n" +
//                       "Host: " + CSE_IP + ":" + CSE_HTTP_PORT + "\r\n" +
//                       "X-M2M-Origin: " + CSE_M2M_ORIGIN + "\r\n" +
//                       "Content-Type: application/json;ty=" + ty + "\r\n" +
//                       "Content-Length: " + rep.length() + "\r\n"
//                       "Connection: close\r\n\n" +
//                       rep;
//                       
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");
  http.addHeader("Content-Length", "100");
  http.addHeader("Connection", "close");
  int code = http.POST("{\"m2m:cin\": {\"cnf\": \"text/plain:0\",\"con\": "+ String(val) +"}}");
  http.end();
  Serial.print(code);
  if(code==-1)
  {
    ESP.restart();
  }
  delay(300);
}

void setup()
{
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  EEPROM.begin(8);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  pinMode(19,OUTPUT);
  digitalWrite(19,LOW);
  int wifi_conn_no = EEPROM.readInt(4);
  wifi_conn_no%=3;
  if(wifi_conn_no<0)
  {
    wifi_conn_no=0;
  }
  EEPROM.writeInt(4,(wifi_conn_no+1)%3);
  EEPROM.commit();
  Serial.println(wifi_conn_no);
  WiFi.begin(wifi_ssid[wifi_conn_no], wifi_pwd[wifi_conn_no]);
  int timee=0;
  Serial.println("Code Started");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    timee++;
    if(timee>20)
    {
      ESP.restart();
    }
    Serial.println("Connecting to WiFi..");
  }
  pinMode(32,INPUT);
  digitalWrite(19,HIGH);
  Serial.println("Connected to the WiFi network");
  Serial.println("Setup done");
}
float voltage;
int total_time=0;
void loop()
{
  //To GET values
  //http.begin("http://192.168.0.113:8080/~/in-cse/in-name/verte001/DATA/la");
  //http.begin("http://192.168.0.113:8080/~/in-cse");
  //int httpCode = http.GET();

  int sensorValue = EEPROM.readInt(0);
  voltage = sensorValue * (5.0 / 4096.0);
  float ntu;
  if(voltage < 2.5)
  {
      ntu = 3000;
  }
  else
  {
      ntu = -1120.4*voltage*voltage+5742.3*voltage-4353.8; 
  }
  Serial.print ("Sensor Output (ntu):");
  Serial.println (ntu);
  Serial.println();

  String sensor2 = String(ntu); 

  String sensor_string = sensor2;

  sensor_string = "\"" + sensor_string + "\""; // DO NOT CHANGE THIS LINE

  // Send data to OneM2M server
  createCI(server, "Team7_Turbidity_monitoring", "node_1", sensor_string);
  
  sensorValue = analogRead(32);
  EEPROM.writeInt(0,sensorValue);
  EEPROM.commit();
//  Serial.println(total_time);
//  float ss = sensorValue;
//  float s = (ss*5.0)/4096.0;
//  Serial.println(sensorValue);

//  delay(500);
  delay(5*60*1000); // DO NOT CHANGE THIS LINE/
  total_time++;
  if(total_time==10)
  {
    ESP.restart();
  }
  
}

