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
#include "dda_motors.h"
#include <stm32f10x.h>
#define USE_CONSOLE //!!!!
#ifdef USE_CONSOLE
#include <console.h>
#endif //USE_CONSOLE
#include <dda_clib.h>
/*----------------------------------------------------------------------------*/
#define STP1_EN     GPIO_Pin_10 //PB10
#define STP1_DIR    GPIO_Pin_11 //PB11
#define STP1_STEP   GPIO_Pin_12 //PB12
#define STP1_RS     GPIO_Pin_6  //PC6

#define STP2_EN     GPIO_Pin_7  //PC7
#define STP2_DIR    GPIO_Pin_8  //PC8
#define STP2_STEP   GPIO_Pin_9  //PC9
#define STP2_RS     GPIO_Pin_8  //PA8

/*----------------------------------------------------------------------------*/
const unsigned char step_table[256]=
{
0xFF, 0x69, 0x51, 0x44, 0x3C, 0x36, 0x32, 0x2E,
0x2B, 0x29, 0x27, 0x25, 0x24, 0x22, 0x21, 0x20,
0x1F, 0x1E, 0x1D, 0x1C, 0x1C, 0x1B, 0x1A, 0x1A,
0x19, 0x19, 0x18, 0x18, 0x17, 0x17, 0x17, 0x16,
0x16, 0x16, 0x15, 0x15, 0x15, 0x14, 0x14, 0x14,
0x14, 0x13, 0x13, 0x13, 0x13, 0x12, 0x12, 0x12,
0x12, 0x12, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11,
0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E, 0x0E,
0x0E, 0x0E, 0x0E, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D,
0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D, 0x0D,
0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
0x0C, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0A, 0x0A,
0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A,
0x0A, 0x0A, 0x0A, 0x09, 0x09, 0x09, 0x09, 0x09,
0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09,
0x09, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x07, 0x07
};
/*----------------------------------------------------------------------------*/
#define STP1_MIN_PERIOD 800 //300//us
#define STP2_MIN_PERIOD 200 //us
#define STOPPAGE_TIME 30000 //us
/*----------------------------------------------------------------------------*/
#define ACCELERATION_STEP 4
#define DECELERATION_STEP 16
#define CHANGE_RATE_STEP 1
/*----------------------------------------------------------------------------*/
static int active_motor = -1;
static volatile int32_t table_index;
static volatile int32_t max_index;
static enum MotorState
{
  Idle,
  Accelerate,
  Decelerate,
  Slewing,
  ChangeRate,
  Stoppage
} motor_state = Idle;
static volatile uint32_t step_counter = 0;
static volatile uint16_t min_period;
/*----------------------------------------------------------------------------*/
#ifdef USE_CONSOLE
static CONSOLE_CMD acc_data, dec_data, stop_data, step_data;
static int acc_cmd(int argc, char **argv)
{
  int mot = 0, dir = 0, rate = 0;
  if(argc > 1)
    mot = str2int(argv[1]);
  if(argc > 2)
    dir = str2int(argv[2]);
  if(argc > 3)
    rate = str2int(argv[3]);
  motor_start(mot, dir, rate);
  return CONSOLE_OK;
}
/*----------------------------------------------------------------------------*/
static int dec_cmd(int argc, char **argv)
{
  motor_deceleration();
  return CONSOLE_OK;
}
/*----------------------------------------------------------------------------*/
static int stop_cmd(int argc, char **argv)
{
  motor_stop();
  return CONSOLE_OK;
}
/*----------------------------------------------------------------------------*/
static int steps_cmd(int argc, char **argv)
{
  const char *state = "????";
  uint16_t counter;

  counter = min_period;
  counter *= step_table[table_index];
  switch(motor_state)
  {
  case Idle:
    state = "Iddle";
    break;
  case Accelerate:
    state = "Accelerate";
    break;
  case Decelerate:
    state = "Decelerate";
    break;
  case Slewing:
    state = "Slewing";
    break;
  case ChangeRate:
    state = "ChangeRate";
    break;
  case Stoppage:
    state = "Stoppage";
    break;
  }

  console_printf("State:'%s', Table index:%u Period:%u, Step counter:%u\n",
                 state,
                 (unsigned)table_index,
                 (unsigned)counter,
                 motor_step_count());
  return CONSOLE_OK;
}
/*----------------------------------------------------------------------------*/
static void console_motors_init()
{
  console_cmd_init(&acc_data);
  acc_data.cmd = "acc";
  acc_data.help = "acc [motor [direction]] - Accelerate step motor start";
  acc_data.handler = acc_cmd;
  console_add_cmd(&acc_data);

  console_cmd_init(&dec_data);
  dec_data.cmd = "dec";
  dec_data.help = "dec - Decelerate step motor start";
  dec_data.handler = dec_cmd;
  console_add_cmd(&dec_data);

  console_cmd_init(&stop_data);
  stop_data.cmd = "stp";
  stop_data.help = "stp - Stop step motor";
  stop_data.handler = stop_cmd;
  console_add_cmd(&stop_data);

  console_cmd_init(&step_data);
  step_data.cmd = "pst";
  step_data.help = "pst - Print step motor state & count";
  step_data.handler = steps_cmd;
  console_add_cmd(&step_data);
}

