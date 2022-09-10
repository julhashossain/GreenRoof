#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#endif

#include <OneWire.h>
#include <DallasTemperature.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "Wire.h"

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);


const char* ssid     = "Roof_Rach";
const char* password = "Arch@1858";

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://api.shwo.com.bd/post-data.php";

String apiKeyValue = "tPmAT5Ab3j7F9";

#define DS3231_I2C_ADDRESS 0x68

#define SD_CS 5

#define ONE_WIRE_BUS_1 16
#define ONE_WIRE_BUS_2 17

#define soilSensor1 39
#define soilSensor2 32
#define soilSensor3 33
#define soilSensor4 34
#define soilSensor5 35

int sensorValue1  = 0 ;
int sensorValue2  = 0 ;
int sensorValue3  = 0 ;
int sensorValue4  = 0 ;
int sensorValue5  = 0 ;

String Temp[11] = {", T1: ", ", T2: ", ", T3: ", ", T4: ", ", T5: ", ", T6: ", ", T7: ", ", T8: ", ", T9: ", ", T10: ", ", T11: "};
String dataMessage;
String TiMe;
String DaTe;

float T_1, T_2, T_3, T_4, T_5, T_6, T_7, T_8, T_9, T_10, T_11;

float Temp_1[11] = {}; float Temp_2[11] = {}; float Temp_3[11] = {}; float Temp_4[11] = {}; float Temp_5[11] = {}; float Temp_6[11] = {};
float Temp_7[11] = {}; float Temp_8[11] = {}; float Temp_9[11] = {}; float Temp_10[11] = {}; float Temp_11[11] = {};

int i = 0; int s = 0; int m = 0; int h = 0; int t;
int percentValue1 = 0 ; int percentValue2 = 0 ; int percentValue3 = 0 ; int percentValue4 = 0 ; int percentValue5 = 0 ;

float T1_Total = 0; float T2_Total = 0; float T3_Total = 0; float T4_Total = 0; float T5_Total = 0; float T6_Total = 0;
float T7_Total = 0; float T8_Total = 0; float T9_Total = 0; float T10_Total = 0; float T11_Total = 0;

String T1_str, T2_str, T3_str, T4_str, T5_str, T6_str, T7_str, T8_str, T9_str, T10_str, T11_str;
String S1_str, S2_str, S3_str, S4_str, S5_str;

OneWire oneWire_in(ONE_WIRE_BUS_1);
OneWire oneWire_out(ONE_WIRE_BUS_2);

DallasTemperature sensor_1(&oneWire_in);
DallasTemperature sensor_2(&oneWire_out);

void setup(void)
{
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  Wire.begin();

  pinMode(soilSensor1, INPUT);
  pinMode(soilSensor2, INPUT);
  pinMode(soilSensor3, INPUT);
  pinMode(soilSensor4, INPUT);
  pinMode(soilSensor5, INPUT);

  sensor_1.begin();
  sensor_2.begin();

  // wifi configuration
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // set the initial time here:
  // DS3231 hours, minutes, seconds
  // setDS3231time(11, 7, 0);
  // DS3231 day, date, month, year
  // setDS3231date(7, 16, 4, 2022);

  // Initialize SD card
  SD.begin(SD_CS);
  if (!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }
  File file = SD.open("/data-log.txt");
  if (!file) {
    Serial.println("File doens't exist");
  }
  else {
    Serial.println("File already exists");
  }
  file.close();
}

