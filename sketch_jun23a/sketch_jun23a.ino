// Include for Server
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266mDNS.h>  //Thư viện mDNS
#include <WebSocketsServer.h>
WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer webServer(80);
BlynkTimer timer_update;
char* ssid = "Nguyen";
char* pass = "12345678"; 


//Include for DHT11
#include "DHT.h"
#define DHTPIN 2// what digital pin we're connected to // pin 4
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);


// Include for LCD
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);


// System Variables
const int Relay1 = 14;
const int Relay2 = 12;
const int Relay3 = 13;
const int Relay4 = 15;
const int Ledgr = 0;
const int Buzz = 16;

int i = 0;
int H1 = 20;
int H0 = 80;
int T1 = 25;
int T0 = 35;

int t;
int h;

 String alertT;
 String alertH;



const char MainPage[] PROGMEM = R"=====(
 <!DOCTYPE html> 
  <html>
   <head> 
       <title>HOME PAGE</title> 
       <style> 
          body {text-align:center;}
          h1 {color:#003399;}
          a {text-decoration: none;color:#FFFFFF;}
      
          .bt_1 {
            height:50px; 
            width:100px; 
            margin:10px 0;
            background-color:#f0fF9F;
            border-radius:5px;
          }
          .bt_2 {
            height:50px; 
            width:100px; 
            margin:10px 0;
            background-color:#00FF00;
            border-radius:5px;
          }
      .bt_3 {
            height:50px; 
            width:500px; 
            margin:10px 0;
            background-color:#00FF00;
            border-radius:5px;
          }
      .Tag{
      font-size: 25px;
      style: color:red;
      }
      .Header{
      font-size: 30px;
      style: color: black;
      background-color: pink;
      }
       </style>
       <meta name="viewport" content="width=device-width,user-scalable=0" charset="UTF-8">
   </head>
   <body onload="javascript:init()"> 
   
    <h1 class = 'Header'> DỰ ÁN GIÁM SÁT  </h1> 
    
    
    <div    <p class="Tag" > Thông số  </div>
  <div  <p class="Tag" > Nhiệt độ - Độ Ẩm  </div>
  <div  <p  </div>
  
  <div>
    <button class="bt_2"style="color:red" id = "Server_T" >T </button>
    <button class="bt_2"style="color:red" id = "Server_H" >H </button>

    </div>
  

    <div  <p class="Tag" > Điều Khiển Hệ Thống </div>
    <div  <p  </div>
  
    <div>
 <button id = "RL1_BT2" class="bt_1" >RƠ LE 1</button>
    <button id = "RL1_BT2" class="bt_1" onclick="sendData('On1')">ON </button>
    <button id = "RL1_BT3" class="bt_1" onclick="sendData('Off1')">OFF</button>
    </div>

    <div>
    <button id = "RL1_BT2" class="bt_1" >RƠ LE 2</button>
    <button id = "RL1_BT2" class="bt_1" onclick="sendData('On2')">ON </button>
    <button id = "RL1_BT3" class="bt_1" onclick="sendData('Off2')">OFF</button>
  </div>

    
   
<div    <p class="Tag" > Trạng Thái Hệ Thống </div>
  <div  <p  </div>
  <div>
    <button class="bt_2"style="color:red" id = "Server_AlertT" > Nop </button>
    <button class="bt_2"style="color:red" id = "Server_AlertH" > Nop </button>
    </div>

    <div>
  <button id = "RL2_BT1" class="bt_2" onclick="sendData('SendData')">Gửi Thông Số</button>
  </div>
      
      <script>
        function alertCS(){
            alert("Xác nhận button 1");
        }
  </script>
  
  <script>
  function alerthight(){
      alert(" Server Error");
      }
      </script>
      
    
      <script>
            var Socket;      //Khai báo biến Socket
            function init(){
              //Khởi tạo websocket
              Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
              //Nhận broadcase từ server
              Socket.onmessage = function(event){   
                
                
                var JSONobj = JSON.parse(event.data);   //Tách dữ liệu json
               
                document.getElementById("Server_H").innerHTML = JSONobj.D1;
                document.getElementById("Server_T").innerHTML = JSONobj.D2;
                document.getElementById("Server_AlertT").innerHTML = JSONobj.D3;
                document.getElementById("Server_AlertH").innerHTML = JSONobj.D4;
               
              }
            }
            //-----------Gửi dữ liệu lên Server-------------------
            function sendData(data){
              Socket.send(data);
            }
      </script>
   </body> 
  </html>
)=====";








void setup() {
  // LCD Init //
  lcd.init();
  lcd.backlight();
  lcd.print("Whats Up");
  lcd.setCursor(4,1);
  lcd.print("Bitch !!!");


  // Set up for GPIO 
  pinMode(Ledgr, OUTPUT);
  pinMode(Relay1,OUTPUT);
  pinMode(Relay2,OUTPUT);
  pinMode(Relay3,OUTPUT);
  pinMode(Relay4,OUTPUT);
  pinMode(Buzz, OUTPUT);
  digitalWrite(Ledgr,HIGH);
  digitalWrite(Relay1,LOW);
  digitalWrite(Relay2,LOW);
  
  
  

  
  //===============Thiết lập kết nối WiFi=================//
  //WiFi.begin(ssid,pass);
  Serial.begin(9600);
  //Serial.print("Connecting");
//  while(WiFi.status()!=WL_CONNECTED){
//    delay(500);
//    Serial.print("...");
//  }
//  Serial.println(WiFi.localIP());
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, pass);
  //======Khởi tạo mDNS cấp phát Tên miền E-SMART HOME====//
  if (!MDNS.begin("E-SMARTHOME")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  MDNS.addService("http","tcp",80);
  //=================Khởi tạo Web Server=================//   
 
  webServerStart();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  timer_update.setInterval(1000L, updateStateD);


  // Setup for Project 
  dht.begin();
  delay(1000);
  lcd.clear();

}
 
void loop() {
  MDNS.update();  //mDNS
  webServer.handleClient();
  webSocket.loop();

//  lcd.clear();
// LCD
lcd.setCursor(0,0);
lcd.print("Nhiet Do:");
lcd.setCursor(0,1);
lcd.print("Do Am:");

// Read DHT11
  h = dht.readHumidity();
  t = dht.readTemperature();

// upload to lcd

lcd.setCursor(10,0);
lcd.print(t);
lcd.setCursor(10,1);
lcd.print(h);

  

  Compareh( h);
  Comparet( t);

  
  String  D1 = convert(h);
  String  D2 = convert(t);
  String D3 = alertT ;
  String D4 = alertH ;


  
  String JSONtxt = "{\"D1\":\""+D1+"\",\"D2\":\""+D2+"\",\"D3\":\""+D3+"\",\"D4\":\""+D4+"\"}";
  webSocket.broadcastTXT(JSONtxt);
//  String RX = webSocketEvent();
//  Serial.print("Rx nay :");
//  Serial.println(Rx);
  
  delay(500);
}

//================== Convert Data for Comunicate ===================//
String convert(int val)
{
  int i;
  char buff[10];
  char valueString[100] = "";
  for (i = 0; i < 10; i++) {
    dtostrf(val, 4, 2, buff);  //4 is mininum width, 6 is precision
    strcat(valueString, buff);
    strcat(valueString, ", ");
  }
  return buff;
}


//================== Compare Data for Comunicate ===================//

void Compareh(int h)
{
  if( h> H0 && i == 0)
  {
    String Content = "Do am cao";
    digitalWrite(Ledgr, LOW);
    digitalWrite(Relay3, HIGH);
    digitalWrite(Buzz,HIGH);
    alertT = Content;
    i = 1;
    Sendsms(Content);
    
    
  }
  else  {
    if(h <H1 && i == 0){
        String Content = "Do am thap";
        digitalWrite(Ledgr, LOW);
        digitalWrite(Relay3, HIGH);
        digitalWrite(Buzz,HIGH);
        alertT = Content;
        i =1;
        Sendsms(Content);
    }
    else{
          digitalWrite(Ledgr, HIGH);
          digitalWrite(Relay3, LOW);
          digitalWrite(Buzz,LOW);
          alertT  = "NOP";
          i = 0;
    }
  }
}

void Comparet(int t)
{
  if( t>T0 && i ==  0)
  {
      String Content = "Nhiet do cao";
      digitalWrite(Ledgr, LOW);
      digitalWrite(Relay4, HIGH);
      digitalWrite(Buzz,HIGH);
      alertH = Content;
      i = 1;
      Sendsms(Content);
  }
  else{
    if( t< T1 && i == 0){
       String Content = "Nhiet do thap";
       digitalWrite(Ledgr, LOW);
       digitalWrite(Relay4, HIGH);
       digitalWrite(Buzz,HIGH);
       alertH = Content;
       i =1;
       Sendsms(Content);
    }
    else {
      digitalWrite(Ledgr, HIGH);
      digitalWrite(Relay4, LOW);
      digitalWrite(Buzz,LOW);
      i =0;
      alertH = "NOP";
      }
  }
}




//===================Chương trình con====================//
void webServerStart(){ 
  webServer.on("/",mainpage);
  webServer.begin();
}
void mainpage(){
  String s = FPSTR(MainPage);
  webServer.send(200,"text/html",s);
}

//=================== Command from Socker Processing ====================//
void webSocketEvent(uint8_t num, WStype_t type,
                    uint8_t * payload,
                    size_t length)
{
  String payloadString = (const char *)payload;
//  Serial.print("payloadString= ");
//  Serial.println(payloadString);
//  Rx = (const char*)payload;

//================== Relay Handler Functions ================================//
  if(payloadString == "On1")
  {
    digitalWrite(Relay1, HIGH);
  }
  if(payloadString == "Off1")
  {
    digitalWrite(Relay1, LOW);
  }

//==================================================// 
  if(payloadString == "On2")
  {
    digitalWrite(Relay2, HIGH);
  }
  if(payloadString == "Off2")
  {
    digitalWrite(Relay2, LOW);
  }
  
//==================================================//   
  if(payloadString == "On3")
  {
    digitalWrite(Relay3, HIGH);
  }
  if(payloadString == "Off3")
  {
    digitalWrite(Relay3, LOW);
  }

//==================================================//   
  if(payloadString == "On4")
  {
    digitalWrite(Relay4, HIGH);
  }
  if(payloadString == "Off4")
  {
    digitalWrite(Relay4, LOW);
  }

//==================================================// 

  if(payloadString == "SMS")
  { 
     String Content2 = "SMS";
    Sendsms(Content2);
  }
  if(payloadString == "SendData")
  {  
  String  D1 = convert(h);
  String  D2 = convert(t);
  String content3 = "{\"Nhietdo\":\""+D1+"\",\"Do am\":\""+D2+"\"}";
  Sendsms(content3);
  }
  
}

void Sendsms(String Content)
{ 
    Serial.print("AT");  //Start Configuring GSM Module
    delay(1000);         //One second delay
    Serial.println();
    Serial.println("AT+CMGF=1");  // Set GSM in text mode
    delay(1000);                  // One second delay
    Serial.println();
    Serial.print("AT+CMGS=");     // Enter the receiver number
 //   Serial.print("\"+84868696720\"");
 Serial.print("\"+84399174769\"");
    Serial.println();
    delay(1000);
    Serial.print(Content); // SMS body - Sms Text
    delay(1000);
    Serial.println();
    Serial.write(26);
}


ICACHE_RAM_ATTR void updateStateD(){
  String d1 = "";
  String d2 = "";
  
  String JSONtxt = "{\"D1\":\""+d1+"\",\"D2\":\""+d2+"\"}";
//  webSocket.broadcastTXT(JSONtxt);
//  Serial.println(JSONtxt);
}
