#include <LiquidCrystal_I2C.h>
#include <DS3231.h>
#include <Wire.h>
#include <ESP32_Servo.h>
#include <SPI.h>
#include <LoRa.h>

#define RELAY_PIN 25
#define SERVO_PIN 26
#define echoPin 13
#define trigPin 12
#define LED_PIN_WORKING 17
#define LED_PIN_FOOD 16
//define the pins used by the transceiver module
#define ss 15
#define rst 0
#define dio0 27

int statusPakan = 2;
int counter = 0;
float phi = 3.14;
int massaJenis = 800;      // 0.8g/cm^3 = 800kg/m^3
float satuLontaran = 0.1;  // 100g = 0.1kg
float jariJari = 0.15;      // 15cm = 0.15m

long durasi, jarak, persentasePakan, volume, massaTerisi, jumlahLontaran, jarakA;
LiquidCrystal_I2C lcd(0x27, 16, 2);

DS3231 myRTC;
Servo myservo;

void setup() {
  Wire.begin();
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);  //Set variabel trigPin sebagai output
  pinMode(echoPin, INPUT);   //Set variabel echoPin sebagai input
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN_WORKING, OUTPUT);
  pinMode(LED_PIN_FOOD, OUTPUT);

  myservo.attach(SERVO_PIN);
  myservo.write(0);                    // buka servo
  myservo.write(110);                  // tutup servo
  digitalWrite(RELAY_PIN, HIGH);       // mematikan relay
  digitalWrite(LED_PIN_WORKING, LOW);  // mematikan LED status bekerja
  digitalWrite(LED_PIN_FOOD, LOW);     // mematikan LED status pakan

  lcd.begin(16, 2);

  while (!Serial)
    ;
  Serial.println("LoRa Sender");
  LoRa.setPins(ss, rst, dio0);  // set pin LoRa
  SPI.begin(14, 12, 13, 15);    // inisiasi pin SPI

  while (!LoRa.begin(915E6))  // setting frekuensi
  {
    Serial.println(".");
    delay(500);
  }

  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  bool h12Flag;
  bool pmFlag;

  digitalWrite(trigPin, LOW);
  delayMicroseconds(1000);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1000);

  durasi = pulseIn(echoPin, HIGH);  // Echo menerima sinyal ultrasonik
  jarak = (durasi / 2) / 29.1;      // Rumus mengubah durasi menjadi jarak (cm)

  Serial.print("T.PAKAN: ");
  Serial.print(jarak);
  Serial.println(" cm");

  // ////////////////////////----------KODE MUFID----------////////////////////////////

  // MENCARI PERSENTASE PAKAN
  float jarakA = 0.2;
  int persentasePakan = (static_cast<float>(jarakA) / 0.4) * 100;
  Serial.println(persentasePakan);

  // MENCARI JUMLAH LONTARAN
  float volume = phi * pow(jariJari, 2) * jarakA;
  float massaTerisi = volume * massaJenis;
  int jumlahLontaran = massaTerisi / satuLontaran;

  //Variable yang dikirim = persentasePakan, jumlahLontaran;
  String packet = String(persentasePakan) + "," + String(jumlahLontaran) + "," + String(jumlahLontaran);  // Menggabungkan tiga string dengan karakter pemisah koma

  Serial.print("Sending packet: ");// print sending packet
  Serial.println(counter); // menunjukan data keberapa yang dikirim

  //Kirim Data ke lora receiver
  LoRa.beginPacket();
  LoRa.print(packet);
  Serial.print(packet);
  LoRa.endPacket();

  counter++;
  delay(2000);  // Mengirim data setiap 5 detik

  ////////////////////////----------KODE MUFID----------////////////////////////////


  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T.PAKAN:");
  lcd.setCursor(9, 0); // Pindahkan kursor ke baris pertama, kolom kesembilan
  lcd.print(jarak);    // Cetak nilai jarak
  lcd.print(" cm");    // Cetak satuan (centimeter)

  // ... (kode yang ada)

  // LCD status pakan
  if (jarak >= 0 && jarak <= 15) {
    lcd.setCursor(0, 1);
    lcd.print("PAKAN BANYAK");
    Serial.print("PAKAN BANYAK  ");
    delay(500);
    statusPakan = 2;
  } else if (jarak > 15 && jarak <= 35) {
    lcd.setCursor(0, 1);
    lcd.print("TAMBAH PAKAN");
    Serial.print("TAMBAH PAKAN  ");
    delay(500);
    statusPakan = 1;
  } else {
    lcd.setCursor(0, 1);
    lcd.print("PAKAN HABIS");
    Serial.print("PAKAN HABIS  ");
    statusPakan = 0;
  }

  // LED status pakan
  if (statusPakan == 2) {
    digitalWrite(LED_PIN_FOOD, LOW);
  } else if (statusPakan == 1) {
    digitalWrite(LED_PIN_FOOD, HIGH);
  } else {
    for (int i = 0; i <= 2; i++) {
      digitalWrite(LED_PIN_FOOD, LOW);
      delay(100);
      digitalWrite(LED_PIN_FOOD, HIGH);
      delay(100);
    }
  }


  Mengubah servo dari 0 derajat ke 110 derajat dan menyalakan relay (format waktu 0 0 0)
  if ((myRTC.getHour(h12Flag, pmFlag)) == 8 && (myRTC.getMinute()) == 30 && (myRTC.getSecond()) == 0) { // time 1
    mainProgram();
  } else if ((myRTC.getHour(h12Flag, pmFlag)) == 8 && (myRTC.getMinute()) == 32 && (myRTC.getSecond()) == 0) { // time 2
    mainProgram();
  } else if ((myRTC.getHour(h12Flag, pmFlag)) == 8 && (myRTC.getMinute()) == 34 && (myRTC.getSecond()) == 0) { // time 3
    mainProgram();
  } else if ((myRTC.getHour(h12Flag, pmFlag)) ==  8 && (myRTC.getMinute()) == 36 && (myRTC.getSecond()) == 0) { // time 4
    mainProgram();
  } else if ((myRTC.getHour(h12Flag, pmFlag)) == 8 && (myRTC.getMinute()) == 38 && (myRTC.getSecond()) == 0) { // time 5
    mainProgram();
  } else if ((myRTC.getHour(h12Flag, pmFlag)) == 8 && (myRTC.getMinute()) == 40 && (myRTC.getSecond()) == 0) { // time 6
    mainProgram();
  }else if ((myRTC.getHour(h12Flag, pmFlag)) == 8 && (myRTC.getMinute()) == 42 && (myRTC.getSecond()) == 0) { // time 7
    mainProgram();
  }else if ((myRTC.getHour(h12Flag, pmFlag)) == 8 && (myRTC.getMinute()) == 44 && (myRTC.getSecond()) == 0) { // time 8
    mainProgram();
  }else if ((myRTC.getHour(h12Flag, pmFlag)) == 8 && (myRTC.getMinute()) == 46 && (myRTC.getSecond()) == 0) { // time 9
    mainProgram();
  }else if ((myRTC.getHour(h12Flag, pmFlag)) == 8 && (myRTC.getMinute()) == 48 && (myRTC.getSecond()) == 0) { // time 10
    mainProgram();
  }
}

void mainProgram() {
  digitalWrite(LED_PIN_WORKING, HIGH); // LED ststus bekerja menyala
  myservo.write(0); // servo membuka
  delay(1000); //delay servo menyala atau delay antara servo nyala kemudian relay nyala
  digitalWrite(RELAY_PIN, LOW); // relay menyala
  delay(5000); //delay relay menyala (banyaknya waktu pakan keluar)
  myservo.write(110); // servo menutup
  delay(1000); // delay sebelum relay mati atau delay anatara servo dan relay mati
  digitalWrite(RELAY_PIN, HIGH); // mematikan relay
  digitalWrite(LED_PIN_WORKING, LOW); // LED status bekerja mati
}