#endif
/*----------------------------------------------------------------------------*/
static void set_enable(int on)
{
  switch(active_motor)
  {
  case 0:
    if(on)
      GPIO_SetBits(GPIOB, STP1_EN);
    else
      GPIO_ResetBits(GPIOB, STP1_EN);
    break;
  case 1:
    if(on)
      GPIO_SetBits(GPIOC, STP2_EN);
    else
      GPIO_ResetBits(GPIOC, STP2_EN);
    break;
  }
}
/*----------------------------------------------------------------------------*/
static void set_dir(int on)
{
  switch(active_motor)
  {
  case 0:
    if(on)
      GPIO_SetBits(GPIOB, STP1_DIR);
    else
      GPIO_ResetBits(GPIOB, STP1_DIR);
    break;
  case 1:
    if(on)
      GPIO_SetBits(GPIOC, STP2_DIR);
    else
      GPIO_ResetBits(GPIOC, STP2_DIR);
    break;
  }
}
/*----------------------------------------------------------------------------*/
static void set_rs(int on)
{
  switch(active_motor)
  {
  case 0:
    if(on)
      GPIO_SetBits(GPIOC, STP1_RS);
    else
      GPIO_ResetBits(GPIOC, STP1_RS);
    break;
  case 1:
    if(on)
      GPIO_SetBits(GPIOA, STP2_RS);
    else
      GPIO_ResetBits(GPIOA, STP2_RS);
    break;
  }
}
/*----------------------------------------------------------------------------*/
static void step()
{
  switch(active_motor)
  {
  case 0:
    GPIO_Write(GPIOB, GPIO_ReadOutputData(GPIOB) ^ STP1_STEP);
    break;
  case 1:
    GPIO_Write(GPIOC, GPIO_ReadOutputData(GPIOC) ^ STP2_STEP);
    break;
  }
}
/*----------------------------------------------------------------------------*/
void motors_init()
{
  GPIO_InitTypeDef gpio;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);


  gpio.GPIO_Pin = STP1_EN | STP1_DIR | STP1_STEP;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &gpio);

  gpio.GPIO_Pin = STP1_RS | STP2_EN | STP2_DIR | STP2_STEP;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &gpio);

  gpio.GPIO_Pin = STP2_RS;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &gpio);

  //Set enable to inactive state
  GPIO_SetBits(GPIOB, STP1_EN);
  GPIO_SetBits(GPIOC, STP2_EN);

  //Set reset to inactive state
  GPIO_SetBits(GPIOC, STP1_RS);
  GPIO_SetBits(GPIOA, STP2_RS);

#ifdef USE_CONSOLE
  console_motors_init();
