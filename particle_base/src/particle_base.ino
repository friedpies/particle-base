// This #include statement was automatically added by the Spark IDE.
#include "RCSwitch.h"
#include "pinDefines.h"

String numSwitches = "5";
char *onCodes[] = {"F0F0FFFF0101", "F0F0FFFF1001", "F0F0FFF10001", "F0F0FF1F0001", "F0F0F1FF0001"};
char *offCodes[] = {"F0F0FFFF0110", "F0F0FFFF1010", "F0F0FFF10010", "F0F0FF1F0010", "F0F0F1FF0010"};

// char *onCodes[] = {"010001000101010100110011", "010001000101010111000011", "010001000101011100000011", "010001000101110100000011", "010001000111010100000011"};
// char *offCodes[] = {"010001000101010100111100", "010001000101010111001100", "010001000101011100001100", "010001000101110100001100", "010001000111010100001100"};
int receivedValue = 0;

RCSwitch transmitPin = RCSwitch();
RCSwitch receivePin = RCSwitch();

void setup() {
  Serial.begin(9600);
  Particle.function("switch", switchToggle);
  Particle.function("all", allLights);
  Particle.variable("numSwitches", &numSwitches, STRING);

  pinMode(RECEIVEPIN, INPUT_PULLDOWN);
  pinMode(LEDPIN, OUTPUT);
  transmitPin.enableTransmit(TRANSMITPIN);
  transmitPin.setPulseLength(181);
  receivePin.enableReceive(RECEIVEPIN);
  receivePin.setPulseLength(181);
}


void loop() {
  int inputPinState = digitalRead(RECEIVEPIN);
  digitalWrite(LEDPIN, inputPinState);

  if (receivePin.available()) {
    output(receivePin.getReceivedValue(), receivePin.getReceivedBitlength(), receivePin.getReceivedDelay(), receivePin.getReceivedRawdata(), receivePin.getReceivedProtocol());
    receivePin.resetAvailable();
  }
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

char *bin2tristate(char *bin) {
  char returnValue[50];
  for (int i=0; i<50; i++) {
    returnValue[i] = '\0';
  }
  int pos = 0;
  int pos2 = 0;
  while (bin[pos]!='\0' && bin[pos+1]!='\0') {
    if (bin[pos]=='0' && bin[pos+1]=='0') {
      returnValue[pos2] = '0';
    } else if (bin[pos]=='1' && bin[pos+1]=='1') {
      returnValue[pos2] = '1';
    } else if (bin[pos]=='0' && bin[pos+1]=='1') {
      returnValue[pos2] = 'F';
    } else {
      return "not applicable";
    }
    pos = pos+2;
    pos2++;
  }
  returnValue[pos2] = '\0';
  return returnValue;
}

void output(unsigned long decimal, unsigned int length, unsigned int delay, unsigned int* raw, unsigned int protocol) {

  if (decimal == 0) {
    Serial.print("Unknown encoding.");
  } else {
    char* b = receivePin.dec2binWzerofill(decimal, length);
    char* tristate = bin2tristate(b);

    Serial.print("Decimal: ");
    Serial.print(decimal);
    Serial.print(" (");
    Serial.print( length );
    Serial.print("Bit) Binary: ");
    Serial.print( b );
    Serial.print(" Tri-State: ");
    Serial.print( tristate );
    Serial.print(" PulseLength: ");
    Serial.print(delay);
    Serial.print(" microseconds");
    Serial.print(" Protocol: ");
    Serial.println(protocol);

    Spark.publish("tristate-received", String(delay) + " " + String(tristate));
  }

  Serial.print("Raw data: ");
  for (int i=0; i<= length*2; i++) {
    Serial.print(raw[i]);
    Serial.print(",");
  }
  Serial.println();
  Serial.println();
}
