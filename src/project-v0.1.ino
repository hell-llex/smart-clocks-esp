#include "wifi-connect.h"
#include "server-init.h"
#include "SevSeg.h"

SevSeg sevseg;
extern int output2;
extern int digitsNumber;
extern int digitsPoint;

void setup(void)
{
  Serial.begin(115200);

  pinMode(output2, OUTPUT);

  digitalWrite(output2, LOW);

  byte numDigits = 4;
  byte digitPins[] = {19, 17, 16, 14};
  byte segmentPins[] = {18, 4, 25, 23, 22, 5, 26, 27};
  bool resistorsOnSegments = false;
  byte hardwareConfig = COMMON_ANODE;
  bool updateWithDelays = false;
  bool leadingZeros = false;
  bool disableDecPoint = false;
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
  updateWithDelays, leadingZeros, disableDecPoint);

  wifiInit();
  serverInit();
  serverControl();
}

void loop(void)
{
  serverHandler();
  updateDisplayDigits(digitsNumber, digitsPoint);
}

void updateDisplayDigits(int numDigits, int pointDigits) {
  sevseg.setNumber(numDigits, pointDigits);
  sevseg.refreshDisplay();
}