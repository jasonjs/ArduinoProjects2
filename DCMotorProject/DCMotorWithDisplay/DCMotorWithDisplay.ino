/*
 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe

 */

// include the library code:
#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 4, 6, 5, 3, 2);
int motorSpeed = 0; //current motor speed
int maxSpeed = 0; //max speed allowed (set at control panel).  This is already adjusted by dividing by 4
int goButton = 0;
int stopButton = 0;
int loopsSinceLastButton = 0;
int sleepAfterLoops = 30000; //30000 = 5 minutes when each loop is 10ms
int secondsLeft = 0;
int delayPerNormalLoop = 8; //Adjust this so that the countdown timer is about 1 second long
int slackOutSpeedPercentage = 40; //What percent of max speed should the start up hold at to take slack out of train?
int slackOutSpeedMillis = 1000; //Length of time to allow train to slack out
int startUpSpeedPercentage = 60; //Just to get the motor turning, hit it with this percentage for the number of milliseconds defined below
int startUpSpeedMillis = 100; //Length of time to spike voltage to get motor turning
boolean debugMode = false;

void setup() {
  //setup the motor shield
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT); //stop button light
  pinMode(12, OUTPUT); //go button light

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print("> Jakes RR v0.4 <");
}

void printDebug()
{
  lcd.setCursor(0, 0);
  lcd.print("G ");
  lcd.print(goButton);
  lcd.print(" S ");
  lcd.print(stopButton);
  lcd.print(" M ");
  lcd.print(maxSpeed);
  lcd.print("       ");
  lcd.setCursor(0, 1);
  lcd.print("DEBUG MODE      ");
}

void printSpeed()
{
  float speedPercentage = (float)motorSpeed / (float)256;
  speedPercentage = speedPercentage * 100;
  float maxSpeedPercentage = (float)maxSpeed / (float)256;
  maxSpeedPercentage = maxSpeedPercentage * 100;
  lcd.setCursor(0, 1);
  lcd.print("S ");
  lcd.print((int)speedPercentage);
  lcd.print("% M ");
  lcd.print((int)maxSpeedPercentage);
  lcd.print("% ");
  lcd.print(secondsLeft);
  lcd.print("s      ");
}

void loop() {
  debugMode = false;
  maxSpeed = analogRead(5);
  maxSpeed = maxSpeed / 4;
  goButton = analogRead(4);
  stopButton = analogRead(3);
  secondsLeft = (sleepAfterLoops - loopsSinceLastButton) / 100;
  if (debugMode)
  {
    printDebug();
    delay(delayPerNormalLoop);
  }
  else
  {
    delay(delayPerNormalLoop);
    if (loopsSinceLastButton >= sleepAfterLoops)
    {
      //if it's been too long since a button was pressed, press the stop button
      stopButton = 513;
    }

    if (goButton > 512)
    {
      digitalWrite(10, LOW);
      digitalWrite(12, HIGH);
      runForward();
      loopsSinceLastButton = 0;
    }
    else if (stopButton > 512)
    {
      digitalWrite(12, LOW);
      digitalWrite(10, HIGH);
      stopMotor();
      loopsSinceLastButton = 0;
    }
    printSpeed();
  }
  if (motorSpeed > 0)
  {
    loopsSinceLastButton++;
  }
}

void printString(String s, int line)
{
  lcd.setCursor(0, line);
  if (s.length() > 16)
  {
    //do nothing, the string is too long to add spaces to
  }
  else
  {
    int spacesToAdd = 16 - s.length();
    String spaces = String(' ');
    for (int i = 0; i < spacesToAdd; i++)
    {
      spaces = String(spaces + ' ');
    }
    String toPrint = String(s + spaces);
    lcd.print(toPrint);
  }
}

void setEngineSpeed(int toSpeed)
{
  analogWrite(9, motorSpeed);
  printSpeed();
}

void accelerateTo(int toSpeed)
{
  if(motorSpeed != maxSpeed) //engine is already underway.  go button pressed but already going
  {
    //it's stopped, need to pull out slack in train.  When this is done, train will be moving slowly
    pullOutSlack();
  }
  
  //accelerate to maxSpeed now...
  for ( ; motorSpeed < toSpeed && motorSpeed < maxSpeed; motorSpeed++)
  {
    setEngineSpeed(motorSpeed);
    delay(10);
  }
}

void pullOutSlack()
{
    float slackOutSpeed = (float)maxSpeed * ((float)slackOutSpeedPercentage / 100);
    float startSpikeSpeed = (float)maxSpeed * ((float)startUpSpeedPercentage / 100);
    motorSpeed = (int)startSpikeSpeed;
    setEngineSpeed(motorSpeed);
    delay(startUpSpeedMillis);
    motorSpeed = (int)slackOutSpeed;
    setEngineSpeed(motorSpeed);
    delay(slackOutSpeedMillis);
  
}

void pullOutSlackStairSteps()
{
  //50% of maxSpeed
  float halfMaxSpeed = (float)maxSpeed / (float)2;
  int ihalf = (int)halfMaxSpeed;
  setEngineSpeed(20);
  delay(1000);
  setEngineSpeed(40);
  delay(1000);
  for ( ; motorSpeed < (int)ihalf; motorSpeed+=30)
  {
    //setEngineSpeed(motorSpeed);
    //delay(1000);
  }
  
}

void decelerateTo(int toSpeed)
{
  for ( ; motorSpeed > toSpeed; motorSpeed--)
  {
    setEngineSpeed(motorSpeed);
    if (motorSpeed < 40) //slow the rate of deceleration
    {
      delay(60);
    }
    else
    {
      delay(20);
    }
    printSpeed();
  }
}


void runForward()
{
  printString("GO FORWARD!", 0);
  digitalWrite(11, LOW); /*MOROTSHIELD_IN2 = 11 */
  digitalWrite(8, HIGH); /*MOROTSHIELD_IN1 = 8 */
  accelerateTo(256);
  printString("CHOO CHOO!", 0);
}

void runBackward()
{
  digitalWrite(8, LOW); /*MOROTSHIELD_IN1 = 8 */
  digitalWrite(11, HIGH); /*MOROTSHIELD_IN1 = 8 */
  accelerateTo(256);
}

void stopMotor()
{
  printString("STOP!", 0);
  decelerateTo(0);
  digitalWrite(8, LOW); /*MOROTSHIELD_IN1 = 8 */
  digitalWrite(11, LOW); /*MOROTSHIELD_IN1 = 8 */
  printString("STOPPED", 0);
}


