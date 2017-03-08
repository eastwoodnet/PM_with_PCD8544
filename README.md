## Arduino PM Sensor with plantower PMS1003

### Device list

* 1 Arduino uno or any other clone board
* 2 Plantower PMS1003 or other Plantower PMS
* 3 PCD8544 screen aka. Nokia 5110 screen
* 4 DHT11 temperature and humidity sensor
* 5 Some Dupont Line

### Software

* 1 arduino IDE 
* 2 U8glib for arduino IDE 
* 3 DHT11 library

### How to link

PMS1003 pinset you can see

http://www.aqmd.gov/docs/default-source/aq-spec/resources-page/plantower-pms1003-manual_v2-5.pdf?sfvrsn=2

Just use PIN 1,2,5

If PMS1003 TX link to arduino pin D0, maybe you need disconnect this pin to upload the arduino program. Or you can use softwareserial.

Any other things, see the PM2.5_with_PCD8544.ino.

My DHT11 sensor is not very accurate, but it may also be an algorithmic accuracy problem, this code i just copy and paste. So if you have any temperature or humidity problem, I can't help.

