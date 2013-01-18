/*----------------------------------------------------------------------------*/
/**
* @pkg dda_db
*/
/**
* Data storage & handle masure data.
*
* (C) DDA team, Kiev, Ukraine 2013.<br>
* started 03.01.2013 10:03:37<br>
* @pkgdoc dda_db
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_DB_H_1357200217
#define DDA_DB_H_1357200217
/*----------------------------------------------------------------------------*/
#include <decimal.h>

/*----------------------------------------------------------------------------*/
typedef struct
{
  int cell;
  decimal32_t size;
  decimal32_t strength;
} measure_t;
/*----------------------------------------------------------------------------*/
typedef struct
{
  int grit_index;
  int particles;
} session_t;
/*----------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

void db_new_session(int grit_index, int particles);
void db_current_session(session_t *dst);
void db_add_measure(const measure_t* measure);
int db_measure_count();
void db_measure(int index, measure_t* dst);



#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_DB_H_1357200217*/

