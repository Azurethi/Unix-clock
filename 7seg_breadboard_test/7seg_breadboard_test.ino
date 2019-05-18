
//7 segment display pins (with 220 ohm resistor)
const int sseg_pins[] = {7,5,12,10,9,8,13,11};    //a=0,b=1,c=2,d=3,e=4,f=5,g=6,dp=7

const boolean sseg_encode[][8] = {
  {1,1,1,1,1,1,0,0}, //0
  {0,1,1,0,0,0,0,0}, //1
  {1,1,0,1,1,0,1,0}, //2
  {1,1,1,1,0,0,1,0}, //3
  {0,1,1,0,0,1,1,0}, //4
  {1,0,1,1,0,1,1,0}, //5
  {1,0,1,1,1,1,1,0}, //6
  {1,1,1,0,0,0,0,0}, //7
  {1,1,1,1,1,1,1,0}, //8
  {1,1,1,1,0,1,1,0}  //9
  };

//74HC595 shift register pins
const int pin_data = 2;
const int pin_rclk = 3; //ST_CP
const int pin_srclk = 4;//SH_CP

void setup() {
  //setup 7seg pins
  for(int i = 0; i<8; i++){
    pinMode(sseg_pins[i], OUTPUT);
    digitalWrite(sseg_pins[i],HIGH);
  }

  //setup shift reg. pins
  pinMode(pin_data, OUTPUT);
  pinMode(pin_rclk, OUTPUT);
  pinMode(pin_srclk, OUTPUT);

  //clear shift register  (Note, looping data would be an issue but the arduino pulls Q7' low
  digitalWrite(pin_rclk, LOW);  
  shiftOut(pin_data, pin_srclk, MSBFIRST, 0b00000001);
  digitalWrite(pin_rclk, HIGH);
  
  pinMode(pin_data, INPUT); //avoid interfering with looping bit

  nextDisplay();  //breadboard setup uses "display pins" 1,2,3 & 4, no display is on 0, so we skip it
}

int curDisp = 0;
unsigned long lastUpdate=0;
int number[] = {0,0,0,0,0,0,0,0};
void loop() {
  clearDisplay(); //avoid cross talk
  nextDisplay();
  writeDisplay(number[curDisp-1]);//display offset for same reason as extr nextDisplay above^^
  if(curDisp==8)curDisp=0;
  unsigned long curTime = millis();
  if(curTime-lastUpdate>=1000){
    lastUpdate = curTime;
    increment();
    analogRead()
  }
}

void increment(){
  number[0]++;
  for(int i = 0; i<7; i++){
    if(number[i]>9){
      number[i] = 0;
      number[i+1]++;  
    }else{
      break;
    }
  }
}

void clearDisplay(){
  for(int i = 0; i<8; i++){
    digitalWrite(sseg_pins[i],HIGH);
  }  
}

void writeDisplay(int num){   //REM, need to set other pins high if not calling clearDisplay() first!
  for(int i = 0; i<8; i++){
    if(sseg_encode[num][i]){
      digitalWrite(sseg_pins[i],LOW);
    }
  }  
}

void nextDisplay(){
    curDisp++;
  digitalWrite(pin_rclk, LOW);
  digitalWrite(pin_srclk, HIGH);
  digitalWrite(pin_srclk, LOW);
  digitalWrite(pin_rclk, HIGH);
}
