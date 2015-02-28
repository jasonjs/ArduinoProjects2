//  Demo function:The application method to drive the DC motor.
//  Author:Frankie.Chu
//  Date:20 November, 2012

#include "MotorDriver.h"


int motorSpeed = 0;

void setup()
{
      pinMode(8, OUTPUT);
      pinMode(9, OUTPUT);
      pinMode(11, OUTPUT);
/*
	motordriver.init();
	motordriver.setSpeed(500,MOTORA);
*/
}

void accelerateTo(int toSpeed)
{
  for( ; motorSpeed < toSpeed; motorSpeed++)
  {
    analogWrite(9,motorSpeed);
    delay(10);
  }
}

void decelerateTo(int toSpeed)
{
  for( ; motorSpeed > toSpeed; motorSpeed--)
  {
    analogWrite(9,motorSpeed);
    delay(10);
  }
}

 
void runForward()
{
  digitalWrite(11,LOW); /*MOROTSHIELD_IN2 = 11 */
  digitalWrite(8,HIGH); /*MOROTSHIELD_IN1 = 8 */
  accelerateTo(256);
}

void runBackward()
{
  digitalWrite(8,LOW); /*MOROTSHIELD_IN1 = 8 */
  digitalWrite(11,HIGH); /*MOROTSHIELD_IN1 = 8 */
  accelerateTo(256);
}

void stopMotor()
{
  decelerateTo(0);
  digitalWrite(8,LOW); /*MOROTSHIELD_IN1 = 8 */
  digitalWrite(11,LOW); /*MOROTSHIELD_IN1 = 8 */
}

void loop()
{
  runForward();
  delay(2000);
  stopMotor();
  delay(3000);
  runBackward();
  delay (4500);
  stopMotor();
  delay (3000);
}
