print("Procession Ok !!!")
//-----------------------------------------------------------------------------------
function displayData(label, data)
{
  var str = label + ":"
  for (var prop in data)
    str = str + prop + "=" + data[prop] + ";"
  console.log(str)
}
//-----------------------------------------------------------------------------------
var gistogrammTable = null

//-----------------------------------------------------------------------------------
var paramTable = null
var nameCol = 0
var valueCol = 1
var serialRow = 0
var dateRow = 1
var userRow = 2
var lotRow = 3
var gritRow = 4
var particlesRow = 5
var avgStretchRow = 6
var devStrengthRow = 7
var maxStrengthRow = 8
var minStrengthRow = 9
var avgSizeRow = 10
var devSizeRow = 11
var avgFractStrRow = 12
var devFractStrRow = 13
var markRow = 14
//-----------------------------------------------------------------------------------
function modelInit()
{
  print("Functions.modelInit()")
  paramTable = model.newTableModel()

  paramTable.columnCount = 2
  paramTable.rowCount = 15
  paramTable.setHeaderData(0, Qt.Horizontal, qsTr("Name"))
  paramTable.setHeaderData(1, Qt.Horizontal, qsTr("Value"))
  paramTable.setData(serialRow, nameCol, qsTr("Device DDA-33"))
  paramTable.setData(dateRow, nameCol, qsTr("Measure date"))
  paramTable.setData(userRow, nameCol, qsTr("User name"))
  paramTable.setData(lotRow, nameCol, qsTr("Lot"))
  paramTable.setData(gritRow, nameCol, qsTr("Grit size"))
  paramTable.setData(particlesRow, nameCol, qsTr("Particles"))
  paramTable.setData(avgStretchRow, nameCol, qsTr("Avg. strength [N]"))
  paramTable.setData(devStrengthRow, nameCol, qsTr("St. dev. strength [N]"))
  paramTable.setData(maxStrengthRow, nameCol, qsTr("Max strength [N]"))
  paramTable.setData(minStrengthRow, nameCol, qsTr("Min strength [N]"))
  paramTable.setData(avgSizeRow, nameCol, qsTr("Avg. size [um]"))
  paramTable.setData(devSizeRow, nameCol, qsTr("St. dev. size [um]"))
  paramTable.setData(avgFractStrRow, nameCol, qsTr("Avg. fract str [N/mm^2]"))
  paramTable.setData(devFractStrRow, nameCol, qsTr("St. fract str [N/mm^2]"))
  paramTable.setData(markRow, nameCol, qsTr("Set mark"))
  paramTable.update()
}
//-----------------------------------------------------------------------------------
function modelUpdate(session)
{
//  displayData("session", session);
  paramTable.setData(serialRow, valueCol, session.deviceSerial)
  paramTable.setData(dateRow, valueCol, Qt.formatDate(session.start, qsTr("yyyy-MM-dd")))
  paramTable.setData(userRow, valueCol, session.userName)
  paramTable.setData(lotRow, valueCol, session.lot)
  paramTable.setData(gritRow, valueCol, session.grit)

  statistic(session)

  //paramTable.setData(particlesRow, valueCol, session.measures.length)
  paramTable.setData(particlesRow, valueCol, session.particles)
  paramTable.setData(avgStretchRow, valueCol, session.avgStrength)
  paramTable.setData(devStrengthRow, valueCol, session.devStrength)
  paramTable.setData(maxStrengthRow, valueCol, session.maxStrength)
  paramTable.setData(minStrengthRow, valueCol, session.minStrength)
  paramTable.setData(avgSizeRow, valueCol, session.avgSize)
  paramTable.setData(devSizeRow, valueCol, session.devSize)
  paramTable.setData(avgFractStrRow, valueCol, session.avgFractStr)
  paramTable.setData(devFractStrRow, valueCol, session.devFractStr)
  paramTable.setData(markRow, valueCol, session.mark)
}
//-----------------------------------------------------------------------------------
function statistic(s)
{
  var minStrength = 9999999999
  var maxStrength = -1
  var avgStrength = 0
  var devStrength = 0

  var minSize = 9999999999
  var maxSize = -1
  var avgSize = 0
  var devSize = 0

  var minFractStr = 9999999999
  var maxFractStr = -1
  var avgFractStr = 0
  var devFractStr = 0

  var count = 0
  for(var i = 0; i < s.measures.length; i++)
  {
    var m = s.measures[i]
    if(m.ignored)
      continue

    var val = m.strength
    //displayData("measure:", m)
    if(minStrength > val)
      minStrength = val
    if(maxStrength < val)
      maxStrength = val
    avgStrength += val

    val = m.size
    if(minSize > val)
      minSize = val
    if(maxSize < val)
      maxSize = val
    avgSize += val

    if(m.size > 0.01)
    {
      val = 10000 * m.strength / (m.size * m.size) //~~~ !!! ATTENTION - this is not true !!! ---
      if(minFractStr > val)
        minFractStr = val
      if(maxFractStr < val)
        maxFractStr = val
      avgFractStr += val
    }

    count ++
  }

  print(count)

  if(count > 1)
  {
    avgStrength /= count
    avgSize /= count
    avgFractStr /= count
    for(i = 0; i < s.measures.length; i++)
    {
      m = s.measures[i]
      if(m.ignored)
        continue

      //print(m.strength, avgStrength, devStrength)
      devStrength += Math.pow(m.strength - avgStrength, 2)
      devSize += Math.pow(m.size - avgSize, 2)
      if(m.size > 0.01)
        devFractStr += Math.pow(10000 * m.strength / (m.size * m.size) - avgFractStr, 2) //~~~ !!! ATTENTION - this is not true !!! ---
    }

    devStrength = Math.sqrt(devStrength / (count - 1))
    devSize = Math.sqrt(devSize / (count - 1))
    devFractStr = Math.sqrt(devFractStr / (count - 1))
  }

  s["minStrength"] = minStrength.toFixed(2)
  s["maxStrength"] = maxStrength.toFixed(2)
  s["avgStrength"] = avgStrength.toFixed(2)
  s["devStrength"] = devStrength.toFixed(2)

  s["minSize"] = minSize.toFixed(2)
  s["maxSize"] = maxSize.toFixed(2)
  s["avgSize"] = avgSize.toFixed(2)
  s["devSize"] = devSize.toFixed(2)

  s["minFractStr"] = minFractStr.toFixed(2)
  s["maxFractStr"] = maxFractStr.toFixed(2)
  s["avgFractStr"] = avgFractStr.toFixed(2)
  s["devFractStr"] = devFractStr.toFixed(2)

  s["particles"] = count
}
//-----------------------------------------------------------------------------------
