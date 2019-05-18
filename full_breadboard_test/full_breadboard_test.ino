
//7 segment display pins (with 220 ohm resistor)
const int sseg_pins[] = {13,9,6,7,5,12,8};                      //a=0,b=1,c=2,d=3,e=4,f=5,g=6 (dp not wired on full test)
const int sseg_map[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}; //will allow shift registers to be wired as convienent on pcb

const boolean sseg_encode[][8] = {
  {1,1,1,1,1,1,0}, //0
  {0,1,1,0,0,0,0}, //1
  {1,1,0,1,1,0,1}, //2
  {1,1,1,1,0,0,1}, //3
  {0,1,1,0,0,1,1}, //4
  {1,0,1,1,0,1,1}, //5
  {1,0,1,1,1,1,1}, //6
  {1,1,1,0,0,0,0}, //7
  {1,1,1,1,1,1,1}, //8
  {1,1,1,1,0,1,1}  //9
  };

//74HC595 shift register pins
const int pin_data = 2;
const int pin_rclk = 4; //ST_CP
const int pin_srclk = 3;//SH_CP

void setup() {
  //setup 7seg pins
  for(int i = 0; i<8; i++){
    pinMode(sseg_pins[i], OUTPUT);
    digitalWrite(sseg_pins[i],LOW);
  }
  
  //setup shift reg. pins
  pinMode(pin_data, OUTPUT);
  pinMode(pin_rclk, OUTPUT);
  pinMode(pin_srclk, OUTPUT);

  //Initialise shift registers such that only the last bit is low
  digitalWrite(pin_rclk, LOW);           
  shiftOut(pin_data, pin_srclk, MSBFIRST, 0xFF);
  shiftOut(pin_data, pin_srclk, MSBFIRST, 0xFE);
  digitalWrite(pin_rclk, HIGH);
  
  pinMode(pin_data, INPUT); //avoid interfering with looping bits
}

unsigned long lastIncrement=0;
unsigned long lastCarry=0;
int carryIndex = 0;
int curDisp=0;
int number[] = {3,4,6,9, 2,4,7,5, 5,1,0,0, 0,0,0,0};

void loop() {                    
  clearDisplay(); //avoid cross talk
  nextDisplay();
  writeDisplay(number[sseg_map[curDisp]]);
  unsigned long curTime = millis();
  unsigned long incTimeDiff = curTime-lastIncrement;
  if(incTimeDiff>=1000){
    lastIncrement = curTime;
    number[0]++;
  }
  if(curTime-lastCarry>=100){
    lastCarry = curTime;
    if(number[carryIndex]>9){
      number[carryIndex]=0;
      carryIndex++;
      number[carryIndex]++;
    }else{
      carryIndex=0;
    } 
  }
}

void writeDisplay(int num){   //REM, need to set other pins high if not calling clearDisplay() first!
  for(int i = 0; i<8; i++){
    if(sseg_encode[num][i]){
      digitalWrite(sseg_pins[i],HIGH);
    }
  }  
}

void clearDisplay(){
  for(int i = 0; i<8; i++){
    digitalWrite(sseg_pins[i],LOW);
  }  
}



void nextDisplay(){
  curDisp++;
  if(curDisp==16)curDisp=0;
  digitalWrite(pin_rclk, LOW);
  digitalWrite(pin_srclk, HIGH);
  digitalWrite(pin_srclk, LOW);
  digitalWrite(pin_rclk, HIGH);
}
