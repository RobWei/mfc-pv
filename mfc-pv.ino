#include <Keypad.h>

#include "config.h"

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
  delay(5000);
  Serial.println("PV-ANLAGE MFC-OTTIBOTTI");
  pinMode(TRANS_PIN, OUTPUT);
  pinMode(CURRENT_SENSOR, INPUT_PULLUP);

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
  // 5V = 1023
  // 1V = 204,6
  //+300A = 90% of 5V = 4,5 V = 920,7 = 818,4
  //   0A = 50% of 5V = 2,5 V = 511,5 = 409,2
  //-300A = 10% of 5V = 0,5 V = 102,3 =   0,0
  // +-1A =                           = 1,364
  {

    //CURRENT_SENSOR
    //Average Analog-Value 1000er mean
    unsigned long sum;
    for (byte  i = 0; i < 1000; i++)
    {
      sum += analogRead(CURRENT_SENSOR);
    }
    sum = sum / 1000;
    //DEBUG I HOPE THIS WILL WORK
    Serial.print("CURRENT_SENSOR 1000er average: ");
    Serial.println(sum);
    Serial.print("CURRENT_SENSOR in Volts average: ");
    Serial.println(sum / 204.6);
    Serial.print("CURRENT_SENSOR in AMPs average: ");
    unsigned long amps = ((sum - 102.3) * 1.364) - 300;
    Serial.println(amps);
    if (amps >= 5 || amps <= -5)
    {
      Serial.println("Amps größer als 5 oder kleiner als -5");
    }
  }
}
