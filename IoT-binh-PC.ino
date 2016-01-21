#include <Event.h>
#include <Timer.h>
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <Adafruit_ESP8266.h>
// Must call begin() on the stream(s) before using Adafruit_ESP8266 object.
#define ESP_RX   12
#define ESP_TX   13
#define ESP_RST  11
#define ESP_SSID "admin" // Your network name here
#define ESP_PASS "dathaymatkhau" // Your network password here
#define HOST     "PING"     // Host to contact
#define SERVER     "192.168.1.34" // Web page to request
#define PORT     "8080"                     // 80 = HTTP default port
#define SERVERIP "192.168.1.91"
const char Content[] PROGMEM =
  "<html>"
  "<head><meta charset=\"utf-8\"/>"
  "<script src=\"https://code.jquery.com/jquery-1.11.3.min.js\">"
  "</script>"
  "<script>"  
  "$(document).ready(function(){"
  "var swFunc = function(){"
  "var btnID = $(this).data(\"id\");"
  "var obj = this;"
  "if ($(obj).data(\"status\") == 0){"
  "$.get(\"/?onl\" + btnID, function(data, status){"
  "if ((status == \"success\") && (data == \"{\"stt\":\"1\"}\")){"
  "$(obj).css(\"background-color\",\"#ff0000\");"
  "$(obj).data(\"status\",\"1\");"
  "}"
  "});"
  "}"
  "else{"
  "$.get(\"/?ofl\" + btnID, function(data, status){"
  "if ((status == \"success\") && (data == \"{\"stt\":\"0\"}\")){"
  "$(obj).css(\"background-color\",\"buttonface\");"
  "$(obj).data(\"status\",\"0\");"
  "}"
  "});"
  "}"
  "};"
  "$(\".light\").click(swFunc);"
  "});"
  "</script>"
  "<script>"  
  "$(document).ready(function(){"
  "var alFunc = function(){"
  "var btnID = $(this).data(\"id\");"
  "var obj = this;"
  "if ($(obj).data(\"status\") == 0){"
  "$.get(\"/?onal\" + btnID, function(data, status){"
  "if ((status == \"success\") && (data == \"1\")){"
  "$(obj).css(\"background-color\",\"#ff0000\");"
  "$(obj).data(\"status\",\"1\");"
  "}"
  "});"
  "}"
  "else{"
  "$.get(\"/?ofal\" + btnID, function(data, status){"
  "if ((status == \"success\") && (data == \"0\")){"
  "$(obj).css(\"background-color\",\"buttonface\");"
  "$(obj).data(\"status\",\"0\");"
  "}"
  "});"
  "}"
  "};"
  "$(\".alarm\").click(alFunc);"
  "});"
  "</script>"
  "<script>"  
  "function url(){"
  "window.location=\"/?al\"+document.getElementById(\"mode\").value+document.getElementById(\"hours\").value+\"h\"+document.getElementById(\"minutes\").value+\"m\"+document.getElementById(\"l\").value;"
  "}"
  "</script>"
  "</head>"
  "<body bgcolor=\"#009933\">"
  "<center>"
  //    "<p><h1 style=\"color:#000033;background:#00CC33\">BẢNG ĐIỀU KHIỂN</h1></p>"
  "<select id=\"mode\"><option value=\"on\">Bật</option><option value=\"of\">Tắt</option></select>"
  "<select id=\"l\"><option value=\"d1\">Đèn 1</option><option value=\"d2\">Đèn 2</option><option value=\"d3\">Đèn 3</option><option value=\"d4\">Đèn 4</option></select>"
  "<input type=\"number\"id=\"hours\"min=\"0\"max=\"23\">"
  "<input type=\"number\"id=\"minutes\"min=\"0\"max=\"60\">"
  "<input type=\"button\" value=\"Đặt hẹn giờ\"onclick=\"url();\"/><br/><br/>"
  "<center><table border=\"1\"style=\"text-align:center;background-color:#FFFF99\">";

//Leonardo
//Serial_ & dbgTerminal = Serial;
//HardwareSerial & espSerial = Serial1;

////UNO & M328P
//#include <SoftwareSerial.h>
//SoftwareSerial espSerial(10, 11); // RX, TX
//HardwareSerial & monitor = Serial;

