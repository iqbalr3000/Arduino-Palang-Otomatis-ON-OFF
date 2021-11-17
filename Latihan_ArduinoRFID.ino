//menggunakan library
#include<SPI.h>
#include<MFRC522.h>
#include<LiquidCrystal_I2C.h>
#include<Servo.h>

//variable mfrc522
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

//variable lcd
LiquidCrystal_I2C lcd(0x27, 16, 2);

//variable servo
int outServo = 6;
Servo myServo;

//variable infrared 
int infrared = 7;

//variable light
int R = 3;
int Y = 4;
int G = 5;

//variable buzzer
int buzzer = 2;

//variable button
int button = 8;

//variable tambahan
int kunci = 0;
int on = 0;
String uidString;
int saldo = 15000;
int incomingByte = 0;
int input = 0;

void setup() {
  //setup baudrate
  Serial.begin(9600);

  //initiate spi
  SPI.begin();

  //initiate mfrc522
  mfrc522.PCD_Init();

  //initiate lcd
  lcd.begin();

  //initiate servo
  myServo.attach(outServo);

  //setup pin mode input atau output
  pinMode(infrared, INPUT);
  pinMode(button, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(R, OUTPUT);
  pinMode(Y, OUTPUT);
  pinMode(G, OUTPUT);

  //kondisi awal lcd
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Mesin mati!");

  //kondisi awal servo
  myServo.write(90);

  //pilihan tambah saldo
  Serial.println("Pilih kondisi : ");
  Serial.println("1) Tambah saldo Rp. 5000");
  Serial.println("2) Tambah saldo Rp. 10000");
}

void loop() {
  int readButtonON = digitalRead(button);
  if(readButtonON == 1 && on == 0){
    on = 1;

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Mesin Nyala!");
    delay(1000);
    
    digitalWrite(R, HIGH);
    digitalWrite(Y, LOW);
    digitalWrite(G, LOW);

    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Tempelkan Kartu!");
  }else if(readButtonON == 1 && on == 1){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Mesin mati!");
    delay(1000);
    
    digitalWrite(R, LOW);
    digitalWrite(Y, LOW);
    digitalWrite(G, LOW);

    servoDown();
    on = 0;
    kunci = 0;
  }
  
  // baca data kartu
  if(mfrc522.PICC_IsNewCardPresent() && kunci == 0 && on == 1){
    readMFRC522();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Tag ID : ");
    lcd.setCursor(0,1);
    lcd.print(uidString);

    if(uidString == "8A-37-5A-1A"){
      if(saldo >= 5000){
        buzzerBerhasil();
        kunci = 1;
        saldo -= 5000;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("BERHASIL!");
        lcd.setCursor(0,1);
        lcd.print("Saldo : " + String(saldo));
        delay(1000);
        
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Palang dibuka!");
        servoUP();
        hijauON();
        
      }else{
        buzzerGagal();
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("GAGAL!");
        lcd.setCursor(0,1);
        lcd.print("Saldo kurang");
        delay(1000);
  
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Tempelkan Kartu!");
      }
    }else{
      buzzerGagal();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("GAGAL!");
      delay(1000);

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Tempelkan Kartu!");
    }
    
  }

  tambahSaldo();
  infraredON();

}

void readMFRC522(){
  mfrc522.PICC_ReadCardSerial();
  uidString = String(mfrc522.uid.uidByte[0], HEX) + "-" +
              String(mfrc522.uid.uidByte[1], HEX) + "-" +
              String(mfrc522.uid.uidByte[2], HEX) + "-" +
              String(mfrc522.uid.uidByte[3], HEX);
              uidString.toUpperCase();
}

void infraredON(){
  int readInfrared = digitalRead(infrared);

  if(readInfrared == 0 && kunci == 1 && on == 1){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Palang ditutup");
    delay(500);
    servoDown();
    merahON();
    kunci = 0;
    
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Tempelkan Kartu!");
  }
}

void buzzerBerhasil(){
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
  delay(100);
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);
}

void buzzerGagal(){
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
}

void merahON(){
  digitalWrite(R, HIGH);
  digitalWrite(Y, LOW);
  digitalWrite(G, LOW);
}

void kuningON(){
  digitalWrite(R, LOW);
  digitalWrite(Y, HIGH);
  digitalWrite(G, LOW);
}

void hijauON(){
  digitalWrite(R, LOW);
  digitalWrite(Y, LOW);
  digitalWrite(G, HIGH);
}

void servoUP(){
  myServo.write(180);
}

void servoDown(){
  myServo.write(90);
}

void tambahSaldo(){
  if(Serial.available() > 0){
    incomingByte = Serial.read();
    input = incomingByte - 48;
  
    switch(input){
      case 1:
        int tambah = 5000;
        saldo += tambah;
        Serial.println("Saldo : " + String(saldo));
        break;
    }
  
    delay(200);
    input = 0;
  }
}
