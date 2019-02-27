// This #include statement was automatically added by the Spark IDE.
#include "RCSwitch.h"
#include "pinDefines.h"

String numSwitches = "5";
char *onCodes[] = {"F0F0FFFF0101", "F0F0FFFF1001", "F0F0FFF10001", "F0F0FF1F0001", "F0F0F1FF0001"};
char *offCodes[] = {"F0F0FFFF0110", "F0F0FFFF1010", "F0F0FFF10010", "F0F0FF1F0010", "F0F0F1FF0010"};
int receivedValue = 0;

RCSwitch transmitPin = RCSwitch();
RCSwitch receivePin = RCSwitch();

void setup() {
  Serial.begin(9600);
  Particle.function("switch", switchToggle);
  Particle.function("all", allLights);
  Particle.variable("numSwitches", &numSwitches, STRING);
  transmitPin.enableTransmit(TRANSMITPIN);
  transmitPin.setPulseLength(181);
  receivePin.enableReceive(RECEIVEPIN);
  receivePin.setPulseLength(181);
}


void loop() {

}

int switchToggle(String command) {
  String switchNumber = command.substring(0, 1);
  String switchStatus = command.substring(1, 2);
  String returnString = "";

  int switchInt = switchNumber.toInt();
  int statusInt = switchStatus.toInt();
  int returnInt = 0;

  if (statusInt == 1) {
    returnString = switchNumber + switchStatus;
    returnInt = returnString.toInt();
    transmitPin.sendTriState(onCodes[switchInt - 1]);
    return returnInt;
  }
  else if (statusInt == 0) {
    returnString = switchNumber + switchStatus;
    returnInt = returnString.toInt();
    transmitPin.sendTriState(offCodes[switchInt - 1]);
    return returnInt;
  }
  else {
    return -1;
  }
}

int allLights(String command) {
  if(command == "1") {
    for(int i = 0; i < numSwitches.toInt(); i++) {
      transmitPin.sendTriState(onCodes[i]);
      delay(50);
    }
  }

  if(command == "0") {
    for(int i = 0; i < numSwitches.toInt(); i++) {
      transmitPin.sendTriState(offCodes[i]);
      delay(50);
    }
  }
  return 1;
}
