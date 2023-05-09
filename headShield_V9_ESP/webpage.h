#pragma once

const char *webpageCode = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Helmet Interface</title>
<style>
  body {font-family: Arial, Helvetica, sans-serif;}
  ul {list-style-type: none; margin: 0; padding: 0; overflow: hidden; background-color: #333;}
  li {float: left;}
  li a {display: block; color: white; text-align: center; padding: 14px 16px; text-decoration: none;}
  li a:hover {background-color: #111;}
</style>
</head>
<body>
  <ul>
    <li><a href="/helmetData">Helmet Data</a></li> <!-- Update this line -->
    <li><a href="/sensorData">Sensor Data</a></li>
    <li><a href="/control">Control</a></li>
  </ul>
</body>
</html>
)rawliteral";

const char helmetDataPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Helmet Data</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
        }
        a {
            display: inline-block;
            background-color: #4CAF50;
            color: white;
            padding: 15px 25px;
            text-decoration: none;
            margin: 10px;
        }
        #data {
            margin-top: 20px;
        }
        .item {
            margin-bottom: 10px;
        }
        .state-box {
            display: inline-block;
            padding: 5px;
            text-align: center;
            width: 150px;
        }
        .red {
            background-color: red;
        }
        .green {
            background-color: green;
        }
    </style>
    <script>
function setVisorStateText(visorState) {
  var visorStateElement = document.getElementById('visorState');
  if (visorState == 0) {
    visorStateElement.innerHTML = "Visor up";
    visorStateElement.className = "state-box red";
  } else {
    visorStateElement.innerHTML = "Visor active";
    visorStateElement.className = "state-box green";
  }
}

function setIRStateText(IRState) {
  var IRStateElement = document.getElementById('IRState');
  if (IRState == 0) {
    IRStateElement.innerHTML = "Helmet not in head";
    IRStateElement.className = "state-box red";
  } else {
    IRStateElement.innerHTML = "Helmet in use";
    IRStateElement.className = "state-box green";
  }
}


    function setFanSpeedText(speed) {
  var fanSpeedElement = document.getElementById('fanSpeed');
  switch (speed) {
    case 0:
      fanSpeedElement.innerHTML = "OFF";
      fanSpeedElement.style.backgroundColor = "red";
      break;
    case 1:
      fanSpeedElement.innerHTML = "low";
      fanSpeedElement.style.backgroundColor = "yellow";
      break;
    case 2:
      fanSpeedElement.innerHTML = "medium";
      fanSpeedElement.style.backgroundColor = "#FFD700"; // brighter yellow
      break;
    case 3:
      fanSpeedElement.innerHTML = "high";
      fanSpeedElement.style.backgroundColor = "green";
      break;
  }
}

function setLampLevelText(level) {
  var lampLevelElement = document.getElementById('lampLevel');
  switch (level) {
    case 0:
      lampLevelElement.innerHTML = "OFF";
      lampLevelElement.style.backgroundColor = "red";
      break;
    case 1:
      lampLevelElement.innerHTML = "low";
      lampLevelElement.style.backgroundColor = "yellow";
      break;
    case 2:
      lampLevelElement.innerHTML = "medium";
      lampLevelElement.style.backgroundColor = "#FFD700"; // brighter yellow
      break;
    case 3:
      lampLevelElement.innerHTML = "high";
      lampLevelElement.style.backgroundColor = "green";
      break;
  }
}
function refreshData() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (xhr.readyState == 4 && xhr.status == 200) {
            var data = JSON.parse(xhr.responseText);

            setVisorStateText(data.visorState);
            setIRStateText(data.IRState);
            setFanSpeedText(data.fanLevel);
            setLampLevelText(data.lampLevel);

            // Other elements...
        }
    };
    xhr.open("GET", "/getHelmetData", true);
    xhr.send();
}

    setInterval(refreshData, 1000); // Refresh data every 1000 milliseconds (1 second)
    </script>
</head>
<body>
    <h1>Helmet Data</h1>
    <div id="data">
        <div class="item"><span id="visorState" class="state-box"></span></div>
        <div class="item"><span id="IRState" class="state-box"></span></div>
        <div class="item">Fan speed: <span id="fanSpeed" class="state-box"></span></div>
        <script>
        setFanSpeedText(0); // Set initial fan speed state to "OFF"
        </script>
        <div class="item">Lamp: <span id="lampLevel" class="state-box"></span></div>
        <script>
        setLampLevelText(0); // Set initial fan speed state to "OFF"
        </script>
        <!-- Other elements... -->
    </div>
    <a href="/">Back to Main</a>
</body>
</html>
)rawliteral";
