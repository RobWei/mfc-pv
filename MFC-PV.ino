#include <Keypad.h>

#include <MFC-PV.h>

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {A6, A5, A4, A3 };
byte colPins[COLS] = {A2, A1, A0};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  Serial.begin(9600);
  delay(2000);
  Serial.println("PV-ANLAGE MFC-OTTIBOTTI");
  pinMode(TRANS_PIN, OUTPUT);
}

String last_digits = "";
bool anlage_is_active = false;
long unlockMillis = 0;


void loop() {
  char key = keypad.getKey();
  digitalWrite(TRANS_PIN, (anlage_is_active == true ? HIGH : LOW));
  if (key) {
    last_digits += key;
    Serial.print("Eingabe: ");
    Serial.print(last_digits);
    Serial.print("  ANLAGE: ");
    Serial.println((anlage_is_active == true ? "EINGESCHALTET" : "AUSGESCHALTET"));
  }
  if (last_digits.indexOf(ON_CODE) != -1)
  {
    Serial.println("Anlage freigeschaltet!");
    last_digits = "";
    anlage_is_active = true;
    unlockMillis = millis();
  }
  if (last_digits.indexOf(OFF_CODE) != -1)
  {
    Serial.println("Anlage ausgeschaltet!");
    last_digits = "";
    anlage_is_active = false;
    unlockMillis = 0;
  }
  if (anlage_is_active && ((((millis() - unlockMillis) / 1000) / 60) >= ABSCHALTDAUER_MIN))
  {
    Serial.println("Anlage automatisch ausgeschaltet!");
    last_digits = "";
    anlage_is_active = false;
    unlockMillis = 0;
  }
  //Strom-Sensor
  {
    //CURRENT_SENSOR
  }
}