//MEGA2560
HardwareSerial & monitor = Serial;
HardwareSerial & espSerial = Serial1;

#define BUFFER_SIZE 128
//Timer checkAlarm;
//RTC_DS1307 RTC;
//DateTime alarmTime = DateTime(2015, 12, 2, 23, 27, 0);
//DateTime alarmStopTime = DateTime(2015, 12, 2, 23, 28, 0);

// set pin numbers:
const int relay[] = {2, 3, 4, 5, 6, 7, 8, 9};
//const int ESP8266_CHPD = 8;
// Variables will change:
char buffer[BUFFER_SIZE];

int result = 0;
boolean OK = true;
boolean TIMEOUT = false;
char relayStatus[] = {0, 0, 0, 0, 0, 0, 0, 0};
int alarmHour[8] = {1, 1, 1, 1, 3, 3, 3, 3};
int alarmMinute[8] = {30, 30, 30, 30, 30, 30, 30, 30};
int alarmStatus[8] = {1, 1, 0, 0, 0, 0, 0, 0};
int i;
int m = 0;
int lp = 0;
Adafruit_ESP8266 server(&espSerial, &Serial, ESP_RST);

char inputChar[50];
//String ssid = "Nha Song";
//String pwd = "nhasongty";
//String ssid = "admin";
//String pwd = "dathaymatkhau";
String ssid = "ABCDE";
String pwd = "khongcopass";
//String ssid = "binh";
//String pwd = "123456789";
String serverip = "192.168.1.91";
String port = "8080";
String cmd;
int ch_id, packet_len;
char *pb;

void(* resetFunc) (void) = 0;                          //declare reset function at address 0

void setup() {
  //RTC init
//  Wire.begin();
//  RTC.begin();
  //Led status init
  pinMode(relay[0], OUTPUT);
  pinMode(relay[1], OUTPUT);
  pinMode(relay[2], OUTPUT);
  pinMode(relay[3], OUTPUT);
  pinMode(relay[4], OUTPUT);
  pinMode(relay[5], OUTPUT);
  pinMode(relay[6], OUTPUT);
  pinMode(relay[7], OUTPUT);
  digitalWrite(relay[0], LOW);
  digitalWrite(relay[1], LOW);
  digitalWrite(relay[2], LOW);
  digitalWrite(relay[3], LOW);
  digitalWrite(relay[4], LOW);
  digitalWrite(relay[5], LOW);
  digitalWrite(relay[6], LOW);
  digitalWrite(relay[7], LOW);
  //monitor and AT command serial init
  monitor.begin(115200);           // Serial monitor
  espSerial.begin(115200);             // ESP8266
  monitor.println(F("http://www.nguyenthaibinh.xyz"));     

  // Hard reset
//  Serial.print(F("Hard reset..."));
//  result = server.hardReset();
//  delay(5000);
  
  // Soft reset
  Serial.print(F("Soft reset..."));  
  do
  {    
    result = server.softReset();
    delay(4000);
  }
  while(!result);   
  // Quit current access point
  Serial.print(F("Quit access point..."));
  do
  {    
    result = server.closeAP();
    delay(500);
  }
  while(!result);

CONNECT_WIFI:

  cmd = "AT+CWJAP=" + String("\"") + ssid + String("\"") + String(",") + String("\"") + pwd + String("\"");
  espSerial.println(cmd);                    // Connect AP
  monitor.print("\n" + cmd);
  delay(4000);
  result = CheckResponse();                         // Check "OK"
  if (result == OK) {
    Serial.print(" OK");
    delay(300);
  }
  else {
    Serial.print(" ERROR");
    delay(1000);
    goto CONNECT_WIFI;
  }
    
//  // Connecting to AP
//  Serial.print(F("Connecting to AP..."));  
//  do
//  {    
//    result = server.connectToAP(F(ESP_SSID), F(ESP_PASS));
//    delay(5000);
//  }
//  while(!result);    
    
  // Setting specific IP
  Serial.print(F("Setting IP..."));
  do
  {
    result = server.setIP(F(SERVERIP));
    delay(500);
  }
  while(!result);
  
  // Setting multiple mode
  Serial.print(F("Setting multiple mode..."));
  do
  {
    result = server.setMultiple(1);
    delay(500);
  }
  while(!result);

  // Starting a server
  Serial.print(F("Starting a server..."));  
  do
  {
    result = server.startServer(F(PORT));
    delay(1000);
  }
  while(!result);

  // Setting multiple connect
//  Serial.print(F("Setting multiple connect..."));  
//  do
//  {
//    result = server.setMultipleConnect();
//    delay(5000);
//  }
//  while(!result);  
  
  for (unsigned char i = 0; i < 2; i++) {
    espSerial.println("AT+CIPCLOSE");                // Close All Socket
    monitor.print("\nAT+CIPCLOSE");
    delay(1000);
    result = CheckResponse();                         // Check "OK"
    if (result == OK) {
      Serial.print(" OK");
      delay(200);
    }
    else {
      Serial.print(" ERROR");
      delay(200);
      //goto START_SERVER;
    }
  }  
}

