#include <EEPROM.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ENCODER_DT 2
#define ENCODER_CLK 3
#define ENCODER_SW 4

volatile int16_t encoderValue = 0;
volatile int16_t lastEncoded = 0;
bool buttonState = LOW;
bool lastButtonState = LOW;
long lastDebounceTime = 0; 

const int buttonPin = 2;  
int buttonStateS = 0;     
int lastButtonStateS = 0; 
int startPressed = 0;    
int endPressed = 0;      
int holdTime = 0;        
int idleTime = 0;  
int reading = 0; 

int rd = 0;
int sP = 0;
int eP = 0;
bool isHolded = false;

bool holdStart = false;
bool resetHold = false;

long chooseDelay = 2;
long lastChooseDelay = 0;
int pressButton = 0;
int chooseValue = 0;
int lastchooseValue = 0;

int massive = 0;
int nwV = 0;
int nmv;

int StartValue[3] = {120, 100, 50};
int newValue[3] = {1, 1, 1};
int dropValue[3] = {0, 0, 0};
int pos[4] = {0, 8, 16, 24};

bool but_flag = false;
bool butt;
bool butS_flag = false;
bool buttS;
bool frst = true;

int lessbbs = 0;
int displaybbs;
int outbbs;

#define OLED_RESET -1
Adafruit_SSD1306 display(128, 32, &Wire, OLED_RESET);

void updateEncoder() {
  int MSB = digitalRead(ENCODER_DT);
  int LSB = digitalRead(ENCODER_CLK);
  int encoded = (MSB << 1) | LSB;
  int sum = (lastEncoded << 2) | encoded;
  
  if(pressButton == 1)
    {
    if(encoded != 0)
    {
      lastChooseDelay = millis();
    }
    if((millis() - lastChooseDelay) > (chooseDelay / 2))
    {
      if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
        if(nmv < 250){
          holdedChoose();
          if(isHolded == true)
          {
            encoderValue = encoderValue + 5;
          }
          if(isHolded == false)
          {
            encoderValue = encoderValue + 1;
          }
        }
      }
        
       else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
        if(nmv > 0)
        {
          holdedChoose();
          if(isHolded == true)
          {
            encoderValue = encoderValue - 5;
          }
          if(isHolded == false)
          {
            encoderValue = encoderValue - 1;
          }
        }
        
      }
    }
  }
  if(pressButton == 0)
  {
    if(encoded != 0)
    {
      lastChooseDelay = millis();
    }
    if((millis() - lastChooseDelay) > (chooseDelay / 2))
    {
      if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
          chooseValue = chooseValue + 8;
          massive++;
      } else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
          chooseValue = chooseValue - 8;
          massive--;
      }
    }
    
  }
  lastEncoded = encoded;

}

void setup() {
  Wire.begin();
  Wire.setClock(400000);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, pos[1]);
  display.println("HELLO");
  display.display();

  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  
  pinMode(ENCODER_DT, INPUT);
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_SW, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(ENCODER_DT), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), updateEncoder, CHANGE);

  newValue[0] = EEPROM.read(0);
  newValue[1] = EEPROM.read(1);
  newValue[2] = EEPROM.read(2);

  delay(200);
  Menu();
}