void loop(void)
{

if (WiFi.status() != WL_CONNECTED) {
    delay(500);
    WiFi.reconnect();
  }

  else {

  
  if (t > 10) t = 0;
  else
    t = t + 1;
  displayTime();
  ReadTemperature();
  readSoilsensor();
  LCD();
  Serial.print("i = ");
  Serial.println(i);
  if (s % 29 == 0 || s % 30 == 0 || s % 31 == 0 || s % 59 == 0)
  {
    Temp_1[i]  = T_1;
    Temp_2[i]  = T_2;
    Temp_3[i]  = T_3;
    Temp_4[i]  = T_4;
    Temp_5[i]  = T_5;
    Temp_6[i]  = T_6;
    Temp_7[i]  = T_7;
    Temp_8[i]  = T_8;
    Temp_9[i]  = T_9;
    Temp_10[i] = T_10;
    Temp_11[i] = T_11;

    i = i + 1;
    delay(3000);

    if (i == 10)
    {
      for (i = 0; i < 10; i++)
      {
        T1_Total  = T1_Total + Temp_1[i];
        T2_Total  = T2_Total + Temp_2[i];
        T3_Total  = T3_Total + Temp_3[i];
        T4_Total  = T4_Total + Temp_4[i];
        T5_Total  = T5_Total + Temp_5[i];
        T6_Total  = T6_Total + Temp_6[i];
        T7_Total  = T7_Total + Temp_7[i];
        T8_Total  = T8_Total + Temp_8[i];
        T9_Total  = T9_Total + Temp_9[i];
        T10_Total = T10_Total + Temp_10[i];
        T11_Total = T11_Total + Temp_11[i];
      }

      T1_str  = String(T1_Total / 10, 2);
      T2_str  = String(T2_Total / 10, 2);
      T3_str  = String(T3_Total / 10, 2);
      T4_str  = String(T4_Total / 10, 2);
      T5_str  = String(T5_Total / 10, 2);
      T6_str  = String(T6_Total / 10, 2);
      T7_str  = String(T7_Total / 10, 2);
      T8_str  = String(T8_Total / 10, 2);
      T9_str  = String(T9_Total / 10, 2);
      T10_str = String(T10_Total / 10, 2);
      T11_str = String(T11_Total / 10, 2);
      i = 0;
    }
  }

  if (m % 5 == 0 && (s >= 15 && s <= 17))
  {
    appendFile(SD, "/data-log.txt", "Time: ");
    appendFile(SD, "/data-log.txt", TiMe.c_str());
    appendFile(SD, "/data-log.txt", ",  Date: ");
    appendFile(SD, "/data-log.txt", DaTe.c_str());
    appendFile(SD, "/data-log.txt", ", T1:");
    appendFile2(SD, "/data-log.txt", T1_str);
    appendFile(SD, "/data-log.txt", ", T2:");
    appendFile2(SD, "/data-log.txt", T2_str);
    appendFile(SD, "/data-log.txt", ", T3:");
    appendFile2(SD, "/data-log.txt", T3_str);
    appendFile(SD, "/data-log.txt", ", T4:");
    appendFile2(SD, "/data-log.txt", T4_str);
    appendFile(SD, "/data-log.txt", ", T5:");
    appendFile2(SD, "/data-log.txt", T5_str);
    appendFile(SD, "/data-log.txt", ", T6:");
    appendFile2(SD, "/data-log.txt", T6_str);
    appendFile(SD, "/data-log.txt", ", T7:");
    appendFile2(SD, "/data-log.txt", T7_str);
    appendFile(SD, "/data-log.txt", ", T8:");
    appendFile2(SD, "/data-log.txt", T8_str);
    appendFile(SD, "/data-log.txt", ", T9:");
    appendFile2(SD, "/data-log.txt", T9_str);
    appendFile(SD, "/data-log.txt", ", T10:");
    appendFile2(SD, "/data-log.txt", T10_str);
    appendFile(SD, "/data-log.txt", ", T11:");
    appendFile2(SD, "/data-log.txt", T11_str);

    appendFile(SD, "/data-log.txt", ", S1:");
    appendFile2(SD, "/data-log.txt", S1_str);
    appendFile(SD, "/data-log.txt", ", S2:");
    appendFile2(SD, "/data-log.txt", S2_str);
    appendFile(SD, "/data-log.txt", ", S3:");
    appendFile2(SD, "/data-log.txt", S3_str);
    appendFile(SD, "/data-log.txt", ", S4:");
    appendFile2(SD, "/data-log.txt", S4_str);
    appendFile(SD, "/data-log.txt", ", S5:");
    appendFile2(SD, "/data-log.txt", S5_str);

    appendFile(SD, "/data-log.txt", "\r\n");
    // send data to the webserver
    sendData();

    T1_Total  = 0;    T2_Total  = 0;
    T3_Total  = 0;    T4_Total  = 0;
    T5_Total  = 0;    T6_Total  = 0;
    T7_Total  = 0;    T8_Total  = 0;
    T9_Total  = 0;    T10_Total = 0;
    T11_Total = 0;    i = 0;
    Serial.println("Data Updated !!!");
    delay(3000);
  }
  Serial.println();
}

}
void sendData() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&T1=" + T1_str  + "&T2=" + T2_str + "&T3=" + T3_str  + "&T4="  + T4_str  + "&T5=" + T5_str
                             + "&T6=" + T6_str + "&T7=" + T7_str + "&T8=" + T8_str + "&T9=" + T9_str + "&T10=" + T10_str + "&T11=" + T11_str
                             + "&S1=" + S1_str + "&S2=" + S2_str + "&S3=" + S3_str + "&S4=" + S4_str + "&S5="  + S5_str  + "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    // You can comment the httpRequestData variable above
    // then, use the httpRequestData variable below (for testing purposes without the BME280 sensor)
    //String httpRequestData = "api_key=tPmAT5Ab3j7F9&value1=24.75&value2=49.54&value3=1005.14";

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain");
    //int httpResponseCode = http.POST("Hello, World!");

    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json");
    //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}");

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  //Send an HTTP POST request every 30 seconds
  //delay(30000);
}

