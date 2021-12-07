#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
//PINS
int BLUE_LED = 8;
int RED_LED = 7;
int GREEN_LED = 5;

int pirPin = 2;          // PIR Out pin
int pirStat = 0;             // PIR status
const int BUTTON = 4;     // the number of the pushbutton pin
int buttonState = 0;
int btnLast = HIGH;
int states = 1;
const int SAFE = 1;
const int MONITOR = 2;

//bools
bool modSafe = true;
bool modLocked = false;
bool isMotion = false;
bool previousStats = false;
bool triggerAlarm = false;
//millis
const int SAFE_INTERVAL = 10;
const int TIMER = 50;//ms
const int RFID = 10;
const int BUTTON_INTERVAL = 30;
const int MOTION_INTERVAL = 15000;
unsigned long current_time;
unsigned long timer_time = 0;
unsigned long rfid_time = 0;
unsigned long btn_time = 0;
unsigned long safe_time = 0;
unsigned long motion_time = 0;
void setup()
{
  Serial.begin(9600);   // Initiate a serial communication
  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(BUTTON, INPUT_PULLUP);
  timer_time = millis();
  rfid_time = millis();
  btn_time = millis();
  safe_time = millis();
  motion_time = millis();
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  //Serial.println("Approximate your card to the reader...");
 // Serial.println();

}
void loop()
{
  current_time = millis();
  buttonState = digitalRead(BUTTON);
  pirStat = digitalRead(pirPin);
  if ((current_time - btn_time) > BUTTON_INTERVAL)
  {
    if (buttonState != btnLast)
    {
      if (buttonState == LOW)
      {
        states++;
        if (states > 2)
        {
          states = 1;
        }
        Serial.println(states);
      }
      btnLast = buttonState;
    }
    btn_time = current_time;
  }
  if ((current_time - safe_time) > SAFE_INTERVAL)
  {
    if (states == SAFE)
    {
      digitalWrite(BLUE_LED, LOW);
    }
    safe_time = current_time;
  }
  if (states == MONITOR) {
    if ((current_time - timer_time ) > TIMER)  //TIMER TO ID
    {
      if (pirStat == HIGH) {            // if motion detected
        digitalWrite(BLUE_LED, HIGH);
        isMotion = true;

      }
      else {
        digitalWrite(BLUE_LED, LOW);
        isMotion = false;
      }
      if (isMotion != previousStats)
      {
        if (isMotion)
        {
          //Serial.println(isMotion);
        }
        previousStats = isMotion;
      }
      timer_time = current_time;
    }
  }


  if ((current_time - motion_time) > MOTION_INTERVAL)
  {
    if (triggerAlarm == true)
    {
      //buzzer every three minutes?
      Serial.println("Alarm");
    }
    if (isMotion == true)
    {
      triggerAlarm = true;
    }
    motion_time = current_time;
  }

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
  //Show UID on serial monitor
  //Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    //Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  //Serial.println();
  //Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "01 02 03 04" || content.substring(1) == "D3 BE FB 14" ) //change here the UID of the card/cards that you want to give access
  {
    digitalWrite(BLUE_LED, LOW);
    //Serial.println("Authorized access");
    triggerAlarm = false;
    isMotion = false;
    states = SAFE;
    //Serial.println();
    digitalWrite(GREEN_LED, HIGH);
    delay(1000);
    digitalWrite(GREEN_LED, LOW);
  }

  else   {
    //Serial.println("Denied");
    isMotion = true;
    digitalWrite(RED_LED, HIGH);
    delay(1000);
    digitalWrite(RED_LED, LOW);
  }
}
