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
var paramTable = null
var nameCol = 0
var valueCol = 1
var row = 0
var serialRow = row ++
var dateRow = row ++
//var userRow = row ++
var lotRow = row ++
var gritRow = row ++
var particlesRow = row ++
var avgStretchRow = row ++
var devStrengthRow = row ++
var maxStrengthRow = row ++
var minStrengthRow = row ++
var avgSizeRow = row ++
var devSizeRow = row ++
var avgFractStrRow = row ++
var devFractStrRow = row ++
var markRow = row ++
//-----------------------------------------------------------------------------------
var histStrengthTable = null
var histSizeTable = null
//-----------------------------------------------------------------------------------
var strengthHistogramm = null
var sizeHistogramm = null
//-----------------------------------------------------------------------------------
var strengthCurve = null
var fractStrengthGraph = null
//-----------------------------------------------------------------------------------
var histogrammColumns = 10
//-----------------------------------------------------------------------------------
function modelInit(role)
{
  // paramTable
  //--------------------------------
  paramTable = model.newTableModel()
  paramTable.columnCount = 2
  paramTable.rowCount = markRow + 1
  paramTable.setHeaderData(0, Qt.Horizontal, qsTr("Name"))
  paramTable.setHeaderData(1, Qt.Horizontal, qsTr("Value"))
  paramTable.setData(serialRow, nameCol, qsTr("Device DDA-33"))
  paramTable.setData(dateRow, nameCol, qsTr("Measure date"))
  //paramTable.setData(userRow, nameCol, qsTr("User name"))
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

  //--------------------------------
  // histStrengthTable
  //--------------------------------
  histStrengthTable = model.newTableModel()
  histStrengthTable.columnCount = 4
  histStrengthTable.rowCount = histogrammColumns
  histStrengthTable.setHeaderData(0, Qt.Horizontal, qsTr(">="))
  histStrengthTable.setHeaderData(1, Qt.Horizontal, qsTr("<"))
  histStrengthTable.setHeaderData(2, Qt.Horizontal, qsTr("Particles"))
  histStrengthTable.setHeaderData(3, Qt.Horizontal, qsTr("% of grain"))
  for(var i = 0; i < histogrammColumns; i++)
    histStrengthTable.setHeaderData(i, Qt.Vertical, i + 1)
  histStrengthTable.update()
  //--------------------------------
  // strengthHistogramm
  //--------------------------------
  strengthHistogramm = model.newHistogrammModel()
  strengthHistogramm.y.min = 0
  strengthHistogramm.y.max = 100
  strengthHistogramm.y.text = "[%]"
  strengthHistogramm.y.decimals = 0
  strengthHistogramm.y.steps = 4

  strengthHistogramm.x.text = "[N]"
  strengthHistogramm.x.decimals = 1
  strengthHistogramm.x.steps = histogrammColumns
  //--------------------------------
  // histSizeTable
  //--------------------------------
  histSizeTable = model.newTableModel()
  histSizeTable.columnCount = 4
  histSizeTable.rowCount = histogrammColumns
  histSizeTable.setHeaderData(0, Qt.Horizontal, qsTr(">="))
  histSizeTable.setHeaderData(1, Qt.Horizontal, qsTr("<"))
  histSizeTable.setHeaderData(2, Qt.Horizontal, qsTr("Particles"))
  histSizeTable.setHeaderData(3, Qt.Horizontal, qsTr("% of grain"))
  for(var i = 0; i < histogrammColumns; i++)
    histSizeTable.setHeaderData(i, Qt.Vertical, i + 1)
  histSizeTable.update()
  //--------------------------------
  // sizeHistogramm
  //--------------------------------
  sizeHistogramm = model.newHistogrammModel()
  sizeHistogramm.y.min = 0
  sizeHistogramm.y.max = 100
  sizeHistogramm.y.text = "[%]"
  sizeHistogramm.y.decimals = 0
  sizeHistogramm.y.steps = 4

  sizeHistogramm.x.text = "[us]"
  sizeHistogramm.x.decimals = 1
  sizeHistogramm.x.steps = histogrammColumns
  //--------------------------------
  // strengthCurve
  //--------------------------------
  strengthCurve = model.newCurveModel()
  strengthCurve.y.min = 0
  strengthCurve.y.max = 100
  strengthCurve.y.text = "[%]"
  strengthCurve.y.decimals = 0
  strengthCurve.y.steps = 4

  strengthCurve.x.text = "[N]"
  strengthCurve.x.decimals = 1
  strengthCurve.x.steps = histogrammColumns
  //--------------------------------
  // fractStrengthGraph
  //--------------------------------
  fractStrengthGraph = model.newGraphModel()
  fractStrengthGraph.y.decimals = 1
  fractStrengthGraph.x.decimals = 1
  fractStrengthGraph.y.text = "[N/mm^2]"
  fractStrengthGraph.x.text = "[um]"
  fractStrengthGraph.x.steps = 5
  fractStrengthGraph.y.steps = 5

}
//-----------------------------------------------------------------------------------
function modelUpdate(session, role)
{
//  displayData("session", session);

  model.dictionarySet("userName", session.userName)
  model.dictionarySet("reportHeader", qsTr("The static strength diamond powder test"))

  statistic(session)
  //--------------------------------
  // paramTable
  //--------------------------------
  paramTable.setData(serialRow, valueCol, session.deviceSerial)
  paramTable.setData(dateRow, valueCol, Qt.formatDate(session.start, qsTr("yyyy-MM-dd")))
  //paramTable.setData(userRow, valueCol, session.userName)
  paramTable.setData(lotRow, valueCol, session.lot)
  paramTable.setData(gritRow, valueCol, session.grit)
  paramTable.setData(particlesRow, valueCol, session.particles)
  paramTable.setData(avgStretchRow, valueCol, session.avgStrength.toFixed(2))
  paramTable.setData(devStrengthRow, valueCol, session.devStrength.toFixed(2))
  paramTable.setData(maxStrengthRow, valueCol, session.maxStrength.toFixed(2))
  paramTable.setData(minStrengthRow, valueCol, session.minStrength.toFixed(2))
  paramTable.setData(avgSizeRow, valueCol, session.avgSize.toFixed(2))
  paramTable.setData(devSizeRow, valueCol, session.devSize.toFixed(2))
  paramTable.setData(avgFractStrRow, valueCol, session.avgFractStr.toFixed(2))
  paramTable.setData(devFractStrRow, valueCol, session.devFractStr.toFixed(2))
  paramTable.setData(markRow, valueCol, session.mark)


  //--------------------------------
  // strengthHistogramm
  //--------------------------------
  strengthHistogramm.x.min = session.minStrength
  strengthHistogramm.x.max = session.maxStrength

  //--------------------------------
  // sizeHistogramm
  //--------------------------------
  sizeHistogramm.x.min = session.minSize
  sizeHistogramm.x.max = session.maxSize

  //--------------------------------
  // strengthCurve
  //--------------------------------
  strengthCurve.x.min = session.minStrength
  strengthCurve.x.max = session.maxStrength
  strengthCurve.clear()

  //fill histogramm
  var hStrength = []
  var hSize = []

  for(var i = 0; i < histogrammColumns; i++)
  {
    hStrength[i] = 0
    hSize[i] = 0
  }

  var strengthStep = (session.maxStrength - session.minStrength) / histogrammColumns
  var sizeStep = (session.maxSize - session.minSize) / histogrammColumns

  for(i = 0; i < session.measures.length; i++)
  {
    var m = session.measures[i]
    if(m.ignored)
      continue
    var v = m.strength - session.minStrength
    var col = Math.round(v / strengthStep)
    if(col >= histogrammColumns)
      col = histogrammColumns - 1
    hStrength[col] += 1
    v = m.size - session.minSize
    col = Math.round(v / sizeStep)
    if(col >= histogrammColumns)
      col = histogrammColumns - 1
    hSize[col] += 1
  }

  for(i = 0; i < histogrammColumns; i++)
  {
    histStrengthTable.setData(i, 0, (session.minStrength + i * strengthStep).toFixed(2))
    histStrengthTable.setData(i, 1, (session.minStrength + (1 + i) * strengthStep).toFixed(2))
    histStrengthTable.setData(i, 2, hStrength[i])
    histStrengthTable.setData(i, 3, (hStrength[i] * 100. / session.particles).toFixed(2))

    histSizeTable.setData(i, 0, (session.minSize + i * sizeStep).toFixed(2))
    histSizeTable.setData(i, 1, (session.minSize + (1 + i) * sizeStep).toFixed(2))
    histSizeTable.setData(i, 2, hSize[i])
    histSizeTable.setData(i, 3, (hSize[i] * 100. / session.particles).toFixed(2))

    strengthCurve.add(session.minStrength + i * strengthStep, hStrength[i] * 100. / session.particles)

    hStrength[i] *= 100. / session.particles
    hSize[i] *= 100. / session.particles
  }
  strengthHistogramm.data = hStrength
  sizeHistogramm.data = hSize
  strengthCurve.add(session.minStrength + i * strengthStep, 0)

  //--------------------------------
  // fractStrengthGraph
  //--------------------------------
  fractStrengthGraph.y.min = session.minFractStr
  fractStrengthGraph.y.max = session.maxFractStr
  fractStrengthGraph.x.min = session.minSize
  fractStrengthGraph.x.max = session.maxSize
  fractStrengthGraph.clear()
  for(i = 0; i < session.measures.length; i++)
  {
    m = session.measures[i]
    if(m.ignored)
      continue
    var x = m.size
    var y = fractalStrength(m.strength, m.size)
    fractStrengthGraph.add(x, y)
    //print(i, x, y)
  }
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
      val = fractalStrength(m.strength, m.size)
      if(minFractStr > val)
        minFractStr = val
      if(maxFractStr < val)
        maxFractStr = val
      avgFractStr += val
    }

    count ++
  }

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
        devFractStr += Math.pow(fractalStrength(m.strength, m.size) - avgFractStr, 2)
    }

    devStrength = standardDeviation(devStrength, count)
    devSize = standardDeviation(devSize, count)
    devFractStr = standardDeviation(devFractStr, count)
  }

  s["minStrength"] = minStrength
  s["maxStrength"] = maxStrength
  s["avgStrength"] = avgStrength
  s["devStrength"] = devStrength

  s["minSize"] = minSize
  s["maxSize"] = maxSize
  s["avgSize"] = avgSize
  s["devSize"] = devSize

  s["minFractStr"] = minFractStr
  s["maxFractStr"] = maxFractStr
  s["avgFractStr"] = avgFractStr
  s["devFractStr"] = devFractStr

  s["particles"] = count
}
//-----------------------------------------------------------------------------------
function fractalStrength(strength, size)
{
  if(Math.abs(size) < 0.001)
    return 0
  return 10000 * strength / (size * size) //~~~ !!! ATTENTION - this is not true !!! ---
}
//-----------------------------------------------------------------------------------
function standardDeviation(sumOfSqDeviation, count)
{
  if(count < 2)
    return 0
  return Math.sqrt(sumOfSqDeviation / (count - 1))
}
//-----------------------------------------------------------------------------------
