// IoT Water Level Monitoring Using Nodemcu ESP8266 and TOF10120 Laser Distance Sensor
//download libraries
//https://www.electroniclinic.com/arduino-libraries-download-and-projects-they-are-used-in-project-codes/

#include<stdlib.h>



#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int i;
int buttonState = HIGH; 
int out_of_range=1;
 


// for TOF10120 Laser Rangefinder Sensor

unsigned char ok_flag;
unsigned char fail_flag;
 
unsigned short lenth_val = 0;
unsigned char i2c_rx_buf[16];
unsigned char dirsend_flag=0;
 
int x_mm; // distance in millimeters
int Sensor_actual_value; 
int Sensor_prev_value; 
int delta, delta_prev;
float y_inches; // distance in inches

int secs; 
int minutes;
int hours;

int prev_value,prev_prev_value,new_value;

void setup(){

  Serial.begin(115200);
  Wire.begin();

 // pinMode(2, INPUT);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(200);
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Hello!");
  display.display(); 
  
  x_mm = 20;
  prev_value = x_mm;
  delta=0;
}
void loop(){

TOF10120();
//delay(200);
    
}



void TOF10120()
{
  
   //Sensor_actual_value = x_mm;
  // x_mm = map(x_mm, 0,600,100,0);
  // Serial.print("Original value: ");
  // Serial.print(Sensor_actual_value);
  // Serial.print("   mapped value: ");
   

    if (x_mm > 0){
      delta_prev = delta;
      delta = x_mm - prev_value;
     /*
      Serial.print("x_mm=");
      Serial.print(x_mm);
      Serial.print(",delta=");
      Serial.print(delta);
     */ 
      if ((delta <= 10 && delta >0)||(delta<0 && delta >= -10)) {delta = 0;}
    
    /* 
 
      Serial.print(" i=");
      Serial.print(i);
      Serial.print(",prev_value=");
      Serial.print(prev_value);
      Serial.print(",prev_delta=");
      Serial.print(delta_prev);
      Serial.print(",delta=");
      Serial.print(delta);
    */
    
      
      if (delta_prev >0 && delta < 0) {
        Serial.print("Noise...");
        }
  
      else{ 
         if ( prev_value >= 500 ){ 
           out_of_range = 1; 
           }
        
          if ( prev_value > 50 && out_of_range == 1 &&  prev_value < 500){ 
             i++;
           //Serial.print(i);
            out_of_range=0;
          }
      }
      Serial.print("x_mm=");
      Serial.print(x_mm);
      Serial.print(" i=");
      Serial.println(i);
      
      
      display.setCursor(0, 10);
    //  display.print(prev_value);
      display.print("Units:");
      display.println(i);
      
      display.display(); 
      delay(200);
      display.clearDisplay();
    }
   prev_value = x_mm;
   x_mm = ReadDistance();

   int j =0;
    
   while ((x_mm == 0 || x_mm > 2500)&& j<10){
       x_mm = ReadDistance(); 
       j++;
   }
}


void SensorRead(unsigned char addr,unsigned char* datbuf,unsigned char cnt) 
{
  unsigned short result=0;
  // step 1: instruct sensor to read echoes
  Wire.beginTransmission(82); // transmit to device #82 (0x52), you can also find this address using the i2c_scanner code, which is available on electroniclinic.com
  // the address specified in the datasheet is 164 (0xa4)
  // but i2c adressing uses the high 7 bits so it's 82
  Wire.write(byte(addr));      // sets distance data address (addr)
  Wire.endTransmission();      // stop transmitting
  // step 2: wait for readings to happen
  delay(1);                   // datasheet suggests at least 30uS
  // step 3: request reading from sensor
  unsigned char  xx=82;
  Wire.requestFrom(xx, cnt);    // request cnt bytes from slave device #82 (0x52)
  // step 5: receive reading from sensor
  if (cnt <= Wire.available()) { // if two bytes were received
    *datbuf++ = Wire.read();  // receive high byte (overwrites previous reading)
    *datbuf++ = Wire.read(); // receive low byte as lower 8 bits
  }
}
 
int ReadDistance(){
    SensorRead(0x00,i2c_rx_buf,2);
    lenth_val=i2c_rx_buf[0];
    lenth_val=lenth_val<<8;
    lenth_val|=i2c_rx_buf[1];
    delay(300); 
    return lenth_val;
}
