import Qt 4.7
import DDA 1.0

import "processing.js" as Functions

DDAExtension {
  version: "DDA extension"
  revision: 101
  property int tag: 0


  //-----------------------------------------------------------------------------------
  function displayData(label, data)
  {
    var str = label + ":"
    for (var prop in data)
      str = str + prop + "=" + data[prop] + ";"
    console.log(str)
  }
  //-----------------------------------------------------------------------------------

  Component.onCompleted: {
      print("Component.onCompleted")
      print(session)
      print(currentDir)
      print(database)
      displayData("processing", Functions)
  }
}
