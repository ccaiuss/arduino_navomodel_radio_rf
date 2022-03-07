
#include <Arduino.h>
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "printf.h"
#include <elapsedMillis.h>
elapsedMillis reciveTimer;

int pulseWidth(int angle);
RF24 radio(9,10); // nRF24L01(+) radio attached using Getting Started board

RF24Network network(radio);    // Network uses that radio
const uint16_t this_node = 01; // Address of our node in Octal format ( 04,031, etc)

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define MIN_PULSE_WIDTH 650
#define MAX_PULSE_WIDTH 2350
#define DEFAULT_PULSE_WIDTH 1500
#define FREQUENCY 50

struct dataStruct
{
  int stanga = 0;
  int dreapta = 0;
  int fata = 0;
  int spate = 0;
  int nadire = 0;
} myData;

void setup(void)
{
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);

  //Serial.begin(9600);
   pwm.begin();
   pwm.setPWMFreq(FREQUENCY);

  SPI.begin();
  radio.begin();
  radio.setPALevel(RF24_PA_MIN);   // RF24_PA_MIN ,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate(RF24_250KBPS); // RF24_250KBPS, RF24_1MBPS, RF24_2MBPS
  network.begin(/*channel*/ 90, /*node address*/ this_node);
  printf_begin();
  radio.printDetails();

  reciveTimer = 0;
  myData.stanga = 0;
  myData.dreapta = 0;
  myData.fata = 0;
  myData.spate = 0;
  myData.nadire = 1;
  reciveTimer = 0;
}

void loop(void)
{
  network.update();
  if (network.available())
  {
    RF24NetworkHeader header;
    network.read(header, &myData, sizeof(myData));
    
    Serial.println("Received packet# ");
    Serial.println(myData.fata);
    Serial.println(myData.spate);
    Serial.println(myData.stanga);
    Serial.println(myData.dreapta);
    Serial.println(myData.nadire);
    reciveTimer = 0;
  }
  if (myData.fata > 10)
  {
    digitalWrite(2, LOW);
    digitalWrite(3, HIGH);
    analogWrite(5, map(myData.fata, 0, 100, 0, 249));
  }
  else if (myData.spate > 10)
  {
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    analogWrite(5, map(myData.spate, 0, 100, 0, 249));
  }
  else
  {
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    analogWrite(5, 0);
  }
  if (myData.dreapta > 0)
  {
     pwm.setPWM(0, 0, pulseWidth(map(myData.dreapta, 0, 100, 80, 120)));
  }
  else if (myData.stanga > 0)
  {
     pwm.setPWM(0, 0, pulseWidth(map(myData.stanga, 0, 100, 80, 40)));
  }
  else
  {
     pwm.setPWM(0, 0, pulseWidth(80));
  }

  if (myData.nadire == 0)
  {
     pwm.setPWM(1, 0, pulseWidth(48));
  }
  else
  {
     pwm.setPWM(1, 0, pulseWidth(20));
  }

  if (reciveTimer > 2000)
  {
    Serial.println("reciveTimer >2000");
    myData.stanga = 0;
    myData.dreapta = 0;
    myData.fata = 0;
    myData.spate = 0;
    myData.nadire = 1;
    reciveTimer = 0;

  }
}

int pulseWidth(int angle)
{
  int pulse_wide, analog_value;
  pulse_wide = map(angle, 0, 180, MIN_PULSE_WIDTH, MAX_PULSE_WIDTH);
  analog_value = int(float(pulse_wide) / 1000000 * FREQUENCY * 4096);
  return analog_value;
}