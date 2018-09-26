
#include <ChipLib3.h>
#include "theSongs.h"
#include <MBlueBotLib.h>

const int SPEED = 120;
const int SPEED_MINUS = 50;

//les Obj

//
DigitalOut led_13(13);
//
Note son("C%3", 300);
//
Buzzer buzzer(8);
//
MBlueButton button;
//
MBlueLineSensor lineSensor(PORT2);
//
MBlueUltrasonic ultrasonic(PORT3);
//
MBlueRGBLed ledrgb;
//
MBlueMotors motors;
//
int linefollow = 0;
int cmpMur = 0;
float distance = 0;

int ancien = 0; //g=-1 d=+1
//**********************************
void testledrgb(int tempo)
{
  ledrgb.clear();
  delay(100);

  //led 1
  ledrgb.setColor(1, 10, 0, 0);
  delay(tempo);
  //
  ledrgb.setColor(1, 0, 10, 0);
  delay(tempo);
  //
  ledrgb.setColor(1, 0, 0, 10);
  delay(tempo);
  //
  ledrgb.clear();
  delay(tempo);
  //
  //led2
  ledrgb.setColor(2, 10, 0, 0);
  delay(tempo);
  //
  ledrgb.setColor(2, 0, 10, 0);
  delay(tempo);
  //
  ledrgb.setColor(2, 0, 0, 10);
  delay(tempo);
  //
  //led1 et 2
  ledrgb.setColor( 10, 0, 0);
  delay(tempo);
  //
  ledrgb.setColor(0, 10, 0);
  delay(tempo);
  //
  ledrgb.setColor(0, 0, 10);
  delay(tempo);
}
//**********************************
void testUltrasonic()
{
  float val = ultrasonic.getDictanceCm();

  Serial.print("distance= ");
  Serial.println(val);

  delay(1000);
}
//**********************************
void testMotors()
{
  motors.setSpeeds(SPEED, SPEED, 300);
  //
  motors.setSpeeds(-SPEED, -SPEED, 300);
  //
}
//*********************************
void testmouv()
{
  bot_front(SPEED, 2000);
  //
  bot_stop(200);
  //
  bot_left(SPEED, 1000);
  //
  bot_stop(200);
  //
  bot_right(SPEED, 1000);
  //
  bot_back(SPEED, 2000);
  //
  bot_stop(200);
}
//*********************************
void bot_front(int speed, int pause_ms)
{
  motors.setSpeeds(speed, speed, pause_ms);
}
//***********************************
void bot_stop(int pause_ms)
{
  motors.stop(pause_ms);
}
//***********************************
void bot_back(int speed, int pause_ms)
{
  motors.setSpeeds(-speed, -speed, pause_ms);
}
//***********************************
void bot_left(int speed, int pause_ms)
{
  motors.setSpeeds(SPEED_MINUS, speed, pause_ms);
}
//*********************************
void bot_right(int speed, int pause_ms)
{
  motors.setSpeeds(speed, SPEED_MINUS, pause_ms);
}
//***********************************
void bot_flip(int speed, int pause_ms)
{
  motors.setSpeeds(-speed, speed, pause_ms);
}
//*********************************
//test led pin 13
void testLed()
{
  led_13.write(1);
  delay(200);
  led_13.write(0);
  delay(200);
}
//*************************************
LineState getLineSensor()
{
  bool capt1 = lineSensor.readSensor1();
  bool capt2 = lineSensor.readSensor2();

  if (capt1 == true && capt2 == true)
    return NO_LINE;
  //
  if (capt1 == false && capt2 == true)
    return LINE_LEFT;
  //
  if (capt1 == true && capt2 == false)
    return LINE_RIGHT;
  //
  return LINE_CENTER;
}
//*********************
void eviteMur()
{
  cmpMur++;

  LineState posi = NO_LINE;

  if (cmpMur  > 10)
  {
    ledrgb.setColor(10, 0, 0);
    bot_left(SPEED, 200);

    //
    while (posi == NO_LINE)
    {
      posi = getLineSensor();
      bot_flip(SPEED, -1);
    }
    //
    cmpMur = 0;
    ledrgb.clear();
  }
}
//**********************
int cherchePiste()
{
  LineState posi = NO_LINE;
  int cmp = 0;
  //
  bot_stop(100);
  ledrgb.setColor(10, 10, 0);
  //
  while (cmp < 1000 && posi == NO_LINE)
  {
    if (ancien < 0)//piste etait a gauche
    {
      motors.setSpeeds(-SPEED, SPEED, -1);
    }
    //
    else
    {
      motors.setSpeeds(SPEED, -SPEED, -1);
    }

    posi = getLineSensor();
    cmp++;
    delay(1);
  }
  bot_stop(0);
  //
  if (posi == LINE_RIGHT || posi == LINE_LEFT)
  {
    ledrgb.clear();
    return 1;
  }
  //
  else
  {
    if (ancien < 0)
    {
      motors.setSpeeds(SPEED, -SPEED, 1000);
    }
    //
    else
    {
      motors.setSpeeds(-SPEED, SPEED, 1000);
    }
    //
    bot_stop(0);
    cmp = 0;
    posi = NO_LINE;
    //
    while (cmp < 1000 && posi == NO_LINE)
    {
      if (ancien < 0)
      {
        motors.setSpeeds(SPEED, -SPEED, -1);
      }
      //
      else
      {
        motors.setSpeeds(-SPEED, SPEED, -1);
      }
      posi = getLineSensor();
      cmp++;
      delay(1);
    }
    //
    if (posi == LINE_LEFT || posi == LINE_RIGHT)
    {
      ledrgb.clear();
      return 1;
    }
    //
    else
    {
      posi == NO_LINE;
      while (posi == NO_LINE)
      {
        bot_flip(SPEED, -1);
        posi = getLineSensor();
      }
      //
      ledrgb.clear();
      return 1;
    }
  }
}
//***********************
void bot_rotate_left()
{
  //
  for (int i = 0; i < 10; i++)
  {
    motors.setSpeeds(-SPEED, SPEED, -1); //10 deg
    delay(10);
  }
  //
  motors.stop(0);
}
//*********************
void bot_rotate_right()
{
  //
  for (int i = 0; i < 10; i++)
  {
    motors.setSpeeds(SPEED, -SPEED, -1); //10 deg
    delay(10);
  }
  //
  motors.stop(0);
}
//*********************
void follow()
{
  //
  LineState posi = NO_LINE;

  posi = getLineSensor();

  switch (posi)
  {
    //
    case NO_LINE:
      //
      bot_right(SPEED, 10);

      linefollow++;

      if (linefollow > 5)
      {
        while (cherchePiste() == 0);
        linefollow = 0;
      }
      break;
    //
    case LINE_CENTER:
      bot_front(SPEED, -1);
      break;
    //
    case LINE_RIGHT:
      bot_right(SPEED, -1);
      ancien = 1;
      break;
    //
    case LINE_LEFT:
      bot_left(SPEED, -1);
      ancien = -1;
      break;
    //
    default:
      break;
  }
}
//**************
void setup() {
  Serial.begin(9600);

  led_13.begin();

  ledrgb.clear();
  delay(100);

  buzzer.begin();

  buzzer.sound(& son);
  //
  button.waitForPressAndReleased();

  //buzzer.music(song);
}
//
//**********
void loop() 
{
  distance = ultrasonic.getDictanceCm();
  //
  if (distance >= 8)
  {
    follow();
  }
  else
  {
    eviteMur();
  }
}
//fin
