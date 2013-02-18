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
var handleSize = true
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
if(handleSize)
{
  var avgSizeRow = row ++
  var devSizeRow = row ++
}
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
//var histogrammColumns = 10
var sizeColumns = 5
//-----------------------------------------------------------------------------------
var gritTable =
[
    [ //ASTM

     {max:25, min:20},
     {max:31, min:25},
     {max:38, min:31},
     {max:45, min:38}, //DDA - 0
     {max:53, min:45},
     {max:63, min:53},
     {max:75, min:63},
     {max:90, min:75},
     {max:106, min:90},
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
     {max:1400, min:1180}, //DDA - MAX
     {max:1700, min:1400}
    ],
    [ //GOST
     {max:20, min:14},
     {max:28, min:20},
     {max:40, min:28},
     {max:50, min:40}, //DDA - 0
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
     {max:2000, min:1600}, //DDA - MAX
     {max:2500, min:2000}
    ],
    [ //DSTU
     {max:20, min:14},
     {max:28, min:20},
     {max:40, min:28},
     {max:50, min:40}, //DDA - 0
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
     {max:2000, min:1600}, //DDA - MAX
     {max:2500, min:2000}
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
  if(handleSize)
  {
    paramTable.setData(avgSizeRow, nameCol, qsTr("Avg. size [um]"))
    paramTable.setData(devSizeRow, nameCol, qsTr("St. dev. size [um]"))
    //paramTable.setData(avgFractStrRow, nameCol, qsTr("Avg. fract str [N/mm^2]"))
    //paramTable.setData(devFractStrRow, nameCol, qsTr("St. fract str [N/mm^2]"))
  }
  paramTable.setData(markRow, nameCol, qsTr("Set mark"))
  paramTable.update()

  //--------------------------------
  // histStrengthTable
  //--------------------------------
  histStrengthTable = model.newTableModel()
  histStrengthTable.columnCount = 4
  histStrengthTable.rowCount = sizeColumns
  histStrengthTable.setHeaderData(0, Qt.Horizontal, qsTr(">="))
  histStrengthTable.setHeaderData(1, Qt.Horizontal, qsTr("<"))
  histStrengthTable.setHeaderData(2, Qt.Horizontal, qsTr("Particles"))
  histStrengthTable.setHeaderData(3, Qt.Horizontal, qsTr("% of grain"))
  for(var i = 0; i < 14; i++)
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
  strengthHistogramm.x.decimals = 0
  strengthHistogramm.x.steps = 10
  strengthHistogramm.title = qsTr("Distribution of the strength of grain")
  if(role === DataModel.ReportRole)
  {
    strengthHistogramm.brush = {style : Qt.Dense3Pattern, color : "black"}
    //strengthHistogramm.pen = {style : Qt.SolidLine, color : "black"}
  }
  //--------------------------------
  // histSizeTable
  //--------------------------------
  if(handleSize)
  {
    histSizeTable = model.newTableModel()
    histSizeTable.columnCount = 4
    histSizeTable.rowCount = sizeColumns
    histSizeTable.setHeaderData(0, Qt.Horizontal, qsTr(">="))
    histSizeTable.setHeaderData(1, Qt.Horizontal, qsTr("<"))
    histSizeTable.setHeaderData(2, Qt.Horizontal, qsTr("Particles"))
    histSizeTable.setHeaderData(3, Qt.Horizontal, qsTr("% of grain"))
    for(var i = 0; i < sizeColumns; i++)
      histSizeTable.setHeaderData(i, Qt.Vertical, i + 1)
      histSizeTable.update()
  }
  //--------------------------------
  // sizeHistogramm
  //--------------------------------
  if(handleSize)
  {
    sizeHistogramm = model.newHistogrammModel()
    sizeHistogramm.y.min = 0
    sizeHistogramm.y.max = 100
    sizeHistogramm.y.text = "[%]"
    sizeHistogramm.y.decimals = 0
    sizeHistogramm.y.steps = 4

    sizeHistogramm.x.text = qsTr("[um]")
    sizeHistogramm.x.decimals = 0
    sizeHistogramm.x.steps = sizeColumns
    sizeHistogramm.title = qsTr("Distribution of the size of grain")
    if(role === DataModel.ReportRole)
    {
      sizeHistogramm.brush = {style : Qt.Dense3Pattern, color : "black"}
      //sizeHistogramm.pen = {style : Qt.SolidLine, color : "black"}
    }
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
  densityStrengthCurve.x.decimals = 0
  densityStrengthCurve.x.steps = 10
  densityStrengthCurve.title = qsTr("The density distribution of strength")
  if(role === DataModel.ReportRole)
  {
    densityStrengthCurve.filed = false
    densityStrengthCurve.pen = {width : 2, color : "black"}
  }
  //--------------------------------
  // sizeStrengthGraph
  //--------------------------------
  if(handleSize)
  {
    sizeStrengthGraph = model.newGraphModel()
    sizeStrengthGraph.y.decimals = 0
    sizeStrengthGraph.x.decimals = 0
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
  if(handleSize)
  {
    paramTable.setData(avgSizeRow, valueCol, session.avgSize.toFixed(2))
    paramTable.setData(devSizeRow, valueCol, session.devSize.toFixed(2))
  }
  paramTable.setData(markRow, valueCol, session.mark)


  //--------------------------------
  // strengthHistogramm
  //--------------------------------
  var steps = [3000, 2000, 1000, 500, 400, 300, 200, 100, 50, 40, 30, 20, 10, 5]
  var minStrength = session.minStrength
  var maxStrength = session.maxStrength

  var minStep = (maxStrength - minStrength) / 14
  var maxStep = (maxStrength - minStrength) / 10
  var strengthStep = 0
  var strengthColumns = 0

  for(var i in steps)
  {
    var s = steps[i]
    if(s >= minStep && s <= maxStep)
    {
      strengthStep = s
      break
    }
  }

  if(!strengthStep)
  {
    for(var i = steps.length - 1; i >= 0;i--)
    {
      var s = steps[i]
      if(s >= minStep && s >= maxStep)
      {
        strengthStep = s;
        break;
      }
    }
  }

  minStrength = roundTo(minStrength, strengthStep, false)
  for(var i = 10; i <= 14; i++)
  {
    if(minStrength + i * strengthStep > maxStrength)
    {
      strengthColumns = i
      maxStrength = minStrength + i * strengthStep
      break
    }
  }

  //print(minStrength, maxStrength, strengthStep, strengthColumns)

  strengthHistogramm.x.min = minStrength
  strengthHistogramm.x.max = maxStrength

  //--------------------------------
  // sizeHistogramm
  //--------------------------------
  var sizeTbl = gritTable[session.standardIndex]

  var gritIndex = session.gritIndex + 3 //Таблица в js сдвинута на 3
  var minSize = sizeTbl[gritIndex - 3].min
  var maxSize = sizeTbl[gritIndex + 1].max


  var sizeMark = [];
  sizeMark[0] = minSize
  sizeMark[1] = sizeTbl[gritIndex - 2].min
  sizeMark[2] = sizeTbl[gritIndex - 1].min
  sizeMark[3] = sizeTbl[gritIndex].min
  sizeMark[4] = sizeTbl[gritIndex].max
  sizeMark[5] = sizeTbl[gritIndex + 1].max

  if(handleSize)
  {
    sizeHistogramm.x.min = minSize
    sizeHistogramm.x.max = maxSize
  }

  //--------------------------------
  // densityStrengthCurve
  //--------------------------------
  densityStrengthCurve.x.min = minStrength
  densityStrengthCurve.x.max = maxStrength
  densityStrengthCurve.clear()

  //fill histogramm
  var hStrength = []
  var hSize = []

  for(var i = 0; i < sizeColumns; i++)
  {
    hSize[i] = 0
  }

  for(var i = 0; i < strengthColumns; i++)
  {
    hStrength[i] = 0
  }

  for(i = 0; i < session.measures.length; i++)
  {
    var m = session.measures[i]
    if(m.ignored)
      continue
    var v = m.strength - minStrength
    var col = Math.round(v / strengthStep - 0.5)
    hStrength[col] += 1

    v = m.size
    for(col = 0; col < sizeColumns; col++)
    {
      if(sizeMark[col] <= v && sizeMark[col + 1] > v)
      {
        hSize[col] += 1
        break
      }
    }

  }

  var percSum = 0

  histStrengthTable.rowCount = strengthColumns
  histStrengthTable.update()
  for(i = 0; i < strengthColumns; i++)
  {
    histStrengthTable.setData(i, 0, (minStrength + i * strengthStep).toFixed(2))
    histStrengthTable.setData(i, 1, (minStrength + (1 + i) * strengthStep).toFixed(2))
    histStrengthTable.setData(i, 2, hStrength[i])
    histStrengthTable.setData(i, 3, (hStrength[i] * 100. / session.particles).toFixed(2))

    percSum += hStrength[i] * 100. / session.particles
    densityStrengthCurve.add(minStrength + i * strengthStep, percSum)

    hStrength[i] *= 100. / session.particles
    //print("hStrength[" + i + "]=", hStrength[i])
  }
  strengthHistogramm.x.steps = strengthColumns
  strengthHistogramm.data = hStrength
  densityStrengthCurve.x.steps = strengthColumns

  if(handleSize)
  {
    for(i = 0; i < sizeColumns; i++)
    {
      histSizeTable.setData(i, 0, sizeMark[i].toFixed(2))
      histSizeTable.setData(i, 1, sizeMark[i + 1].toFixed(2))
      histSizeTable.setData(i, 2, hSize[i])
      histSizeTable.setData(i, 3, (hSize[i] * 100. / session.particles).toFixed(2))

      hSize[i] *= 100. / session.particles
    }
    sizeHistogramm.data = hSize
    sizeHistogramm.x.marks = sizeMark
    //--------------------------------
    // sizeStrengthGraph
    //--------------------------------
    sizeStrengthGraph.y.min = minStrength
    sizeStrengthGraph.y.max = maxStrength
    //sizeStrengthGraph.y.steps = strengthColumns

    sizeStrengthGraph.x.min = minSize
    sizeStrengthGraph.x.max = maxSize
    sizeStrengthGraph.x.marks = sizeMark //!!!!!
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
function roundTo(val, step, upper)
{
  var n
  if(upper)
    n = Math.round(val / step + 0.5)
  else
    n = Math.round(val / step - 0.5)
  return n * step
}
//-----------------------------------------------------------------------------------
