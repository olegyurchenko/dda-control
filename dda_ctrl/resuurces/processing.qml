import Qt 4.7
import DDA 1.0

import "template/processing.js" as Functions

DDAExtension {
  version: "DDA extension"
  revision: 101
  property int tag: 0


  //-----------------------------------------------------------------------------------
  function modelInit()
  {
    //print("DDAExtension.modeInit()")
    Functions.modelInit()
  }
  //-----------------------------------------------------------------------------------
  function modelUpdate(session)
  {
    Functions.modelUpdate(session)
  }

  //-----------------------------------------------------------------------------------
  //Component.onCompleted: {
  //}
}
