// Traffic light controller with pedestrian crossing button
// Board: Arduino Uno (Wokwi simulator). Language: C++

// ---- Pin assignments ----
const uint8_t CAR_RED    = 13;
const uint8_t CAR_YELLOW = 12;
const uint8_t CAR_GREEN  = 11;
const uint8_t PED_RED    = 10;
const uint8_t PED_GREEN  = 9;
const uint8_t BUTTON_PIN = 2;   // button between pin 2 and GND

// ---- Timing (milliseconds) ----
const unsigned long GREEN_MAX   = 10000; // longest time cars keep green
const unsigned long GREEN_MIN   = 3000;  // shortest green before a button request is served
const unsigned long YELLOW_TIME = 2000;
const unsigned long WALK_TIME   = 5000;  // pedestrians cross
const unsigned long CLEAR_TIME  = 1500;  // everything red, junction clears

// ---- State machine ----
enum State { CARS_GREEN, CARS_YELLOW, PED_WALK, ALL_RED };
State state = CARS_GREEN;
unsigned long stateStart = 0;
bool pedRequested = false;

void setLights(bool carR, bool carY, bool carG, bool pedR, bool pedG) {
  digitalWrite(CAR_RED, carR);
  digitalWrite(CAR_YELLOW, carY);
  digitalWrite(CAR_GREEN, carG);
  digitalWrite(PED_RED, pedR);
  digitalWrite(PED_GREEN, pedG);
}

void enterState(State next) {
  state = next;
  stateStart = millis();
  switch (state) {
    case CARS_GREEN:
      setLights(LOW, LOW, HIGH, HIGH, LOW);
      Serial.println("State: CARS_GREEN");
      break;
    case CARS_YELLOW:
      setLights(LOW, HIGH, LOW, HIGH, LOW);
      Serial.println("State: CARS_YELLOW");
      break;
    case PED_WALK:
      setLights(HIGH, LOW, LOW, LOW, HIGH);
      pedRequested = false;           // request served
      Serial.println("State: PED_WALK");
      break;
    case ALL_RED:
      setLights(HIGH, LOW, LOW, HIGH, LOW);
      Serial.println("State: ALL_RED");
      break;
  }
}

void setup() {
  pinMode(CAR_RED, OUTPUT);
  pinMode(CAR_YELLOW, OUTPUT);
  pinMode(CAR_GREEN, OUTPUT);
  pinMode(PED_RED, OUTPUT);
  pinMode(PED_GREEN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // pressed = LOW
  Serial.begin(9600);
  enterState(CARS_GREEN);
}

void loop() {
  // Register a pedestrian request only while cars have green or yellow
  if (digitalRead(BUTTON_PIN) == LOW && (state == CARS_GREEN || state == CARS_YELLOW)) {
    if (!pedRequested) Serial.println("Pedestrian request registered");
    pedRequested = true;
  }

  unsigned long elapsed = millis() - stateStart;

  switch (state) {
    case CARS_GREEN:
      if (elapsed >= GREEN_MAX || (pedRequested && elapsed >= GREEN_MIN)) {
        enterState(CARS_YELLOW);
      }
      break;
    case CARS_YELLOW:
      if (elapsed >= YELLOW_TIME) {
        enterState(pedRequested ? PED_WALK : ALL_RED);
      }
      break;
    case PED_WALK:
      if (elapsed >= WALK_TIME) enterState(ALL_RED);
      break;
    case ALL_RED:
      if (elapsed >= CLEAR_TIME) enterState(CARS_GREEN);
      break;
  }
}