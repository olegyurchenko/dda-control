/*----------------------------------------------------------------------------*/
/**
* @pkg dda_result_view
*/
/**
* View last session result.
*
* (C) DDA team, Kiev, Ukraine 2013.<br>
* started 18.01.2013 11:15:42<br>
* @pkgdoc dda_result_view
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#include "dda_result_view.h"
#include <view.h>
#include <dda_mesh.h>
#include <dda_db.h>
#include <dda_text.h>
#include <dda_clib.h>
#include <dda_mode.h>
/*----------------------------------------------------------------------------*/
enum ReportString
{
  DeviceString = 0,
  MeshString,
  ParticlesString,
  AvgStrengthString,
  StdDevStrengthString,
  MaxStrnegthString,
  MinStrengthString,
  AvgSizeStringth,
  StdDevSizeString,

  StatisticStringCount
};
/*----------------------------------------------------------------------------*/
static void result_get(void *data, int index, char **dst, SCR_ALIGN *align);
/*----------------------------------------------------------------------------*/
void view_result()
{
  viewer(result_get, 0, StatisticStringCount + 1 + db_measure_count());
}
/*----------------------------------------------------------------------------*/
static void avg_strneght(decimal32_t *dst)
{
  measure_t measure;
  decimal32_t avg;
  decimal32_t cnt;
  int count, i;

  *dst = decimal32_init(0, 0);
  count = db_measure_count();
  if(!count)
    return;
  cnt = decimal32_init(count, 0);
  avg = decimal32_init(0, 0);
  for(i = 0; i < count; i++)
  {
    db_measure(i, &measure);
    decimal32_add(&avg, &measure.strength, &avg);
  }
  decimal32_div(&avg, &cnt, &avg);
  decimal32_math_round(&avg, 1, dst);
}
/*----------------------------------------------------------------------------*/
static void std_dev_strength(decimal32_t *dst)
{
  measure_t measure;
  decimal32_t avg;
  decimal32_t dev;
  decimal32_t cnt;
  int count, i;

  *dst = decimal32_init(0, 0);
  count = db_measure_count();
  if(!count)
    return;
  cnt = decimal32_init(count, 0);
  avg_strneght(&avg);
  dev = decimal32_init(0, 0);
  for(i = 0; i < count; i++)
  {
    decimal32_t d;
    db_measure(i, &measure);
    decimal32_sub(&avg, &measure.strength, &d);
    decimal32_abs(&d, &d);
    decimal32_add(&dev, &d, &dev);
  }
  decimal32_div(&dev, &cnt, &dev);
  decimal32_math_round(&dev, 1, dst);
}
/*----------------------------------------------------------------------------*/
static void minmax_strength(decimal32_t *min, decimal32_t *max)
{
  measure_t measure;
  int count, i;

  if(min != 0)
    *min = decimal32_init(0, 0);
  if(max != 0)
    *max = decimal32_init(0, 0);
  count = db_measure_count();
  if(!count)
    return;

  if(min != 0)
    *min = decimal32_init(0x7fffffff, 2);
  if(max != 0)
    *max = decimal32_init(-1 * 0x7ffffffe, 2);

  for(i = 0; i < count; i++)
  {
    db_measure(i, &measure);
    if(min != 0 && decimal32_cmp(min, &measure.strength) > 0)
      *min = measure.strength;
    if(max != 0 && decimal32_cmp(max, &measure.strength) < 0)
      *max = measure.strength;

  }
}
/*----------------------------------------------------------------------------*/
static void avg_size(decimal32_t *dst)
{
  measure_t measure;
  decimal32_t avg;
  decimal32_t cnt;
  int count, i;

  *dst = decimal32_init(0, 0);
  count = db_measure_count();
  if(!count)
    return;
  cnt = decimal32_init(count, 0);
  avg = decimal32_init(0, 0);
  for(i = 0; i < count; i++)
  {
    db_measure(i, &measure);
    decimal32_add(&avg, &measure.size, &avg);
  }
  decimal32_div(&avg, &cnt, &avg);
  decimal32_math_round(&avg, 1, dst);
}
/*----------------------------------------------------------------------------*/
static void std_dev_size(decimal32_t *dst)
{
  measure_t measure;
  decimal32_t avg;
  decimal32_t dev;
  decimal32_t cnt;
  int count, i;

  *dst = decimal32_init(0, 0);
  count = db_measure_count();
  if(!count)
    return;
  cnt = decimal32_init(count, 0);
  avg_size(&avg);
  dev = decimal32_init(0, 0);
  for(i = 0; i < count; i++)
  {
    decimal32_t d;
    db_measure(i, &measure);
    decimal32_sub(&avg, &measure.size, &d);
    decimal32_abs(&d, &d);
    decimal32_add(&dev, &d, &dev);
  }
  decimal32_div(&dev, &cnt, &dev);
  decimal32_math_round(&dev, 1, dst);
}
/*----------------------------------------------------------------------------*/
static void result_get(void *data, int index, char **dst, SCR_ALIGN *align)
{
  static char buffer[64], buf[16];

  (void)data;
  buffer[0] = '\0';
  *dst = buffer;
  *align = SCR_ALIGN_LEFT;
  switch(index)
  {
  case DeviceString:
    snprintf(buffer, sizeof(buffer), "%-9s%6s", get_text(STR_DEVICE), device_serial_str());
    break;
  case MeshString:
    snprintf(buffer, sizeof(buffer), "%-9s%6s", get_text(STR_GRIT), mesh()->caption);
    break;
  case ParticlesString:
    snprintf(buffer, sizeof(buffer), "%-9s%6d", get_text(STR_SAMPLES), db_measure_count());
    break;
  case AvgStrengthString:
  {
    decimal32_t avg;
    avg_strneght(&avg);
    decimal32_str(&avg, buf, sizeof(buf));
    snprintf(buffer, sizeof(buffer), "%-7s%8s", get_text(STR_AVG_STRENGTH), buf);
  }
    break;
  case StdDevStrengthString:
  {
    decimal32_t avg;
    std_dev_strength(&avg);
    decimal32_str(&avg, buf, sizeof(buf));
    snprintf(buffer, sizeof(buffer), "%-7s%8s", get_text(STR_ST_DEV_STRENGTH), buf);
  }
    break;
  case MaxStrnegthString:
  {
    decimal32_t m;
    minmax_strength(0, &m);
    decimal32_str(&m, buf, sizeof(buf));
    snprintf(buffer, sizeof(buffer), "%-7s%8s", get_text(STR_MAX_STRENGTH), buf);
  }
    break;
  case MinStrengthString:
  {
    decimal32_t m;
    minmax_strength(&m, 0);
    decimal32_str(&m, buf, sizeof(buf));
    snprintf(buffer, sizeof(buffer), "%-7s%8s", get_text(STR_MIN_STRENGTH), buf);
  }
    break;
  case AvgSizeStringth:
  {
    decimal32_t avg;
    avg_size(&avg);
    decimal32_str(&avg, buf, sizeof(buf));
    snprintf(buffer, sizeof(buffer), "%-7s%8s", get_text(STR_AVG_SIZE), buf);
  }
    break;
  case StdDevSizeString:
  {
    decimal32_t avg;
    std_dev_size(&avg);
    decimal32_str(&avg, buf, sizeof(buf));
    snprintf(buffer, sizeof(buffer), "%-7s%8s", get_text(STR_ST_DEV_SIZE), buf);
  }
    break;
  case StatisticStringCount:
    snprintf(buffer, sizeof(buffer), "%s", get_text(STR_MEASUREMENTS));
    *align = SCR_ALIGN_CENTER;
    break;

  default:
  {
    measure_t measure;

    db_measure(index - StatisticStringCount - 1, &measure);
    decimal32_str(&measure.strength, buf, sizeof(buf));
    index = snprintf(buffer, sizeof(buffer), "%02d %5s ", index - StatisticStringCount, buf);
    decimal32_str(&measure.size, buf, sizeof(buf));
    snprintf(&buffer[index], sizeof(buffer) - index, "%s", buf);
  }
    break;
  }
}
/*----------------------------------------------------------------------------*/