#endif
}
/*----------------------------------------------------------------------------*/
static void start_timer()
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

  TIM_TimeBaseInitTypeDef base_timer;
  TIM_TimeBaseStructInit(&base_timer);

  base_timer.TIM_Prescaler = (HSE_VALUE / 1000000) - 1; //1 MHz
  base_timer.TIM_Period = 1000; //1 ms
  TIM_TimeBaseInit(TIM6, &base_timer);

  TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
  TIM_Cmd(TIM6, ENABLE);

  NVIC_EnableIRQ(TIM6_IRQn);
}
/*----------------------------------------------------------------------------*/
void TIM6_IRQHandler()
{
  if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    uint16_t counter;
    counter = min_period;
    switch(motor_state)
    {
    case Idle:
      //Set reset to inactive state
      set_rs(1);
      motor_state = Accelerate;
      table_index = 0;
    case Accelerate:
    case Decelerate:
    case Slewing:
    case ChangeRate:
      step();
      ++ step_counter;
      if(!(step_counter & 1))
      {
        counter *= step_table[table_index];
        //counter *= 256 - table_index;
        //TIM_SetCounter(TIM6, counter);
        TIM_SetAutoreload(TIM6, counter);
        if(motor_state == Accelerate)
        {
          table_index += ACCELERATION_STEP;
          if(table_index >= max_index)
          {
            motor_state = Slewing;
            table_index = max_index;
          }
        }
        else
        if(motor_state == Decelerate)
        {
          if(table_index <= 0)
          {
            motor_state = Stoppage;
            TIM_SetAutoreload(TIM6, STOPPAGE_TIME);
          }
          else
          {
            table_index -= DECELERATION_STEP;
            if(table_index < 0)
              table_index = 0;
          }
        }
        else
        if(motor_state == ChangeRate)
        {
          if(table_index < max_index)
          {
            table_index += CHANGE_RATE_STEP;
            if(table_index >= max_index)
            {
              motor_state = Slewing;
              table_index = max_index;
            }
          }
          else
          {
            table_index -= CHANGE_RATE_STEP;
            if(table_index <= max_index)
            {
              motor_state = Slewing;
              table_index = max_index;
            }
          }
        }
      } //if(!(step_counter & 1))
      break;
    case Stoppage:
      set_enable(1); //Enable to inactive state
      motor_state = Idle;
      TIM_Cmd(TIM6, DISABLE);
      NVIC_DisableIRQ(TIM6_IRQn);
      break;

    }
  }
}
/*----------------------------------------------------------------------------*/
unsigned char motor_rate()
{
  return (unsigned char) max_index;
}
/*----------------------------------------------------------------------------*/
void motor_start(int mr, int dir, unsigned char rate)
{
  if(mr < 0
     || mr > 1
     || motor_state != Idle)
    return;
  active_motor = mr;
  set_dir(dir);
  step_counter = 0;
  set_enable(0); //Enable to active state
  set_rs(0); //Reset to active state
  if(!mr)
    min_period = STP1_MIN_PERIOD / 7; //7 - minimal multiplier from step_table
  else
    min_period = STP2_MIN_PERIOD / 7; //7 - minimal multiplier from step_table
  table_index = 0;
  if(!rate)
    max_index = 255;
  else
    max_index = rate;
  start_timer();
}
/*----------------------------------------------------------------------------*/
void motor_deceleration()
{
  if(motor_state == Accelerate || motor_state == Slewing || motor_state == ChangeRate)
    motor_state = Decelerate;
}
/*----------------------------------------------------------------------------*/
void motor_change_rate(unsigned char rate)
{
  if(motor_state == Accelerate || motor_state == Slewing || motor_state == ChangeRate)
  {
    if(!rate)
      max_index = 255;
    else
      max_index = rate;

    motor_state = ChangeRate;
  }
}
/*----------------------------------------------------------------------------*/
void motor_stop()
{
  switch(motor_state)
  {
  case Idle:
  case Stoppage:
    break;
  case Accelerate:
  case Decelerate:
  case ChangeRate:
  case Slewing:
    motor_state = Decelerate;
    table_index = DECELERATION_STEP;
    break;
  }
}
/*----------------------------------------------------------------------------*/
unsigned motor_step_count()
{
  return step_counter / 2;
}
/*----------------------------------------------------------------------------*/
int is_motor_idle()
{
  return motor_state == Idle;
}
/*----------------------------------------------------------------------------*/
