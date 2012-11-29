/*----------------------------------------------------------------------------*/
/**
* @pkg dda_motors
*/
/**
* DDA step motors controller.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 29.11.2012 10:00:27<br>
* @pkgdoc dda_motors
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_MOTORS_H_1354176027
#define DDA_MOTORS_H_1354176027
/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

typedef enum
{
  Forward = 0,
  Reverse =1
} MotorDirection;

typedef enum
{
  StrengthMotor = 0,
  CasseteMotor = 1
} MotorIndex;


void motors_init();
void motor_start(int motor,int dir);
void motor_deceleration();
void motor_stop();
unsigned motor_step_count();

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_MOTORS_H_1354176027*/

