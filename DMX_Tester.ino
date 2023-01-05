#include <DMXSerial.h>
const int WhitePin = 6;

const int signalRedPin = 7;
const int signalGreenPin = 8;
const int signalBluePin = 9;

const int powerRedPin = 7;
const int powerGreenPin = 8;
const int powerBluePin = 9;

const int switchPin = 10;

const int readPin = 11;
const int sendPin = 12;

const int batteryMonitor = A2;

int modeSwitch = 0;
int channel = 1;
int dmxValue = 0;

#define ChannelSelectPot A0
#define ValueSelectSlider A1

#define WhiteDefaultLevel 0
#define RedDefaultLevel 0
#define GreenDefaultLevel 0
#define BlueDefaultLevel 0


void setup() {
  pinMode(WhitePin, OUTPUT);
  
  pinMode(signalRedPin, OUTPUT);
  pinMode(signalGreenPin, OUTPUT);
  pinMode(signalBluePin, OUTPUT);

  pinMode(powerRedPin, OUTPUT);
  pinMode(powerGreenPin, OUTPUT);
  pinMode(powerBluePin, OUTPUT);
  
  pinMode(switchPin, INPUT);
}

void loop() {
  modeSwitch = digitalRead(switchPin);
  
  // DMX Send Mode
  if (modeSwitch == 1){
    DMXSerial.init(DMXController);
    //Put into Send Mode
    digitalWrite(readPin, HIGH);
    digitalWrite(sendPin, HIGH);
  }
  while (modeSwitch == 1){
    int channelRead = analogRead(ChannelSelectPot);
    int channel = map(channelRead, 0, 1023, 0, 255);
    int valueRead = analogRead(ValueSelectSlider);
    int sliderValue = map(valueRead, 0, 1023, 0, 255);
    DMXSerial.write(channel, sliderValue);
    analogWrite(WhitePin, sliderValue);
    delayMicroseconds(250);
    modeSwitch = digitalRead(switchPin);
    CheckBattery();
  }
  // DMX Read Mode
  if (modeSwitch == 1){
    DMXSerial.init(DMXReceiver);
    //Put into Read Mode
    digitalWrite(readPin, LOW);
    digitalWrite(sendPin, LOW);
  }
  while (modeSwitch == 0){
    // Calculate how long no data bucket was received
    unsigned long lastPacket = DMXSerial.noDataSince();
  
    int channelRead = analogRead(ChannelSelectPot);
    int channel = map(channelRead, 0, 1023, 0, 255);
  
    if (lastPacket < 5000) {
      // read recent DMX values and set pwm levels
      dmxValue = DMXSerial.read(channel);
      analogWrite(signalRedPin, RedDefaultLevel);
      analogWrite(signalGreenPin, 255);
      analogWrite(signalBluePin, BlueDefaultLevel);
      analogWrite(WhitePin, dmxValue);
    }
    else {
      // Show pure red color, when no data was received since 5 seconds or more.
      analogWrite(signalRedPin, 255);
      analogWrite(signalGreenPin, GreenDefaultLevel);
      analogWrite(signalBluePin, BlueDefaultLevel);
    }
    delayMicroseconds(250);
    modeSwitch = digitalRead(switchPin);
    CheckBattery();
  }
}

void CheckBattery() {
    // Adjust Values based on Arduino Range of 0-1023 and the voltage of the battery
    int value = analogRead(batteryMonitor);
    int batteryPercent = 0;
    
    if (value >= 3100) {
      batteryPercent = 100;
      // Green LED
    }
    else if (value >= 2820) {   // 90-100% range
      batteryPercent = map(value, 2820, 3100, 90, 100);
      // Green LED
    }
    else if (value >= 2540) {   // 10-90% range
      batteryPercent = map(value, 2540, 2820, 10, 90);
      // Break up smaller, Yellow LED
    }
    else if (value >= 1700) {   // 0-10% range
      batteryPercent = map(value, 1700, 2540, 0, 10);
      // Red LED
    }
    else {
      batteryPercent = 0;
      // Blink Red LED
    }
    // if charging -> make blue until charged, then make blink green
}
