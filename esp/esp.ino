#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <SocketIOClient.h>
#include <SerialCommand.h>  
 
//include thư viện để kiểm tra free RAM trên con esp8266
extern "C" {
  #include "user_interface.h"
}
const byte Rx = 0;
const byte Tx = 2;
 
SoftwareSerial mySerial(Rx, Tx, false, 256); 
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command
 
SocketIOClient client;
//const char* ssid = "Sikbon";          
//const char* password = "bon140115@@";   
char host[] = "lanzbc.herokuapp.com";
int port =80;
//char host[] = "192.168.1.135";
//int port = 3484;

const char* ssid = "Lanzbc";          
const char* password = "meomeomeo"; 
//char host[] = "172.20.10.3";  
                 
char namespace_esp8266[] = "esp8266"; 
//từ khóa extern: dùng để #include các biến toàn cục ở một số thư viện khác. 
//Trong thư viện SocketIOClient có hai biến toàn cục mà chúng ta cần quan tâm đó là
// RID: Tên hàm (tên sự kiện
// Rfull: Danh sách biến (được đóng gói lại là chuối JSON)
extern String RID;
extern String Rfull;
 
void setup()
{
    //Bật baudrate ở mức 57600 để giao tiếp với máy tính qua Serial
    Serial.begin(57600);
    mySerial.begin(57600); //Bật software serial để giao tiếp với Arduino
    delay(10);
 
    Serial.print("Ket noi vao mang ");
    Serial.println(ssid);
    //Kết nối vào mạng Wifi
    WiFi.begin(ssid, password);
    //Chờ đến khi đã được kết nối
    while (WiFi.status() != WL_CONNECTED) { //Thoát ra khỏi vòng 
        delay(500);
        Serial.print('.');
    }
 
    Serial.println();
    Serial.println(F("Da ket noi WiFi"));
    Serial.println(F("Di chi IP cua ESP8266 (Socket Client ESP8266): "));
    Serial.println(WiFi.localIP());
 
    if (!client.connect(host, port, namespace_esp8266)) {
        Serial.println(F("Ket noi den socket server that bai!"));
        return;
    }
  //bắt tất cả các lệnh rồi chuyển xuống hàm defaultCommand
    sCmd.addDefaultHandler(defaultCommand);
    Serial.println("Da san sang nhan lenh");
}
 
void loop()
{
    //Khi bắt được bất kỳ sự kiện nào thì chúng ta có hai tham số:
    //  +RID: Tên sự kiện
    //  +RFull: Danh sách tham số được nén thành chuỗi JSON!
    if (client.monitor()) {
        //in ra serial cho Arduino
        mySerial.print(RID);
        mySerial.print('\r');
        mySerial.print(Rfull);
        mySerial.print('\r');
        
        //Kiểm tra xem còn dư bao nhiêu RAM, để debug
        uint32_t free = system_get_free_heap_size();
        Serial.println(free);
    }
    //Kết nối lại!
    if (!client.connected()) {
      client.reconnect(host, port, namespace_esp8266);
    }
 
    sCmd.readSerial();
}
 
void defaultCommand(String command) {
  char *json = sCmd.next();
  client.send(command, (String) json);//gửi dữ liệu về cho Socket Server
}