void loop() {  
  delay(100);
//  setAlarmFromTo(RTC, alarmHour, alarmMinute, alarmStatus);
  espSerial.readBytesUntil('\n', buffer, BUFFER_SIZE);
  if (strncmp(buffer, "+IPD,", 5) == 0) {
    //monitor.print("\nBuffer");
    // request: +IPD,ch,len:data
    sscanf(buffer + 5, "%d,%d", &ch_id, &packet_len);
    if (packet_len > 0) {
      //monitor.print("\nlen > 0");
      // read serial until packet_len character received
      // start from :
      pb = buffer + 5;
      while (*pb != ':') pb++;
      pb++;

      if (strncmp(pb, "GET /?updt", 10) == 0) {
        monitor.println(String(pb));
        delay(100);                
        cmd = "{\"stt1\":\"" + String(relayStatus[0]) + "\",\"stt2\":\"" + String(relayStatus[1]) + "\",\"stt3\":\"" + String(relayStatus[2]) + "\",\"stt4\":\"" + String(relayStatus[3]) + "\",}";
        anything(cmd, ch_id);
        clearSerialBuffer();
      }
      
      if (strncmp(pb, "GET /?onl", 9) == 0) {
        monitor.println(String(pb));
        delay(100);        
        digitalWrite(relay[String(String(pb).charAt(9)).toInt()], HIGH);
        relayStatus[String(String(pb).charAt(9)).toInt()] = 1;
        cmd = "{\"stt\":\"1\"}";
        anything(cmd, ch_id);
        clearSerialBuffer();
      }

      if (strncmp(pb, "GET /?ofl", 9) == 0) {
        monitor.println(String(pb));
        delay(100);        
        digitalWrite(relay[String(String(pb).charAt(9)).toInt()], LOW);
        relayStatus[String(String(pb).charAt(9)).toInt()] = 0;
        cmd = "{\"stt\":\"0\"}";
        anything(cmd, ch_id);
        clearSerialBuffer();
      }

      if (strncmp(pb, "GET /?alon", 10) == 0) {
        delay(100);        
        String link(pb);
        monitor.println(link.substring(10, link.indexOf("h")));
        monitor.println(link.substring(link.indexOf("h") + 1, link.indexOf("m")));
        if ((link.substring(10, link.indexOf("h")).toInt() < 23) && (link.substring(10, link.indexOf("h")).toInt() >= 0) && (link.substring(link.indexOf("h") + 1, link.indexOf("m")).toInt() < 60) && (link.substring(link.indexOf("h") + 1, link.indexOf("m")).toInt() >= 0)) {
          alarmHour[link.substring(link.indexOf("d") + 1).toInt() - 1] = link.substring(10, link.indexOf("h")).toInt();
          alarmMinute[link.substring(link.indexOf("d") + 1).toInt() - 1] = link.substring(link.indexOf("h") + 1, link.indexOf("m")).toInt();
          homepage(Content, ch_id, relayStatus, alarmHour, alarmMinute, alarmStatus);
        }
        clearSerialBuffer();
      }

      if (strncmp(pb, "GET /?alof", 10) == 0) {
        delay(100);
        String link(pb);
        monitor.println(link.substring(10, link.indexOf("h")));
        monitor.println(link.substring(link.indexOf("h") + 1, link.indexOf("m")));
        if ((link.substring(10, link.indexOf("h")).toInt() < 24) && (link.substring(10, link.indexOf("h")).toInt() >= 0) && (link.substring(link.indexOf("h") + 1, link.indexOf("m")).toInt() < 60) && (link.substring(link.indexOf("h") + 1, link.indexOf("m")).toInt() >= 0)) {
          alarmHour[link.substring(link.indexOf("d") + 1).toInt() + 3] = link.substring(10, link.indexOf("h")).toInt();
          alarmMinute[link.substring(link.indexOf("d") + 1).toInt() + 3] = link.substring(link.indexOf("h") + 1, link.indexOf("m")).toInt();
          homepage(Content, ch_id, relayStatus, alarmHour, alarmMinute, alarmStatus);
        }
        clearSerialBuffer();
      }

      if (strncmp(pb, "GET /?onal", 10) == 0) {
        delay(100);
        String link(pb);
        alarmStatus[link.substring(link.indexOf("l") + 1).toInt()] = 1;
        cmd = "1";
        anything(cmd, ch_id);
        clearSerialBuffer();
      }

      if (strncmp(pb, "GET /?ofal", 10) == 0) {
        delay(100);
        String link(pb);
        alarmStatus[link.substring(link.indexOf("l") + 1).toInt()] = 0;
        cmd = "0";
        anything(cmd, ch_id);
        clearSerialBuffer();
      }

      if (strncmp(pb, "GET /home", 9) == 0) {
        delay(100);
        homepage(Content, ch_id, relayStatus, alarmHour, alarmMinute, alarmStatus);
        clearSerialBuffer();
      }
    }
  }
  //monitor.print(buffer);
  clearBuffer();
  monitor.print( "\nLoop ");
}
void homepage(const char* Content, int ch_id, char* relayStatus, int* alarmHour, int* alarmMinute, int* alarmStatus) {
  String Header;

  Header =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n";
  int Content_len = strlen_P(Content);
  String Content2;
  Content2 =
    "<tr>"
    "<td>Thiết bị</td>";
  for (int i = 0; i < 4; i++) {
    if (relayStatus[i] == 1) {
      Content2 += "<td><button class=\"light\" data-id=\""; Content2 += i; Content2 += "\" data-status = \"1\" style=\"background-color:red\">Đèn "; Content2 += i + 1; Content2 += "</button></td>";
    }
    else {
      Content2 += "<td><button class=\"light\" data-id=\""; Content2 += i; Content2 += "\" data-status = \"0\" style=\"background-color:buttonface\">Đèn "; Content2 += i + 1; Content2 += "</button></td>";
    }
  }
  Content2 += "</tr>""<tr>""<td>Giờ bật</td>";
  char str[] = "0";
  for (int i = 0; i < 4; i++) {
    if (alarmStatus[i] == 1) {
      Content2 += "<td>"; Content2 += "<button class=\"alarm\" data-id=\""; Content2 += i; Content2 += "\" data-status = \"1\" style=\"background-color:red;width:100%\">";   Content2 += alarmHour[i]; Content2 += ":"; Content2 += alarmMinute[i]; Content2 += "</button>"; Content2 += "</td>";
    }
    else {
      Content2 += "<td>"; Content2 += "<button class=\"alarm\" data-id=\""; Content2 += i; Content2 += "\" data-status = \"0\" style=\"width:100%\">";   Content2 += alarmHour[i]; Content2 += ":"; Content2 += alarmMinute[i]; Content2 += "</button>"; Content2 += "</td>";
    }
  }
  Content2 += "</tr><tr><td>Giờ tắt</td>";
  for (int i = 4; i < 8; i++) {
    if (alarmStatus[i] == 1) {
      Content2 += "<td>"; Content2 += "<button class=\"alarm\" data-id=\""; Content2 += i; Content2 += "\" data-status = \"1\" style=\"background-color:red;width:100%\">";   Content2 += alarmHour[i]; Content2 += ":"; Content2 += alarmMinute[i]; Content2 += "</button>"; Content2 += "</td>";
    }
    else {
      Content2 += "<td>"; Content2 += "<button class=\"alarm\" data-id=\""; Content2 += i; Content2 += "\" data-status = \"0\" style=\"width:100%\">";   Content2 += alarmHour[i]; Content2 += ":"; Content2 += alarmMinute[i]; Content2 += "</button>"; Content2 += "</td>";
    }
  }
  Content2 += "</tr></table></center>";
  //Content2 += "<hr><p>Bởi<a href=\"http://nguyenthaibinh.xyz\"></p><p>Nguyễn Thái Bình</p></a>";
  Content2 += "</center></body></html>";

  Header += "Content-Length: ";
  Header += String(Content_len + (int)(Content2.length()));
  Header += "\r\n\r\n";

  SendString(Header, ch_id);
    delay(3000);
  SendChar(Content, ch_id);
    delay(8000);
  SendString(Content2, ch_id);
  delay(8000);
}

