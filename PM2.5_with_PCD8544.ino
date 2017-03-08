#include <Arduino.h>
#include "U8glib.h" // you need download this U8glib first
#include <dht11.h>

#define DHT11PIN 3
#define backlight_pin  12 //lcd backlight pin but not use

#define LENG 31   //0x42 + 31 bytes equal to 32 bytes
unsigned char buf[LENG];

int PM01Value = 0;        //define PM1.0 value of the air detector module
int PM2_5Value = 0;       //define PM2.5 value of the air detector module
int PM10Value = 0;       //define PM10 value of the air detector module
dht11 DHT11;

U8GLIB_PCD8544 u8g(13, 11, 10, 9, 8);  // CLK=8, DIN=4, CE=7, DC=5, RST=6 ,you can change it form U8glib


void draw(void) {
  u8g.setFont(u8g_font_profont11);  // select font
  u8g.drawStr(0, 8, "T: ");  // put string of display at position X, Y
  u8g.setPrintPos(15, 8);  // set position
  u8g.print(DHT11.temperature, 1);  // display temperature from DHT11
  u8g.drawStr(27, 8, "°C"); // can't display degree ,but maybe change font can display it
  u8g.drawStr(47, 8, "H: ");
  u8g.setPrintPos(60, 8);
  u8g.print(DHT11.humidity, 1);  // display humidity from DHT11
  u8g.drawStr(74, 8, "%");
  u8g.drawStr(0, 20, "PM0.1: ");
  u8g.setPrintPos(40, 20);
  u8g.print(PM01Value);   // display PM01
  u8g.drawStr(55, 20, "ug/m3");
  u8g.drawStr(0, 32, "PM2.5: ");
  u8g.setPrintPos(40, 32);
  u8g.print(PM2_5Value);   // display PM2.5
  u8g.drawStr(55, 32, "ug/m3");
  u8g.drawStr(0, 44, "PM10: ");
  u8g.setPrintPos(40, 44);
  u8g.print(PM10Value);   // display PM10
  u8g.drawStr(55, 44, "ug/m3");
}

void setup(void) {
  analogWrite(backlight_pin, 50);  /* Set the Backlight intensity */
  Serial.begin(9600);   //use serial0, if you like,you can use softserial 
  Serial.setTimeout(1500);    //set the Timeout to 1500ms, longer than the data transmission periodic time of the sensor
}

void loop(void) {
  if (Serial.find(0x42)) {  //start to read when detect 0x42
    Serial.readBytes(buf, LENG);
    if (buf[0] == 0x4d) {
      if (checkValue(buf, LENG)) {
        PM2_5Value = transmitPM2_5(buf); //count PM2.5 value of the air detector module
        PM10Value = transmitPM10(buf); //count PM10 value of the air detector module
        PM01Value = transmitPM01(buf); //count PM0.1 value of the air detector module
      }
    }
  }
  static unsigned long OledTimer = millis();
  if (millis() - OledTimer >= 1000)
  {
    OledTimer = millis();
    Serial.print("PM0.1: ");
    Serial.print(PM01Value);
    Serial.println("  ug/m3");
    Serial.print("PM2.5: ");
    Serial.print(PM2_5Value);
    Serial.println("  ug/m3");
    Serial.print("PM10: ");
    Serial.print(PM10Value);
    Serial.println("  ug/m3");
    Serial.print("T: ");
    Serial.print(DHT11.temperature, 1);
    Serial.println("c");
  }  
  DHT11.read(DHT11PIN);  // Read apin on DHT11

  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );

  delay(5000);  // Delay of 5sec before accessing DHT11 (min - 2sec)
}
char checkValue(unsigned char *thebuf, char leng)
{
  char receiveflag = 0;
  int receiveSum = 0;

  for (int i = 0; i < (leng - 2); i++) {
    receiveSum = receiveSum + thebuf[i];
  }
  receiveSum = receiveSum + 0x42;

  if (receiveSum == ((thebuf[leng - 2] << 8) + thebuf[leng - 1])) //check the serial data
  {
    receiveSum = 0;
    receiveflag = 1;
  }
  return receiveflag;
}

int transmitPM01(unsigned char *thebuf)
{
  int PM01Val;
  PM01Val = ((thebuf[3] << 8) + thebuf[4]); //count PM1.0 value of the air detector module
  return PM01Val;
}

int transmitPM2_5(unsigned char *thebuf)
{
  int PM2_5Val;
  PM2_5Val = ((thebuf[5] << 8) + thebuf[6]); //count PM2.5 value of the air detector module
  return PM2_5Val;
}

int transmitPM10(unsigned char *thebuf)
{
  int PM10Val;
  PM10Val = ((thebuf[7] << 8) + thebuf[8]); //count PM10 value of the air detector module
  return PM10Val;
}

double Fahrenheit(double celsius)
{
  return 1.8 * celsius + 32;
}    //摄氏温度度转化为华氏温度

double Kelvin(double celsius)
{
  return celsius + 273.15;
}     //摄氏温度转化为开氏温度

// 露点（点在此温度时，空气饱和并产生露珠）
// 参考: http://wahiduddin.net/calc/density_algorithms.htm
double dewPoint(double celsius, double humidity)
{
  double A0 = 373.15 / (273.15 + celsius);
  double SUM = -7.90298 * (A0 - 1);
  SUM += 5.02808 * log10(A0);
  SUM += -1.3816e-7 * (pow(10, (11.344 * (1 - 1 / A0))) - 1) ;
  SUM += 8.1328e-3 * (pow(10, (-3.49149 * (A0 - 1))) - 1) ;
  SUM += log10(1013.246);
  double VP = pow(10, SUM - 3) * humidity;
  double T = log(VP / 0.61078); // temp var
  return (241.88 * T) / (17.558 - T);
}

// 快速计算露点，速度是5倍dewPoint()
// 参考: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity / 100);
  double Td = (b * temp) / (a - temp);
  return Td;
}
