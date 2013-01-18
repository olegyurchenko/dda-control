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
#include "dda_db.h"
#include <dda_config.h>
/*----------------------------------------------------------------------------*/
static int measure_count = 0;
static measure_t measures[CASSETTE_MAX_CELL];
static session_t current_session;

void db_new_session(int grit_index, int particles)
{
  current_session.grit_index = grit_index;
  current_session.particles = particles;
  measure_count = 0;
}
/*----------------------------------------------------------------------------*/
void db_current_session(session_t *dst)
{
  dst = &current_session;
}
/*----------------------------------------------------------------------------*/
void db_add_measure(const measure_t* measure)
{
  if((unsigned)measure_count < sizeof(measures)/sizeof(measures[0]))
  {
    measures[measure_count] = *measure;
    measure_count++;
  }
}
/*----------------------------------------------------------------------------*/
int db_measure_count()
{
  return measure_count;
}
/*----------------------------------------------------------------------------*/
void db_measure(int index, measure_t* dst)
{
  if((unsigned)index < sizeof(measures)/sizeof(measures[0]) && index < measure_count)
    *dst = measures[index];
}
/*----------------------------------------------------------------------------*/