void anything(String cmd, int ch_id) {
  SendString(cmd, ch_id);
  delay(300);
}


void SendString(String StringToSend, int IPD_CH) {
  espSerial.println("AT+CIPSEND=" + String(IPD_CH) + "," + String(StringToSend.length()));
  monitor.println("AT+CIPSEND=" + String(IPD_CH) + "," + String(StringToSend.length()));
  if (espSerial.find(">")) {
    monitor.println(">");
    delay(100);
    espSerial.println(StringToSend);
    //    monitor.println(StringToSend);
  }
}

void SendChar(const char* MessageToSend, int IPD_CH) {
  int Message_len = strlen_P(MessageToSend);
  espSerial.println("AT+CIPSEND=" + String(IPD_CH) + "," + String(Message_len));
  monitor.println("AT+CIPSEND=" + String(IPD_CH) + "," + String(Message_len));
  delay(100);
  if (CheckContinueSend()){
    for (int k = 0; k < (Message_len - 1); k++){
      espSerial.print((char)(pgm_read_byte_near(MessageToSend + k)));
//    monitor.print((char)(pgm_read_byte_near(MessageToSend + k)));
    }
    espSerial.println((char)(pgm_read_byte_near(MessageToSend + (Message_len - 1))));
//  monitor.println((char)(pgm_read_byte_near(MessageToSend + (Message_len - 1))));     
  }  
}

