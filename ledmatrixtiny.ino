/* ATtiny85 as an I2C Master  Ex1          BroHogan                      1/21/11
 * I2C master reading DS1621 temperature sensor. (display with leds)
 * SETUP:
 * ATtiny Pin 1 = (RESET) N/U                      ATtiny Pin 2 = (D3) LED3
 * ATtiny Pin 3 = (D4) to LED1                     ATtiny Pin 4 = GND
 * ATtiny Pin 5 = SDA on DS1621                    ATtiny Pin 6 = (D1) to LED2
 * ATtiny Pin 7 = SCK on DS1621                    ATtiny Pin 8 = VCC (2.7-5.5V)
 * NOTE! - It's very important to use pullups on the SDA & SCL lines!
 * DS1621 wired per data sheet. This ex assumes A0-A2 are set LOW for an addeess of 0x48
 * TinyWireM USAGE & CREDITS: - see TinyWireM.h
 * NOTES:
 * The ATtiny85 + DS1621 draws 1.7mA @5V when leds are not on and not reading temp.
 * Using sleep mode, they draw .2 @5V @ idle - see http://brownsofa.org/blog/archives/261
 */

#include <TinyWireM.h>                  // I2C Master lib for ATTinys which use USI

#define MATRIX_ADDR   0x74             // 7 bit I2C address for DS1621 temperature sensor
#define LED1_PIN         4              // ATtiny Pin 3
#define LED2_PIN         1              // ATtiny Pin 6
#define LED3_PIN         3              // ATtiny Pin 2
#define COMMAND_REG   0xFD

#define ISSI_REG_CONFIG  0x00
#define ISSI_REG_CONFIG_PICTUREMODE 0x00
#define ISSI_REG_CONFIG_AUTOPLAYMODE 0x08
#define ISSI_REG_CONFIG_AUDIOPLAYMODE 0x18

#define ISSI_CONF_PICTUREMODE 0x00
#define ISSI_CONF_AUTOFRAMEMODE 0x04
#define ISSI_CONF_AUDIOMODE 0x08

#define ISSI_REG_PICTUREFRAME  0x01

#define ISSI_REG_SHUTDOWN 0x0A
#define ISSI_REG_AUDIOSYNC 0x06

#define ISSI_COMMANDREGISTER 0xFD
#define ISSI_BANK_FUNCTIONREG 0x0B  

static const uint8_t PROGMEM
  smile_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
  neutral_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10111101,
    B10000001,
    B01000010,
    B00111100 },
  frown_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B01000010,
    B00111100 },

  hawkr_bmp[] =
  { B00000000,
    B01111011,
    B00110001,
    B00110001,
    B00110001,
    B00110001,
    B00110001,
    B01111011 },

  hawkl_bmp[] =
  { B00000000,
    B11011110,
    B10011110,
    B10001100,
    B10001100,
    B10001100,
    B10001100,
    B11001100 };

//uint8_t sweep[] = {1, 2, 3, 4, 6, 8, 10, 15, 20, 30, 40, 60, 60, 40, 30, 20, 15, 10, 8, 6, 4, 3, 2, 1};


int tempC = 0;                          // holds temp in C
int tempF = 0;                          // holds temp in F


void setup(){
  pinMode(LED1_PIN,OUTPUT);
  pinMode(LED2_PIN,OUTPUT);
  pinMode(LED3_PIN,OUTPUT);
  
  if (! Init_Matrix()) {
//    Serial.println("IS31 not found");
    while (1);
  }  
  
  //delay(2000);// initialize I2C lib
                       // Setup DS1621

 for(int y = 9; y >= -2; y--)
 {
      for(int x = 0; x < 16; x++)
      {
        drawPixel(x,y+2,0,0);
        drawPixel(x,y+1,255,0);
        drawPixel(x,y,255,0); 
        //drawPixel(x,y+10,0,0);       
      }
      
      
      if(y % 2 == 0){
      for(int x = 0; x <= 16; x++)
      {
        drawPixel(x,y+2,255,0);
        drawPixel(x-1,y+2,0,0);
        
        delay(25);
      }
      }
      else
      {
      for(int x = 15; x >= -1; x--)
      {
        drawPixel(x,y+2,255,0);
        drawPixel(x+1,y+2,0,0);
        
        delay(25);
      }
      }
      
      delay(100);
      drawBitmap(0,y+2,hawkr_bmp, 8,8, 255);
      drawBitmap(8,y+2,hawkl_bmp, 8,8, 255);
        
      
     
      
 }
      for(int x = 0; x < 16; x++)
      {
        drawPixel(x,8,0,0);
      }
}


