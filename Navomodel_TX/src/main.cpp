
#include <Arduino.h>
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <elapsedMillis.h>
#include "printf.h"
elapsedMillis sendTimer;
RF24 radio(9,10);  

RF24Network network(radio); 

const uint16_t this_node = 00; // Address of our node in Octal format
const uint16_t node01 = 01;

int pin_fata_spate =A1;
int pin_stanga_dreapta = A0;

struct dataStruct
{
  int stanga = 0;
  int dreapta = 0;
  int fata = 0;
  int spate = 0;
  int nadire = 0;
} myData;

int stanga = 0;
int dreapta = 0;
int fata = 0;
int spate = 0;
int nadire = 0;
void setup(void)
{
 // Serial.begin(9600);
  pinMode(pin_fata_spate, INPUT);
  pinMode(pin_stanga_dreapta, INPUT);
  pinMode(4, INPUT_PULLUP);

  SPI.begin();
  radio.begin();
  radio.setPALevel(RF24_PA_MIN); // RF24_PA_MIN ,RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate(RF24_250KBPS); // RF24_250KBPS, RF24_1MBPS, RF24_2MBPS
  network.begin(90, this_node);
  printf_begin();
  radio.printDetails();
}

void loop()
{
  stanga = map(analogRead(pin_fata_spate), 514, 1024, 0,100);

  if (stanga <= 100)
  {
    if (stanga < 0)
    {
      stanga = 0;
    }
    myData.stanga = stanga;
  }

  dreapta = map(analogRead(pin_fata_spate), 500, 0, 0, 100);

  if (dreapta <= 100)
  {
    if (dreapta < 0)
    {
      dreapta = 0;
    }
    myData.dreapta = dreapta;
  }

  fata = map(analogRead(pin_stanga_dreapta), 514, 1020,0,100);

  if (fata <= 100)
  {
    if (fata < 0)
    {
      fata = 0;
    }
    myData.fata = fata;
  }

  spate = map(analogRead(pin_stanga_dreapta), 500, 0, 0, 100);

  if (spate <= 100)
  {
    if (spate < 0)
    {
      spate = 0;
    }
    myData.spate = spate;
  }

  myData.nadire = digitalRead(4);

  network.update();
  if (sendTimer >= 300)
  {

    Serial.println("Send packet# ");
    Serial.println(analogRead(pin_fata_spate));
    Serial.println(analogRead(pin_stanga_dreapta));
    Serial.println(myData.fata);
    Serial.println(myData.spate);
    Serial.println(myData.stanga);
    Serial.println(myData.dreapta);
    Serial.println(myData.nadire);

    RF24NetworkHeader header(node01);
    bool ok = network.write(header, &myData, sizeof(myData));
    if (ok)
      Serial.println("ONLINE.");
    else
      Serial.println("OFFLINE !.");

    sendTimer = 0;
  }
}
