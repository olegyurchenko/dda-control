//print("Procession Ok !!!")
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
var uniformityRow = row ++
var maxStrengthRow = row ++
var minStrengthRow = row ++
var avgSizeRow = row ++
var devSizeRow = row ++
//var avgFractStrRow = row ++
//var devFractStrRow = row ++
var markRow = row ++
//-----------------------------------------------------------------------------------
var histStrengthTable = null
var histSizeTable = null
//-----------------------------------------------------------------------------------
var strengthHistogramm = null
var sizeHistogramm = null
//-----------------------------------------------------------------------------------
var densityStrengthCurve = null
var sizeStrengthGraph = null
//-----------------------------------------------------------------------------------
var histogrammColumns = 10
//-----------------------------------------------------------------------------------
var gritTable =
[
    [ //ASTM
        {max:45, min:38},
        {max:55, min:45},
        {max:63, min:53},
        {max:75, min:63},
        {max:90, min:75},
        {max:106, min:80},
        {max:125, min:106},
        {max:150, min:125},
        {max:180, min:150},
        {max:212, min:180},
        {max:250, min:212},
        {max:300, min:250},
        {max:355, min:300},
        {max:425, min:355},
        {max:500, min:425},
        {max:595, min:500},
        {max:710, min:595},
        {max:850, min:710},
        {max:1000, min:850},
        {max:1180, min:1000},
        {max:1400, min:1180}
      ],
      [ //GOST
        {max:50, min:40},
        {max:63, min:50},
        {max:80, min:63},
        {max:100, min:80},
        {max:125, min:100},
        {max:160, min:125},
        {max:200, min:160},
        {max:250, min:200},
        {max:315, min:250},
        {max:400, min:315},
        {max:500, min:400},
        {max:630, min:500},
        {max:800, min:630},
        {max:1000, min:800},
        {max:1250, min:1000},
        {max:1600, min:1250},
        {max:2000, min:1600}
      ],
      [ //DSTU
        {max:50, min:40},
        {max:63, min:50},
        {max:80, min:63},
        {max:100, min:80},
        {max:125, min:100},
        {max:160, min:125},
        {max:200, min:160},
        {max:250, min:200},
        {max:315, min:250},
        {max:400, min:315},
        {max:500, min:400},
        {max:630, min:500},
        {max:800, min:630},
        {max:1000, min:800},
        {max:1250, min:1000},
        {max:1600, min:1250},
        {max:2000, min:1600}
      ]
]

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
  paramTable.setData(uniformityRow, nameCol, qsTr("Uniformity of grain"))
  paramTable.setData(avgSizeRow, nameCol, qsTr("Avg. size [um]"))
  paramTable.setData(devSizeRow, nameCol, qsTr("St. dev. size [um]"))
  //paramTable.setData(avgFractStrRow, nameCol, qsTr("Avg. fract str [N/mm^2]"))
  //paramTable.setData(devFractStrRow, nameCol, qsTr("St. fract str [N/mm^2]"))
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

  strengthHistogramm.x.text = qsTr("[N]")
  strengthHistogramm.x.decimals = 1
  strengthHistogramm.x.steps = histogrammColumns
  strengthHistogramm.title = qsTr("Distribution of the strength of grain")
  if(role === DataModel.ReportRole)
  {
    strengthHistogramm.brush = {style : Qt.Dense3Pattern, color : "black"}
    //strengthHistogramm.pen = {style : Qt.SolidLine, color : "black"}
  }
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

  sizeHistogramm.x.text = qsTr("[um]")
  sizeHistogramm.x.decimals = 1
  sizeHistogramm.x.steps = histogrammColumns
  sizeHistogramm.title = qsTr("Distribution of the size of grain")
  if(role === DataModel.ReportRole)
  {
    sizeHistogramm.brush = {style : Qt.Dense3Pattern, color : "black"}
    //sizeHistogramm.pen = {style : Qt.SolidLine, color : "black"}
  }
  //--------------------------------
  // densityStrengthCurve
  //--------------------------------
  densityStrengthCurve = model.newCurveModel()
  densityStrengthCurve.y.min = 0
  densityStrengthCurve.y.max = 100
  densityStrengthCurve.y.text = "[%]"
  densityStrengthCurve.y.decimals = 0
  densityStrengthCurve.y.steps = 4

  densityStrengthCurve.x.text = qsTr("[N]")
  densityStrengthCurve.x.decimals = 1
  densityStrengthCurve.x.steps = histogrammColumns
  densityStrengthCurve.title = qsTr("The density distribution of strength")
  if(role === DataModel.ReportRole)
  {
    densityStrengthCurve.filed = false
    densityStrengthCurve.pen = {width : 2, color : "black"}
  }
  //--------------------------------
  // sizeStrengthGraph
  //--------------------------------
  sizeStrengthGraph = model.newGraphModel()
  sizeStrengthGraph.y.decimals = 1
  sizeStrengthGraph.x.decimals = 1
  sizeStrengthGraph.y.text = qsTr("[N]")
  sizeStrengthGraph.x.text = qsTr("[um]")
  sizeStrengthGraph.x.steps = 5
  sizeStrengthGraph.y.steps = 5
  sizeStrengthGraph.title = qsTr("Strength VS Size")
  if(role === DataModel.ReportRole)
  {
    sizeStrengthGraph.brush = {style : Qt.Dense3Pattern, color : "black"}
    //sizeStrengthGraph.pen = {style : Qt.SolidLine, color : "black"}
  }

}
//-----------------------------------------------------------------------------------
function modelUpdate(session, role)
{
//  displayData("session", session);

  model.dictionarySet("userName", session.userName)
  model.dictionarySet("reportHeader", qsTr("The static strength diamond powder test"))
  model.dictionarySet("product", session.product)

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
  if( Math.abs(session.devStrength) >= 0.005 )
    paramTable.setData(uniformityRow, valueCol, (session.avgStrength / session.devStrength).toFixed(2))
  else
    paramTable.setData(uniformityRow, valueCol, "")
  paramTable.setData(maxStrengthRow, valueCol, session.maxStrength.toFixed(2))
  paramTable.setData(minStrengthRow, valueCol, session.minStrength.toFixed(2))
  paramTable.setData(avgSizeRow, valueCol, session.avgSize.toFixed(2))
  paramTable.setData(devSizeRow, valueCol, session.devSize.toFixed(2))
  //paramTable.setData(avgFractStrRow, valueCol, session.avgFractStr.toFixed(2))
  //paramTable.setData(devFractStrRow, valueCol, session.devFractStr.toFixed(2))
  paramTable.setData(markRow, valueCol, session.mark)


  //--------------------------------
  // strengthHistogramm
  //--------------------------------
  strengthHistogramm.x.min = session.minStrength
  strengthHistogramm.x.max = session.maxStrength

  //--------------------------------
  // sizeHistogramm
  //--------------------------------
  var sizeTbl = gritTable[session.standardIndex]
  var minSize = 0
  var maxSize = sizeTbl[sizeTbl.length - 1].max

  if(session.gritIndex >= 3)
    minSize = sizeTbl[session.gritIndex - 3].min
  if(session.gritIndex < sizeTbl.length - 1)
    maxSize = sizeTbl[session.gritIndex + 1].max

  //sizeHistogramm.x.min = session.minSize
  //sizeHistogramm.x.max = session.maxSize
  sizeHistogramm.x.min = minSize
  sizeHistogramm.x.max = maxSize

  //--------------------------------
  // densityStrengthCurve
  //--------------------------------
  densityStrengthCurve.x.min = session.minStrength
  densityStrengthCurve.x.max = session.maxStrength
  densityStrengthCurve.clear()

  //fill histogramm
  var hStrength = []
  var hSize = []

  for(var i = 0; i < histogrammColumns; i++)
  {
    hStrength[i] = 0
    hSize[i] = 0
  }

  var strengthStep = (session.maxStrength - session.minStrength) / histogrammColumns
  //var sizeStep = (session.maxSize - session.minSize) / histogrammColumns
  var sizeStep = (maxSize - minSize) / histogrammColumns

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
    //v = m.size - session.minSize
    v = m.size - minSize
    col = Math.round(v / sizeStep)
    if(col < histogrammColumns)
      hSize[col] += 1
  }

  var percSum = 0
  for(i = 0; i < histogrammColumns; i++)
  {
    histStrengthTable.setData(i, 0, (session.minStrength + i * strengthStep).toFixed(2))
    histStrengthTable.setData(i, 1, (session.minStrength + (1 + i) * strengthStep).toFixed(2))
    histStrengthTable.setData(i, 2, hStrength[i])
    histStrengthTable.setData(i, 3, (hStrength[i] * 100. / session.particles).toFixed(2))

    //histSizeTable.setData(i, 0, (session.minSize + i * sizeStep).toFixed(2))
    //histSizeTable.setData(i, 1, (session.minSize + (1 + i) * sizeStep).toFixed(2))
    histSizeTable.setData(i, 0, (minSize + i * sizeStep).toFixed(2))
    histSizeTable.setData(i, 1, (minSize + (1 + i) * sizeStep).toFixed(2))
    histSizeTable.setData(i, 2, hSize[i])
    histSizeTable.setData(i, 3, (hSize[i] * 100. / session.particles).toFixed(2))

    percSum += hStrength[i] * 100. / session.particles
    densityStrengthCurve.add(session.minStrength + i * strengthStep, percSum)

    hStrength[i] *= 100. / session.particles
    hSize[i] *= 100. / session.particles
  }
  strengthHistogramm.data = hStrength
  sizeHistogramm.data = hSize
  //densityStrengthCurve.add(session.minStrength + i * strengthStep, 0)

  //--------------------------------
  // sizeStrengthGraph
  //--------------------------------
  sizeStrengthGraph.y.min = session.minStrength
  sizeStrengthGraph.y.max = session.maxStrength
  //sizeStrengthGraph.x.min = session.minSize
  //sizeStrengthGraph.x.max = session.maxSize
  sizeStrengthGraph.x.min = minSize
  sizeStrengthGraph.x.max = maxSize
  sizeStrengthGraph.clear()
  for(i = 0; i < session.measures.length; i++)
  {
    m = session.measures[i]
    if(m.ignored)
      continue
    var x = m.size
    var y = m.strength//fractalStrength(m.strength, m.size)
    sizeStrengthGraph.add(x, y)
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
