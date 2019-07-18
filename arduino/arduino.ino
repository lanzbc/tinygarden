#include<DHT.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <SerialCommand.h>  
const byte RX = 3;          
const byte TX = 2;         
 
SoftwareSerial mySerial = SoftwareSerial(RX, TX); 
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command
 
int mayBom = 4, den = 5; 
int doAmSensor = 6;
int autoTuoi = 9;
int autoDen=10;
int doAmAnalog = A0;
int camBienAnhSang= A1;
const int DHTPIN = 8;

const int DHTTYPE = DHT11;
DHT dht(DHTPIN, DHTTYPE);
//Cứ sau 5000ms = 5s thì chu kỳ lặp lại
const unsigned long CHU_KY_1_LA_BAO_NHIEU = 5000UL; 
 
void setup() {
  //Khởi tạo Serial ở baudrate 57600 để debug ở serial monitor
  Serial.begin(57600);
  //Khởi tạo Serial ở baudrate 57600 cho cổng Serial thứ hai kết nối với ESP8266
  mySerial.begin(57600);
  
  pinMode(mayBom,OUTPUT);
  pinMode(den,OUTPUT);
  pinMode(autoTuoi,OUTPUT);
  pinMode(autoDen,OUTPUT);
   
  pinMode(doAmSensor,INPUT);
  pinMode(doAmAnalog,INPUT);
  dht.begin();
  // Một số hàm trong thư viện Serial Command 
  sCmd.addCommand("DELAY",   delay); // lenh, ham
  sCmd.addCommand("doAm",  doAm_detect);
  sCmd.addCommand("DHT11",  dht11);
  Serial.println("Da san sang nhan lenh");
}
 
unsigned long chuky1 = 0;
void loop() {
  //Khởi tạo một chu kỳ lệnh, chu kỳ là 5000ms
  if (millis() - chuky1 > CHU_KY_1_LA_BAO_NHIEU) {
    chuky1 = millis();
    doAm_detect();
    dht11();
    if(digitalRead(doAmSensor)==1){
      digitalWrite(autoTuoi,1);
    } else{
      digitalWrite(autoTuoi,0);
    }
    int value= analogRead(camBienAnhSang); 
    float A1=(5/(value*0.004887585533));
    float Rldr1 = 10/(A1 - 1);
    float anhSang = 500/Rldr1;
    if(anhSang<200){
      digitalWrite(autoDen,1);
    } else digitalWrite(autoDen,0);    
  }
  sCmd.readSerial();
}
// ham delay duoc thuc thi khi gui lenh DELAY
void delay() {
  Serial.println("DELAY");
  char *json = sCmd.next(); //đọc tham số nhận được
  Serial.println(json);
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);//đặt một biến root mang kiểu json
 
  int mayBomStatus = root["delay"][0];
  int denStatus = root["delay"][1];

  //kiểm tra giá trị
  Serial.print(F("mayBomStatus "));
  Serial.println(mayBomStatus);
  Serial.print(F("denStatus "));
  Serial.println(denStatus);
 
  StaticJsonBuffer<200> jsonBuffer2;
  JsonObject& root2 = jsonBuffer2.createObject();
  root2["mayBomStatus"] = mayBomStatus;
  root2["denStatus"] = denStatus;
 
  //Tạo một mảng trong JSON
  JsonArray& data = root2.createNestedArray("data");
  data.add(mayBomStatus); 
  data.add(denStatus);

  //in ra cổng software serial để ESP8266 nhận
  mySerial.print("DELAY_STATUS");   //gửi tên lệnh
  mySerial.print('\r');           // gửi \r
  root2.printTo(mySerial);        //gửi chuỗi JSON
  mySerial.print('\r');           // gửi \r
  //in ra Serial để debug
  root2.printTo(Serial);
  
  digitalWrite(mayBom, mayBomStatus);
  digitalWrite(den, denStatus);
}
 
void doAm_detect() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  //đọc giá trị cảm biến rồi in ra root
  root["digital"] = digitalRead(doAmSensor);
  root["message"] = digitalRead(doAmSensor) ? "Cần tưới nước!" : "Đủ nước" ;
  int value = analogRead(doAmAnalog);  
  int a = map(value, 0, 1023, 0, 100);
  int percent = 100-a;
  root["analog"] = percent;
  //in ra cổng software serial để ESP8266 nhận
  mySerial.print("doAm");   //gửi tên lệnh
  mySerial.print('\r');           // gửi \r
  root.printTo(mySerial);        //gửi chuỗi JSON
  mySerial.print('\r'); 
}
void dht11() {
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  float hum = dht.readHumidity();    //Đọc độ ẩm
  float tem = dht.readTemperature(); //Đọc nhiệt độ
  root["hum"] = hum;
  root["tem"] = tem; 
  int value= analogRead(camBienAnhSang); 
  float A1=(5/(value*0.004887585533));
  float Rldr1 = 10/(A1 - 1);
  float anhSang = 500/Rldr1;
  root["camBienAnhSang"] = anhSang;
  
  mySerial.print("DHT11");   //gửi tên lệnh
  mySerial.print('\r');           // gửi \rS
  root.printTo(mySerial);        //gửi chuỗi JSON
  mySerial.print('\r'); 
}
