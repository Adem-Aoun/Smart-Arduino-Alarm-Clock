#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Bounce2.h>

// Forward declarations
void updateTime();
void updateButtons();
void handleAlarm();
void updateDisplay();
void drawDigitalClock();
void drawDate();
void handleUI();
void triggerAlarm();
void stopAlarm();

// Pins
#define BUZZER_PIN 10
#define LED_ALARM 9
#define LED_STATUS 8
#define BTN_SET 2
#define BTN_UP 3
#define BTN_DOWN 4
#define BTN_MODE 5
#define POT_PIN A0

// OLED Setup
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// RTC Setup
RTC_DS3231 rtc;
DateTime now;

// Button Setup
Bounce btnSet = Bounce();
Bounce btnUp = Bounce();
Bounce btnDown = Bounce();
Bounce btnMode = Bounce();

// Alarm System
uint8_t alarmHour = 5;
uint8_t alarmMinute = 0;
bool alarmEnabled = true;
bool alarmActive = false;
bool editMode = false;
uint8_t uiState = 0; // 0=Clock, 1=Set Time, 2=Set Alarm

void setup() {
  Serial.begin(9600);
  
  // Initialize RTC
  if(!rtc.begin()) {
    Serial.println("RTC Error");
    while(1);
  }
  if(rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Initialize Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED Error");
    while(1);
  }
  display.setTextColor(WHITE);

  // Button Setup
  btnSet.attach(BTN_SET, INPUT_PULLUP);
  btnUp.attach(BTN_UP, INPUT_PULLUP);
  btnDown.attach(BTN_DOWN, INPUT_PULLUP);
  btnMode.attach(BTN_MODE, INPUT_PULLUP);

  // LED and Buzzer
  pinMode(LED_ALARM, OUTPUT);
  pinMode(LED_STATUS, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
}

void loop() {
  updateTime();
  updateButtons();
  handleUI();
  handleAlarm();
  updateDisplay();
  delay(50);
}

void updateTime() {
  now = rtc.now();
  digitalWrite(LED_STATUS, millis() % 1000 < 100);
}

void updateButtons() {
  btnSet.update();
  btnUp.update();
  btnDown.update();
  btnMode.update();
}

void handleUI() {
  if(btnMode.fell()) {
    uiState = (uiState + 1) % 3;
    editMode = (uiState != 0);
  }

  if(editMode) {
    int potValue = analogRead(POT_PIN);
    
    if(uiState == 1) { // Set Time
      if(btnSet.fell()) {
        uint8_t newHour = map(potValue, 0, 1023, 0, 23);
        rtc.adjust(DateTime(now.year(), now.month(), now.day(), 
                          newHour, now.minute(), 0));
      }
    }
    else { // Set Alarm
      if(btnUp.fell()) alarmHour = map(potValue, 0, 1023, 0, 23);
      if(btnDown.fell()) alarmMinute = map(potValue, 0, 1023, 0, 59);
    }
  }
}

void handleAlarm() {
  if(alarmEnabled && !alarmActive && 
     now.hour() == alarmHour && 
     now.minute() == alarmMinute) {
    triggerAlarm();
  }

  if(alarmActive) {
    tone(BUZZER_PIN, 2000, 500);
    if(btnSet.fell()) stopAlarm();
  }
}

void triggerAlarm() {
  alarmActive = true;
  digitalWrite(LED_ALARM, HIGH);
}

void stopAlarm() {
  alarmActive = false;
  noTone(BUZZER_PIN);
  digitalWrite(LED_ALARM, LOW);
}

void drawEditOverlay() {
  display.fillRect(0, 50, 128, 14, WHITE);
  display.setTextColor(BLACK);
  display.setCursor(5, 52);
  
  if(uiState == 1) {
    display.print("Set Time: ");
    display.print(map(analogRead(POT_PIN), 0, 1023, 0, 23));
  } else {
    display.print("Set Alarm: ");
    display.print(alarmHour);
    display.print(":");
    display.print(alarmMinute);
  }
  display.setTextColor(WHITE);
}

void updateDisplay() {
  display.clearDisplay();
  drawDate();
  drawDigitalClock();
  
  if(alarmActive) {
    display.fillRect(0, 20, 128, 24, WHITE);
    display.setTextColor(BLACK);
    display.setCursor(15, 27);
    display.print("ALARM ACTIVE!");
    display.setTextColor(WHITE);
  }
  
  if(editMode) drawEditOverlay();
  display.display();
}

void drawDigitalClock() {
  display.setTextSize(3);
  String timeString = "";
  if(now.hour() < 10) timeString += "0";
  timeString += now.hour();
  timeString += (now.second() % 2 == 0) ? ":" : " ";
  if(now.minute() < 10) timeString += "0";
  timeString += now.minute();

  // Center clock display
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(timeString, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((128 - w)/2, 25);
  display.print(timeString);
}

void drawDate() {
  display.setTextSize(1);
  display.setCursor(5, 5);
  display.print(
    now.dayOfTheWeek() == 1 ? "MON" :
    now.dayOfTheWeek() == 2 ? "TUE" :
    now.dayOfTheWeek() == 3 ? "WED" :
    now.dayOfTheWeek() == 4 ? "THU" :
    now.dayOfTheWeek() == 5 ? "FRI" :
    now.dayOfTheWeek() == 6 ? "SAT" : "SUN"
  );
  display.print(" ");
  display.print(now.day());
  display.print(" ");
  display.print(
    now.month() == 1 ? "JAN" :
    now.month() == 2 ? "FEB" :
    now.month() == 3 ? "MAR" :
    now.month() == 4 ? "APR" :
    now.month() == 5 ? "MAY" :
    now.month() == 6 ? "JUN" :
    now.month() == 7 ? "JUL" :
    now.month() == 8 ? "AUG" :
    now.month() == 9 ? "SEP" :
    now.month() == 10 ? "OCT" :
    now.month() == 11 ? "NOV" : "DEC"
  );
  
  // Alarm indicator
  if(alarmEnabled) {
    display.setCursor(90, 5);
    display.print("AL ");
    display.print(alarmHour < 10 ? "0" : "");
    display.print(alarmHour);
    display.print(":");
    display.print(alarmMinute < 10 ? "0" : "");
    display.print(alarmMinute);
  }
}
