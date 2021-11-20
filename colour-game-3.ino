#define BUZZER_PIN 0
#define RED_LED_PIN 4
#define YELLOW_LED_PIN 1
#define GREEN_LED_PIN 2

#define COUNTCLICKS 6             //сложность игры или количество нажатий
#define STDELAY 200               //стандартная задержка
#define BIGDELAY 1000             //большая задержка

#define RED_SIGNAL 0
#define YELLOW_SIGNAL 1
#define GREEN_SIGNAL 2
#define WIN_SIGNAL 3
#define FAULT_SIGNAL 4
#define HELLO_SIGNAL 5

int iCurVal, iRefVal;
byte iPressedButNum;
static int iRValue[4];
//  3.3V      5V        - замеры референсных значений 
//                        на аналоговом входе при разном напряжении
//    +40=590   +63=913
//  549       850       - на х1 сопротивлении
//    -40=510   -63=787
//  469       725       - на х2 сопротивлении
//    +30=440   +45=679
//  410       634       - на х3 сопротивлении
//    -30=390   -45=589
//  RESULT=~7%

byte gamemode;
byte bCount;
byte myClicks[COUNTCLICKS];
bool isWinner;
uint32_t tmr;

void clearallpins(){
  digitalWrite(GREEN_LED_PIN, 0);
  digitalWrite(YELLOW_LED_PIN, 0);
  digitalWrite(RED_LED_PIN, 0);
  digitalWrite(BUZZER_PIN, 0);
  noTone(BUZZER_PIN);
}

void dosignal(byte sygnaltype){
  // 0 - RED SIGNAL
  // 1 - YELLOW SIGNAL
  // 2 - GREEN SIGNAL
  // 3 - WIN SIGNAL
  // 4 - FAULT SIGNAL
  // 5 - HELLO SIGNAL
  switch (sygnaltype) {
    case RED_SIGNAL:
      //Serial.println("RED");
      digitalWrite(RED_LED_PIN, 1);
      tone(BUZZER_PIN, 1000, STDELAY);
      delay(STDELAY);
      clearallpins();
      break;
    case YELLOW_SIGNAL:
      //Serial.println("YELLOW");
      digitalWrite(YELLOW_LED_PIN, 1);
      tone(BUZZER_PIN, 1300, STDELAY);
      delay(STDELAY);
      clearallpins();
      break;
    case GREEN_SIGNAL:
      //Serial.println("GREEN");
      digitalWrite(GREEN_LED_PIN, 1);
      tone(BUZZER_PIN, 1600, STDELAY);
      delay(STDELAY);
      clearallpins();
      break;
    case WIN_SIGNAL:
      //Serial.println("WIN");
      digitalWrite(GREEN_LED_PIN, 1);
      tone(BUZZER_PIN, 600);
      delay(STDELAY/2);
      tone(BUZZER_PIN, 800);
      delay(BIGDELAY/2);
      clearallpins();
      break;
    case FAULT_SIGNAL:
      //Serial.println("FAULT");
      digitalWrite(RED_LED_PIN, 1);
      tone(BUZZER_PIN, 300);
      delay(STDELAY/2);
      tone(BUZZER_PIN, 100);
      delay(BIGDELAY/2);
      clearallpins();
      break;
    case HELLO_SIGNAL:
      //Serial.println("HELLO");
      dosignal(RED_SIGNAL);
      dosignal(YELLOW_SIGNAL);
      dosignal(GREEN_SIGNAL);
      dosignal(YELLOW_SIGNAL);
      dosignal(RED_SIGNAL);
      clearallpins();
      break;
    default:
      break;
  }
}

void setup(){
  //Serial.begin(9600);
  pinMode(A3, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  clearallpins();
  delay(100);
  iRefVal = analogRead(A3); //юстирует значение при нажатии "красной" кнопки при загрузке
  if (iRefVal < 100 || iRefVal > 1000) iRefVal = 850; //850- экспериментальное значение
  else dosignal(WIN_SIGNAL);
  iRValue[0] = iRefVal - iRefVal*0.07*5;
  iRValue[1] = iRefVal - iRefVal*0.07*3;
  iRValue[2] = iRefVal - iRefVal*0.07;
  iRValue[3] = iRefVal + iRefVal*0.07;
  
  gamemode = 1;
  tmr = millis();
}

int getPressedButNum(){
  iCurVal = analogRead(A3);
  //Serial.println(iCurVal);
  if (iCurVal > iRValue[0] && iCurVal < iRValue[1] && millis() - tmr >= 500) {
    tmr = millis();
    return 3; //GREEN
  }
  else if (iCurVal > iRValue[1] && iCurVal < iRValue[2] && millis() - tmr >= 500){
    tmr = millis();
    return 2; //YELLOW
  }
  else if (iCurVal > iRValue[2] && iCurVal < iRValue[3] && millis() - tmr >= 500){
    tmr = millis();
    return 1; //RED
  }
  else {
    return 0;
  }
}

void loop(){
  if (gamemode == 1){
    delay(BIGDELAY);
    dosignal(HELLO_SIGNAL);
    delay(BIGDELAY);
    bCount = 0;
    do {
      iPressedButNum = getPressedButNum();
      if (iPressedButNum==1) {
        dosignal(RED_SIGNAL);
        myClicks[bCount] = RED_SIGNAL;
        bCount++;
      }
      if (iPressedButNum==2){
        dosignal(YELLOW_SIGNAL);
        myClicks[bCount] = YELLOW_SIGNAL;
        bCount++;
      }
      if (iPressedButNum==3){
        dosignal(GREEN_SIGNAL);
        myClicks[bCount] = GREEN_SIGNAL;
        bCount++;
      }  
    } while (bCount < COUNTCLICKS);
    gamemode = 2;
  }
  else if (gamemode == 2){
    isWinner = true;
    delay(BIGDELAY);
    for (byte i = 0; i < COUNTCLICKS; i++){
      switch (myClicks[i]) {
        case RED_SIGNAL:
          dosignal(RED_SIGNAL);
          delay(BIGDELAY/2);
          break;
        case YELLOW_SIGNAL:
          dosignal(YELLOW_SIGNAL);
          delay(BIGDELAY/2);
          break;
        case GREEN_SIGNAL:
          dosignal(GREEN_SIGNAL);
          delay(BIGDELAY/2);
          break;
      }
      gamemode = 3;
    }
  }
  else if (gamemode == 3){
    bCount = 0;
    do {
      iPressedButNum = getPressedButNum();
      if (iPressedButNum == 1) {
        dosignal(RED_SIGNAL);
        if (myClicks[bCount] != RED_SIGNAL) isWinner = false;
        bCount++;
      }
      if (iPressedButNum == 2){
        dosignal(YELLOW_SIGNAL);
        if (myClicks[bCount] != YELLOW_SIGNAL) isWinner = false;
        bCount++;
      }
      if (iPressedButNum == 3){
        dosignal(GREEN_SIGNAL);
        if (myClicks[bCount] != GREEN_SIGNAL) isWinner = false;
        bCount++;
      }  
    } while (bCount < COUNTCLICKS && isWinner);
    
    if (isWinner) {
      delay(BIGDELAY/2);
      dosignal(WIN_SIGNAL);
      bCount = 0;
      do {
        myClicks[bCount] = NULL;
        bCount++; 
      } while (bCount < COUNTCLICKS);
      gamemode = 1;
    }
    else {
      delay(BIGDELAY/2);
      dosignal(FAULT_SIGNAL);
      gamemode = 2;
    }
    delay(BIGDELAY);
  }
}
 
