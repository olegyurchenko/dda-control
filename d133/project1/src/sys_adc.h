/*----------------------------------------------------------------------------*/
/**
* @pkg sys_adc
*/
/**
* st32f107 ADC functions.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 05.09.2012  8:53:48<br>
* @pkgdoc sys_adc
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef SYS_ADC_H_1346824428
#define SYS_ADC_H_1346824428
/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void sys_adc_init();
void sys_adc_start_conversion();
int sys_adc_get_value(int *dst);

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*SYS_ADC_H_1346824428*/

