#define f07 for(int i = 0; i<8; i++)  

const int spm_ssegd[] = {8,11,10,9, 13,14,15,12,   0,1,2,3, 5,6,7,4};// {11,8,9,10, 12,13,14,15, 3,0,1,2, 6,5,4,7}; {7,4,5,6, 2,1,0,3,  15,14,13,12, 10,9,8,11}; //will allow shift registers to be wired as convienent on pcb
const int pm_sseg[] = {8,6,10,12,13,7,9,11};                        //a=0,b=1,c=2,d=3,e=4,f=5,g=6, dp=14 (dp not wired on full bb test)

const boolean dec_sseg[][8] = {
  //a,b,c,d,e,f,g, dp
  {1,1,1,1,1,1,0, 0 }, //0    
  {0,1,1,0,0,0,0, 0 }, //1     ----a----
  {1,1,0,1,1,0,1, 0 }, //2    |         |
  {1,1,1,1,0,0,1, 0 }, //3    f         b
  {0,1,1,0,0,1,1, 0 }, //4    |         |
  {1,0,1,1,0,1,1, 0 }, //5     ----g----
  {1,0,1,1,1,1,1, 0 }, //6    |         |
  {1,1,1,0,0,0,0, 0 }, //7    e         c
  {1,1,1,1,1,1,1, 0 }, //8    |         |
  {1,1,1,1,0,1,1, 0 }, //9     ----d----  .dp
  
  {1,1,1,1,1,1,0, 1 }  //0.(for carry) (dp will show on propegation)
};

//74HC595 shift register pins
const int sp_data = 2;
const int sp_rclk = 3; //ST_CP
const int sp_srclk = 4;//SH_CP


int number[16]; //now1558 7063 24Time 
void setup() {
  //setup 7seg pins
  f07{
    pinMode(pm_sseg[i], OUTPUT);
    digitalWrite(pm_sseg[i],LOW);
  }
  
  //setup shift reg. pins
  pinMode(sp_data, OUTPUT);
  pinMode(sp_rclk, OUTPUT);
  pinMode(sp_srclk, OUTPUT);

  //Initialise shift registers such that only the last bit is low
  //two 8 bit shift registers in a loop, with sp_data tapping the "first" register's data line & "over-riding" the loop back from the "second" register
  digitalWrite(sp_rclk, LOW);           
  shiftOut(sp_data, sp_srclk, MSBFIRST, 0xFF);   
  shiftOut(sp_data, sp_srclk, MSBFIRST, 0xFE);
  digitalWrite(sp_rclk, HIGH);
  
  pinMode(sp_data, INPUT); //avoid interfering with looping bits

  if(EEPROM.read(0) != 2){
    number = {3,2,5,6, 0,7,8,5, 5,1,0,0, 0,0,0,0};
  } else {
    for(int i = 1; i<17; i++){
      number[i-1] = EEPROM.read(i);
    }
  }
}

unsigned long lastIncrement=0;
unsigned long lastCarry=0;
int carryIndex = 0;
int curDisp=0;


void loop() {                    
  clearDisplay(); //avoid cross talk
  nextDisplay();
  writeDisplay(number[spm_ssegd[curDisp]]);
  unsigned long curTime = millis();
  unsigned long incTimeDiff = curTime-lastIncrement;
  if(incTimeDiff>=1000){
    lastIncrement = curTime;
    number[0]++;
    EEPROM.write(0,2);
    for(int i = 1; i<17; i++){
      EEPROM.write(i,number[i-1]);
    }
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
  f07 if(dec_sseg[num][i]){
      digitalWrite(pm_sseg[i],HIGH);
  }  
}

void clearDisplay(){
  f07 digitalWrite(pm_sseg[i],LOW);
}

void nextDisplay(){
  curDisp++;
  if(curDisp==16)curDisp=0;
  digitalWrite(sp_rclk, LOW);
  digitalWrite(sp_srclk, HIGH);
  digitalWrite(sp_srclk, LOW);
  digitalWrite(sp_rclk, HIGH);
}
