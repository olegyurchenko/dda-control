/*----------------------------------------------------------------------------*/
/**
* @pkg dda_mesh
*/
/**
* Mesh handle.
*
* (C) DDA team, Kiev, Ukraine 2012.<br>
* started 25.12.2012  9:12:03<br>
* @pkgdoc dda_mesh
* @author oleg
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
#ifndef DDA_MESH_H_1356419523
#define DDA_MESH_H_1356419523
#include <menu.h>
#include <decimal.h>
/*----------------------------------------------------------------------------*/
typedef struct
{
  const char *caption;
  int max;
  int min;
} mesh_t;
/*----------------------------------------------------------------------------*/
#define INVALID_MESH_INDEX (-1)
/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

int mesh_index();
void set_mesh_index(int index);
const mesh_t* mesh();
void init_mesh_menu(MENU_ITEM *root);
const decimal32_t* relative_size_deviation();

#ifdef __cplusplus
} //extern "C"
#endif /*__cplusplus*/
/*----------------------------------------------------------------------------*/
#endif /*DDA_MESH_H_1356419523*/