void LCD()
{
  if (t <= 5) {
    lcd.setCursor(0,  0); lcd.print("T1:"); lcd.print(T_1, 1); lcd.write(223); lcd.print("C");
    lcd.setCursor(10, 0); lcd.print("T2:"); lcd.print(T_2, 1); lcd.write(223); lcd.print("C");
    lcd.setCursor(0,  1); lcd.print("T3:"); lcd.print(T_3, 1); lcd.write(223); lcd.print("C");
    lcd.setCursor(10, 1); lcd.print("T4:"); lcd.print(T_4, 1); lcd.write(223); lcd.print("C");
    lcd.setCursor(0,  2); lcd.print("T5:"); lcd.print(T_5, 1); lcd.write(223); lcd.print("C");
    lcd.setCursor(10, 2); lcd.print("T6:"); lcd.print(T_6, 1); lcd.write(223); lcd.print("C");
    lcd.setCursor(0,  3); lcd.print("T7:"); lcd.print(T_7, 1); lcd.write(223); lcd.print("C");
    lcd.setCursor(10, 3); lcd.print("T8:"); lcd.print(T_8, 1); lcd.write(223); lcd.print("C");
    delay(300);
    if (t == 5) lcd.clear();
  }
  else if (t > 5 && t <= 10) {
    lcd.setCursor(0,  0);  lcd.print("T9 :"); lcd.print(T_9);     lcd.write(223); lcd.print("C");
    lcd.setCursor(12, 0);  lcd.print("S1:");  lcd.print(S1_str);  lcd.print("% ");
    lcd.setCursor(0,  1);  lcd.print("T10:"); lcd.print(T_10);    lcd.write(223); lcd.print("C");
    lcd.setCursor(12, 1);  lcd.print("S2:");  lcd.print(S2_str);  lcd.print("% ");
    lcd.setCursor(0,  2);  lcd.print("T11:"); lcd.print(T_11);    lcd.write(223); lcd.print("C");
    lcd.setCursor(12, 2);  lcd.print("S3:");  lcd.print(S3_str);  lcd.print("% ");
    lcd.setCursor(0,  3);  lcd.print("S4 :");  lcd.print(S4_str);  lcd.print("% ");
    lcd.setCursor(12, 3);  lcd.print("S5:");  lcd.print(S5_str);  lcd.print("% ");
    delay(300); if (t == 10) lcd.clear();
  }
}

void ReadTemperature()
{
  sensor_1.requestTemperatures();
  sensor_2.requestTemperatures();

  T_1 = sensor_1.getTempCByIndex(0);
  T_2 = sensor_1.getTempCByIndex(1);
  T_3 = sensor_1.getTempCByIndex(2);
  T_4 = sensor_1.getTempCByIndex(3);
  T_5 = sensor_1.getTempCByIndex(4);
  T_6 = sensor_1.getTempCByIndex(5);
  T_7 = sensor_1.getTempCByIndex(6);

  T_8  = sensor_2.getTempCByIndex(0);
  T_9  = sensor_2.getTempCByIndex(1);
  T_10 = sensor_2.getTempCByIndex(2);
  T_11 = sensor_2.getTempCByIndex(3);
}

void readSoilsensor()
{
  sensorValue1  = analogRead(soilSensor1);
  percentValue1 = map(sensorValue1, 4095, 0, 0, 99);
  sensorValue2  = analogRead(soilSensor2);
  percentValue2 = map(sensorValue2, 4095, 0, 0, 99);
  sensorValue3  = analogRead(soilSensor3);
  percentValue3 = map(sensorValue3, 4095, 0, 0, 99);
  sensorValue4  = analogRead(soilSensor4);
  percentValue4 = map(sensorValue4, 4095, 0, 0, 99);
  sensorValue5  = analogRead(soilSensor5);
  percentValue5 = map(sensorValue5, 4095, 0, 0, 99);

  S1_str = String(percentValue1);
  S2_str = String(percentValue2);
  S3_str = String(percentValue3);
  S4_str = String(percentValue4);
  S5_str = String(percentValue5);
}

byte decToBcd(byte val) {
  return ( (val / 10 * 16) + (val % 10) );
}

byte bcdToDec(byte val) {
  return ( (val / 16 * 10) + (val % 16) );
}

void setDS3231time(byte hour, byte minute, byte second)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.endTransmission();
}

void setDS3231date(byte dayOfWeek, byte  dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(3); // set next input to start at the seconds register
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}

void displayTime() {
  byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;

  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);

  TiMe = String(hour, DEC) + ":" + String(minute, DEC) + ":" + String(second, DEC);
  DaTe = String(dayOfMonth, DEC) + "/" + String (month, DEC) + "/" + String (year, DEC);

  s = second, DEC;
  m = minute, DEC;
  h = hour, DEC;

  Serial.print(TiMe);
  Serial.print("\t");
  Serial.println(DaTe);
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  //  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    //    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void appendFile2(fs::FS &fs, const char * path, String message) {
  // Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    //   Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