void loop() {

  reading = digitalRead(ENCODER_SW);
  if (reading != lastButtonState) {
    updateState();
  }

  lastButtonState = reading;
  if(frst == true)
  {
    pressButton = 0;
    frst = false;
  }
  if(pressButton == 1){

    if(massive == 3)
    {
      if(resetHold == false)
      {
      Save();
      Done();
      Menu();
      }
      if(resetHold == true)
      {
        Reset();
        Menu();
      }
      
    }

     if(massive <= 2)
      {
      if (newValue[massive] != encoderValue) 
        {
          nwV = newValue[massive];
          nmv = nwV + encoderValue;
          display.setTextSize(1);
          display.setTextColor(SSD1306_BLACK);
          display.setCursor(70, pos[massive]);
          display.println(dropValue[massive]);

          for(int i = 0; i <  4;i++)
            {
              display.setCursor(0, pos[i]);
              display.println(">");
              display.setCursor(100, pos[i]);
              display.println("<");
            }

          display.setTextColor(SSD1306_WHITE);
          display.setCursor(70, pos[massive]);
          display.println(nmv);
          display.setCursor(100, chooseValue);
          display.println("<");
          display.display();
          dropValue[massive] = nmv;
      }
    }
  }

  if(pressButton >= 2)
  {
    newValue[massive] = nmv;
    pressButton = 0;
    encoderValue = 0;
    if(isHolded == true)
    {
      isHolded = false;
    }
  }

  

  if(pressButton == 0)
  {
    ChooseValue();
  }

  butt = !digitalRead(5);
  buttS = !digitalRead(6);
  
  if(butt == true && but_flag == false)
  {
        if(buttS == true && butS_flag == false)
        {
          bbs();
          butS_flag = true;
          if(outbbs <= 0)
          {
            but_flag = true;
            display.clearDisplay();
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(25, pos[1]);
            display.println("DONE");
            display.display();
          }
          
        }
        if(buttS == false && butS_flag == true)
        {
          butS_flag = false;
        }
  }

  if(butt == false && but_flag == false && outbbs > 0)
  {
    outbbs = 0;
    lessbbs = 0;
    Menu();
  }

    if(butt == false && but_flag == true)
  {
    but_flag = false;
    lessbbs = 0;
    Menu();
  }

}
void ChooseValue()
{ 
  if(chooseValue < 25)
  {
    for(int i = 0; i < 4;i++)
    {
      display.setTextSize(1);
      display.setTextColor(SSD1306_BLACK);
      display.setCursor(0, pos[i]);
      display.println(">");
      display.setCursor(100, pos[i]);
      display.println("<");
    }

    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, chooseValue);
    display.println(">");
    display.display();
    lastchooseValue = chooseValue;
    
  }
  if(chooseValue > 24){
    chooseValue = 0;
    massive = 0;
  }
  if(chooseValue < 0)
  {
    chooseValue = 24;
    massive = 3;
  }
}

void Menu()
{
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("  Preset 1: ");
  display.println("  Preset 2: ");
  display.println("  Preset 3: ");
  display.println("  Save Presets ");
  display.setCursor(70, pos[0]);
  display.println(newValue[0]);
  display.setCursor(70, pos[1]);
  display.println(newValue[1]);
  display.setCursor(70, pos[2]);
  display.println(newValue[2]);
  display.display();
}

void bbs()
{
  lessbbs = lessbbs + 1;
  displaybbs = newValue[massive];
  outbbs = displaybbs - lessbbs;
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(30, pos[1]);
  display.setTextColor(SSD1306_WHITE);
  display.print(outbbs);
  display.display();
  if(outbbs <= 0 )
  {
    delay(50);
  }
}

void Done()
{
  massive = 0;
  pressButton = 0;
  chooseValue = 0;
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, pos[1]);
  display.println("DONE");
  display.display();
  delay(200);
}
void Reset()
{
  for(int i = 0; i < 3; i++)
  {
    newValue[i] = StartValue[i];
  }
  resetHold = false;
  massive = 0;
  pressButton = 0;
  chooseValue = 0;
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, pos[0]);
  display.println("RESET");
  display.setCursor(25, pos[2]);
  display.println("DONE");
  display.display();
  delay(200);
}
void updateState() {
  if (reading == LOW) {
      startPressed = millis();
      
  } else {
      endPressed = millis();
      if(holdStart == false)
      {
        startPressed = startPressed + 70;
      }
      holdTime = endPressed - startPressed;
      
      if (holdTime >= 5 && holdTime < 400 && isHolded == false) {
          pressButton = pressButton + 1;
      }

      if (holdTime >= 400) {
          pressButton = pressButton + 1;
          if(massive == 3)
          {
          resetHold = true;
          }
      }
      holdStart = true;
      startPressed = 0;
      endPressed = 0;
      holdTime = 0;

  }
}

void holdedChoose() {
  rd = digitalRead(ENCODER_SW);
  if (rd == LOW && massive != 3 && pressButton == 1) {
    isHolded = true;
  }
  if(rd != LOW)
  {
    isHolded = false;
  }
}

void Save(){
  int dVF = dropValue[0];
  EEPROM.update(0, dVF);
  delay(5);
  int dVS = dropValue[1];
  EEPROM.update(2, dVS);
  delay(5);
  int dVT = dropValue[2];
  EEPROM.update(4, dVT);
  delay(5);
}