#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

// Chân servor
Servo servo1;
Servo servo2;
const int servo1pin = 3;
const int servo2pin = 11;

// Chân start
const int Start = 9;
// Chân reset
const int Reset = 10;
// Chế độ
bool Status = 0;

// Chân cảm biến màu sắc
// TCS230 or TCS3200 pins wiring to Arduino
#define S0 4
#define S1 5
#define S2 7
#define S3 6
#define sensorOut 8

// Stores frequency read by the photodiodes
int r = 0;
int g = 0;
int b = 0;
int color = 0;

// Đếm màu sắc
int spR = 0, spG = 0, spB = 0, tongsp = 0;
bool khoaDemR = 0, khoaDemG = 0, khoaDemB = 0;

// Giá trị quay của các servo
int gtquay1 = 0;

// Biến thời gian
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;
unsigned long currentMillis1 = 0;
unsigned long previousMillis1 = 0;
unsigned long timeDelay1;

void setup() {
  Serial.begin(9600);
  pinMode(Start, INPUT_PULLUP);      // start
  pinMode(Reset, INPUT_PULLUP);      // reset

  //Phím test cảm biến
//  pinMode(pinR, INPUT_PULLUP);      // màu đỏ
//  pinMode(pinG, INPUT_PULLUP);      // màu xanh
//  pinMode(pinY, INPUT_PULLUP);      // màu vàng

  // Cấu hình chân Servo
  servo1.attach(servo1pin);
  servo2.attach(servo2pin);

  // Cảm biến màu sắc
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  
  // Setting the sensorOut as an input
  pinMode(sensorOut, INPUT);
  
  // Setting frequency scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  // Hiển thị khởi đầu LCD
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("HE THONG DEM VA");
  lcd.setCursor(0,1);
  lcd.print("PHAN LOAI S.PHAM");
  //delay(2000);

  // Servo begin.
  servo1.write(180);
  servo2.write(90);
  
}

void loop() {
  CheDo();
  if(Status == 1) chuKiHD();
} //end


//------------------------------------------------------
// Servo 1 hoạt động
void chuKiHD(){
  unsigned long currentMillis1 = millis();
  ReadCB();
  LCDColor();
  delay(30);
  if(currentMillis1 - previousMillis1 >= 4000){
    previousMillis1 = currentMillis1;
    //ReadCB();
    servo1.write(90);
    //ReadCB();
    //hienThi();
    gtquay1 = gtquay1 - 90;
    if(gtquay1 < 0){
      gtquay1 = 180;
      khoaDemR = 0;
      khoaDemG = 0;
      khoaDemB = 0;
    }
  }
}

//-----------------------------------------------------
// Đọc cảm biến
void ReadCB(){
  readColor();
  if(color == 1){       // Đếm và phân loại màu đỏ
    servo2.write(45);
    if(khoaDemR == 0){
      spR++;
      khoaDemR = 1;
    }
    //Serial.println("do");
  }
  if(color == 3){       // Đếm và phân loại màu xanh
    servo2.write(90);
    if(khoaDemG == 0){
      spG++;
      khoaDemG = 1;
    }
    //Serial.println("xanh");
  }
  if(color == 2){       // Đếm và phân loại màu Blue
    servo2.write(135);
    if(khoaDemB == 0){
      spB++;
      khoaDemB = 1;
    }
    //Serial.println("vang");
  }
  //Serial.print(spR); Serial.print(" - "); Serial.print(spG); Serial.print(" - "); Serial.print(spY); Serial.print(" - "); Serial.println(tongsp);
}

//-----------------------------------------------------
// Đếm sản phẩm và in ra màn hình
void hienThi(){
  tongsp = spR + spG + spB;
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Tong SP:");
  lcd.setCursor(9,0); lcd.print(tongsp);

  // Đỏ
  lcd.setCursor(0,1); lcd.print("R:");
  lcd.setCursor(2,1); lcd.print(spR);

  // Xanh
  lcd.setCursor(5,1); lcd.print("G:");
  lcd.setCursor(7,1); lcd.print(spG);

  // Vàng
  lcd.setCursor(10,1); lcd.print("B:");
  lcd.setCursor(12,1); lcd.print(spB);
}

// Đọc nút bấm
void CheDo(){
  if(digitalRead(Start) == 0){
    delay(20);
    if(digitalRead(Start) == 1){
      Status = 1;
    }
  }
  if(digitalRead(Reset) == 0){
    delay(20);
    if(digitalRead(Reset) == 1){
      
      LCDreset();
      resetBoard();
    }
  }
}

//-----------------------------------------------------
// LCD hiển thị trước khi reser
void LCDreset(){
  lcd.clear();
  lcd.setCursor(5,0); 
  lcd.print("RESER");
  for(int i = 0; i<3; ++i){
    lcd.setCursor(5+i,1); 
    lcd.print(".");
    delay(500);
  }
}


//-----------------------------------------------------
// Hàm reset
void resetBoard(){
  asm volatile ("jmp 0"); // Nhảy về dòng đầu tiên
}


//-----------------------------------------------------
void readColor(){
  // Setting RED (R) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  r = pulseIn(sensorOut, LOW);
  // Printing the RED (R) value
  Serial.print("R = ");
  Serial.print(r);
  delay(50);
  
  // Setting GREEN (G) filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  g = pulseIn(sensorOut, LOW);
  // Printing the GREEN (G) value  
  Serial.print(" G = ");
  Serial.print(g);
  delay(50);
 
  // Setting BLUE (B) filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  b = pulseIn(sensorOut, LOW);
  // Printing the BLUE (B) value 
  Serial.print(" B = ");
  Serial.println(b);
  delay(50);

  //case
  if(r >= 80 && r <= 100 && g >= 130 && g <= 170 && b >= 90 && b <= 131){
    color = 1;      //Đỏ
  }
  else if(r >= 140 && r <= 180 && g >= 125 && g <= 150 && b >= 80 && b <= 110){
    color = 2;      // Blue
  }
  else if(r >= 130 && r <= 160 && g >= 100 && g <= 126 && b >= 105 && b <= 130){
    color = 3;      // Xanh
  }
  else color = 0;      // ko có
}



//-----------------------------------------------------
// hiển thị dải màu LCD
void LCDColor(){
  lcd.clear();
  // Đỏ
  lcd.setCursor(0,0); lcd.print("R:");
  lcd.setCursor(2,0); lcd.print(r);

  // Xanh
  lcd.setCursor(8,0); lcd.print("G:");
  lcd.setCursor(10,0); lcd.print(g);

  // Vàng
  lcd.setCursor(5,1); lcd.print("B:");
  lcd.setCursor(7,1); lcd.print(b);
}