void clearSerialBuffer(void) {
  while ( espSerial.available() > 0 ) {
    espSerial.read();
  }
}
void clearBuffer(void) {
  for (int i = 0; i < BUFFER_SIZE; i++ ) {
    buffer[i] = 0;
  }
}
int readSerial() {
  int i = 0;
  while (1) {
    while (espSerial.available() > 0) {
      char inChar = espSerial.read();

      if (i > 0) {
        if (inChar == '\n') {
          inputChar[i] = '\0';
          espSerial.flush();
          return 0;
          //break;
        }
      }
      if (inChar != '\r') {
        inputChar[i] = inChar;
        i++;
      }
    }
  }
}

boolean CheckResponse() {
  if (espSerial.find("K")) {     // Check "OK"
    return OK;
  }
  else {
    return TIMEOUT;
  }
}

boolean CheckContinueSend() {
  if (espSerial.find(">")) {     // Check "OK"
    return OK;
  }
  else {
    return TIMEOUT;
  }
}

//void setAlarmFromTo(RTC_DS1307 RTC, int* alarmHour, int* alarmMinute, int* alarmStatus) {
//  for (int i = 0; i < 4; i++) {
//    if (alarmStatus[i] == 1) {
//      if (alarmHour[i] == RTC.now().hour()) {
//        if (alarmMinute[i] == RTC.now().minute()) {
//          if (relayStatus[i] == 0) {
//            digitalWrite(relay[i], HIGH);
//            relayStatus[i] = 1;
//          }
//        }
//      }
//    }
//  }
//  for (int i = 4; i < 8; i++) {
//    if (alarmStatus[i] == 1) {
//      if (alarmHour[i] == RTC.now().hour()) {
//        if (alarmMinute[i] == RTC.now().minute()) {
//          if (relayStatus[i - 4] == 1) {
//            digitalWrite(relay[i - 4], LOW);
//            relayStatus[i - 4] = 0;
//          }
//        }
//      }
//    }
//  }
//}