void loop(){

   // clearMatrix();    
}

void drawBitmap(uint8_t y, uint8_t x, const uint8_t bitmap[], uint8_t w, uint8_t h, uint16_t color)
{ 
uint8_t drawByte = 0;
  for(int posx = 0; posx < w; posx++)
  {
    drawByte = pgm_read_byte(&bitmap[posx]);
    
    for(int posy = 0; posy < h; posy++)
    {

      //Serial.print((drawByte & 0x80)/128);
      drawPixel(y + posy,x + posx, ((drawByte & 0x80)/128)*color,0);
      drawByte = drawByte << 1;
 
      
     //Serial.println(drawByte);
      //Serial.println("next step");
      //Serial.println((drawByte & 0x80));
    }

   // Serial.println();
  }
}

void clearMatrix(){
  
   for(int x = 0; x < 16; x++)
      for(int y = 0; y < 9; y++)
        drawPixel(x,y,0,0);  
}


boolean Init_Matrix(){ // Setup the DS1621 for one-shot mode
  
  TinyWireM.begin();
  //TinyWireM.setClock(100000);

  TinyWireM.beginTransmission(MATRIX_ADDR);

  if (TinyWireM.endTransmission() != 0)
    return false;
    
    // shutdown
  
  writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x00);

  delay(10);

  // out of shutdown
  writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_SHUTDOWN, 0x01);

  // picture mode
  writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_CONFIG, ISSI_REG_CONFIG_PICTUREMODE);

  displayFrame(0);

  clearMatrix();

  for (uint8_t f=0; f<8; f++) {
    for (uint8_t i=0; i<=0x11; i++)
      writeRegister8(f, i, 0xff);     // each 8 LEDs on
  }


  audioSync(false);
  clearMatrix();

  return true;
         // Send to the slave
}

void audioSync(boolean sync) {
  if (sync) {
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x1);
  } else {
    writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_AUDIOSYNC, 0x0);
  }
}



void drawPixel(int16_t x, int16_t y, uint16_t color, uint8_t bank){

  if ((x < 0) || (x >= 16)) return;
  if ((y < 0) || (y >= 9)) return;
  if (color > 255) color = 255; //max pwm
  setLEDPWM(x + y*16, color, bank);

}



void setLEDPWM(uint8_t lednum, uint8_t pwm, uint8_t bank){
  if (lednum >= 144) return;
  writeRegister8(bank, 0x24+lednum, pwm);
  /*Wire.beginTransmission(0x24+lednum);
  Wire.write(pwm);
  Wire.endTransmission();          // Send to the slave*/
  
}

void writeRegister8(uint8_t bank, uint8_t reg, uint8_t data){

  selectBank(bank);
  
  
  TinyWireM.beginTransmission(MATRIX_ADDR);
  TinyWireM.write((byte) reg);
  TinyWireM.write((byte) data);
  TinyWireM.endTransmission();
}

void selectBank(uint8_t bank){
  TinyWireM.beginTransmission(MATRIX_ADDR);
  TinyWireM.write((byte)COMMAND_REG);
  TinyWireM.write(bank);
  TinyWireM.endTransmission();
}

void displayFrame(uint8_t frame) {
  if (frame > 7) frame = 0;
  writeRegister8(ISSI_BANK_FUNCTIONREG, ISSI_REG_PICTUREFRAME, frame);
}
