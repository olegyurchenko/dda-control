/*----------------------------------------------------------------------------*/
/**
* @pkg dda_work_mode
*/
/**
* DDA work mode.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 20.12.2012 10:27:00<br>
* @pkgdoc dda_work_mode
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_WORK_MODE_H_1355992020
#define DDA_WORK_MODE_H_1355992020
/*----------------------------------------------------------------------------*/
typedef enum WORK_MODE
{
  UnknownMode,
  AutoMode,
  ManualMode
} work_mode_t;

typedef enum
{
  Idle,
  Calibration,
  Measuring,
  NexCasseteWait,
  Done
} work_state_t;


#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void work_mode_init();
void set_work_mode(work_mode_t m);
work_mode_t work_mode();
void start_work_menu();
work_state_t work_state();

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_WORK_MODE_H_1355992020*/

