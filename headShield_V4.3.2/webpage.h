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
    <li><a href="/helmetData">Helmet Data</a></li>
    <li><a href="/sensorData">Sensor Data</a></li>
    <li><a href="/control">Control</a></li>
    <li><a href="/debugData">Debug Data</a></li>
  </ul>
</body>
</html>
)rawliteral";

const char *helmetDataPage = R"rawliteral(
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
function setIRStateText(state) {
  var IRStateElement = document.getElementById('IRState');
  if (state == 0) {
    IRStateElement.innerHTML = "Helmet not in head";
    IRStateElement.className = "state-box red";
  } else {
    IRStateElement.innerHTML = "Helmet in use";
    IRStateElement.className = "state-box green";
  }
}
function setVisorStateText(state) {
  var visorStateElement = document.getElementById('visorState');
  if (state == 0) {
    visorStateElement.innerHTML = "Visor up";
    visorStateElement.className = "state-box red";
  } else {
    visorStateElement.innerHTML = "Visor active";
    visorStateElement.className = "state-box green";
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
 function setAudioStateText(state) {
            var audioStateElement = document.getElementById('audioState');
            if (state == 0) {
                audioStateElement.innerHTML = "OFF";
                audioStateElement.className = "state-box red";
            } else {
                audioStateElement.innerHTML = "ON";
                audioStateElement.className = "state-box green";
            }
        }


function setBatteryLevelText(level) {
            var batteryLevelElement = document.getElementById('batteryLevel');
            switch (level) {
                case 0:
                    batteryLevelElement.innerHTML = "OFF";
                    batteryLevelElement.style.backgroundColor = "red";
                    break;
                case 1:
                    batteryLevelElement.innerHTML = "LOW";
                    batteryLevelElement.style.backgroundColor = "yellow";
                    break;
                case 2:
                    batteryLevelElement.innerHTML = "MEDIUM";
                    batteryLevelElement.style.backgroundColor = "#FFD700"; // brighter yellow
                    break;
                case 3:
                    batteryLevelElement.innerHTML = "HIGH";
                    batteryLevelElement.style.backgroundColor = "green";
                    break;
            }
        }

        function setFanRPMText(speed_rpm) {
            var fanRPMElement = document.getElementById('fanRPM');
            var statusText = "";
            switch(speed_rpm) {
                case 0:
                    statusText = "NOT ENOUGH FLOW";
                    break;
                case 1: 
                    statusText = "NORMAL";
                    break;
                case 2:
                    statusText = "NO FILTER";
                    break;
                case 3:
                    statusText = "FAN MALFUNCTION";
                    break;
                case 4:
                    statusText = "OBSTACLE IN THE OUTLET";
                    break;
                default:
                    statusText = "Invalid speed";
            }
            fanRPMElement.innerHTML = statusText;
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
            setAudioStateText(data.audioState);
            setBatteryLevelText(data.batteryLevel);
            setFanRPMText(data.fanRPM);
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
        setLampLevelText(0); // Set initial lamp level state to "OFF"
        </script>
        <div class="item">Audio: <span id="audioState" class="state-box"></span></div>
        <script>
        setAudioStateText(0); // Set initial audio state to "OFF"
        </script>
        <div class="item">Charge: <span id="batteryLevel" class="state-box"></span></div>
        <script>
        setBatteryLevelText(0); // Set initial battery level state to "OFF"
        </script>
        <div class="item">Airflow: <span id="fanRPM"></span></div>
        <script>
        setFanRPMText(0); // Set initial fan RPM to 0
        </script>
        <!-- Other elements... -->
    </div>
    <a href="/">Back to Main</a>
</body>
</html>
)rawliteral";

const char sensorDataPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Sensor Data</title>
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
        .yellow {
            background-color: yellow;
        }
        .slightly-darker-green {
            background-color: #008000;
        }
        .slightly-darker-yellow {
            background-color: #FFD700;
        }
        .sensor-box {
            border: 1px solid #000;
            margin-bottom: 20px;
            padding: 10px;
        }
    </style>
<script>
function setTempText(temp) {
    var tempElement = document.getElementById('temp');
    tempElement.innerHTML = temp;
}

function setPressText(press) {
    var pressElement = document.getElementById('press');
    pressElement.innerHTML = press;
}

function setHumiText(humi) {
    var humiElement = document.getElementById('humi');
    humiElement.innerHTML = humi;
}

function setAQIText(AQI) {
    var AQIElement = document.getElementById('AQI');
    switch (AQI) {
        case 1:
            AQIElement.innerHTML = "Excellent";
            AQIElement.className = "state-box green";
            break;
        case 2:
            AQIElement.innerHTML = "Good";
            AQIElement.className = "state-box slightly-darker-green";
            break;
        case 3:
            AQIElement.innerHTML = "Moderate";
            AQIElement.className = "state-box yellow";
            break;
        case 4:
            AQIElement.innerHTML = "Poor";
            AQIElement.className = "state-box slightly-darker-yellow";
            break;
        case 5:
            AQIElement.innerHTML = "Unhealthy";
            AQIElement.className = "state-box red";
            break;
    }
}

function setTVOCText(TVOC) {
    var TVOCElement = document.getElementById('TVOC');
    TVOCElement.innerHTML = TVOC;
}

function setECO2Text(ECO2) {
    var ECO2Element = document.getElementById('ECO2');
    ECO2Element.innerHTML = ECO2;
}

function setStatusText(status) {
    var statusElement = document.getElementById('status');
    switch (status) {
        case 0:
            statusElement.innerHTML = "normal mode";
            statusElement.className = "state-box green";
            break;
        case 1:
            statusElement.innerHTML = "warm up";
            statusElement.className = "state-box yellow";
            break;
        case 2:
            statusElement.innerHTML = "start up";
            statusElement.className = "state-box red";
            break;
    }
}

function refreshData() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function() {
        if (xhr.readyState == 4 && xhr.status == 200) {
            var data = JSON.parse(xhr.responseText);
            setTempText(data.temp);
            setPressText(data.press);
            setHumiText(data.humi);
            setAQIText(data.AQI);
            setTVOCText(data.TVOC);
            setECO2Text(data.ECO2);
            setStatusText(data.status);
        }
    };
    xhr.open("GET", "/getSensorData", true);
    xhr.send();
}

setInterval(refreshData, 1000); // Refresh data every 1000 milliseconds (1 second)
</script>

</head>
<body>
    <h1>Sensor Data</h1>
    <div id="data">
        <div class="sensor-box">
            <h2>BMS280</h2>
            <div class="item">Temperature: <span id="temp"></span> Celsius</div>
            <div class="item">Pressure: <span id="press"></span> Pa</div>
            <div class="item">Humidity: <span id="humi"></span> %</div>
        </div>
        <div class="sensor-box">
            <h2>ENS160</h2>
            <div class="item">AQI: <span id="AQI" class="state-box"></span></div>
            <div class="item">TVOC: <span id="TVOC"></span> ppb</div>
            <p>concentration of total volatile organic compounds</p>
            <div class="item">ECO2: <span id="ECO2"></span> ppm</div>
            <p>detected data of VOCs and hydrogen</p>
            <div class="item">Status: <span id="status" class="state-box"></span></div>
        </div>
    </div>
    <a href="/">Back to Main</a>
</body>
</html>
)rawliteral";

const char controlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Control Page</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
        }
        .control-box {
            border: 1px solid #000;
            margin-bottom: 20px;
            padding: 10px;
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
        .slightly-darker-yellow {
            background-color: #FFD700;
        }
        .yellow {
            background-color: yellow;
        }
        .green {
            background-color: green;
        }
        .audio-button {
            width: 70px;
            height: 30px;
            color: white;
            text-align: center;
            cursor: pointer;
            margin: auto;
            display: block;
        }
    </style>
    <script>
        var audioState = false;
        function sendAudioState() 
        {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/setAudioState?state=" + audioState, true);
            xhr.send();
        }
        function setFanSpeedText(level) 
        {
            var levelElement = document.getElementById('fanSpeedValue');
            switch(level) {
                case 0:
                    levelElement.innerHTML = "OFF";
                    levelElement.className = "state-box red";
                    break;
                case 1:
                    levelElement.innerHTML = "low";
                    levelElement.className = "state-box yellow";
                    break;
                case 2:
                    levelElement.innerHTML = "medium";
                    levelElement.className = "state-box slightly-darker-yellow";
                    break;
                case 3:
                    levelElement.innerHTML = "high";
                    levelElement.className = "state-box green";
                    break;
            }
        }
        function setLampLevelText(level) 
        {
            var levelElement = document.getElementById('lampLevelValue');
            switch(level) {
                case 0:
                    levelElement.innerHTML = "OFF";
                    levelElement.className = "state-box red";
                    break;
                case 1:
                    levelElement.innerHTML = "low";
                    levelElement.className = "state-box yellow";
                    break;
                case 2:
                    levelElement.innerHTML = "medium";
                    levelElement.className = "state-box slightly-darker-yellow";
                    break;
                case 3:
                    levelElement.innerHTML = "high";
                    levelElement.className = "state-box green";
                    break;
            }
        }
        function toggleAudio() 
        {
            audioState = !audioState;
            var audioButton = document.getElementById('audioButton');
            if (audioState) 
            {
                audioButton.style.backgroundColor = "green";
                audioButton.innerHTML = "ON";
            } else 
            {
                audioButton.style.backgroundColor = "red";
                audioButton.innerHTML = "OFF";
            }
            sendAudioState();
        }

        function sendFanSpeed(speed) 
        {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/setFanSpeed?value=" + speed, true);
            xhr.send();
        }

        function handleFanSpeedChange(value) 
        {
            var speed = parseInt(value, 10);
            setFanSpeedText(speed);
            sendFanSpeed(speed);
        }

        function sendLampLevel(level) 
        {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/setLampLevel?level=" + level, true);
            xhr.send();
        }

        function handleLampLevelChange(value) {
            var level = parseInt(value, 10);
            setLampLevelText(level);
            sendLampLevel(level);
        }
    </script>
</head>
<body>
    <h1>Control Page</h1>
    <div class="control-box">
        <h2>Fan Speed</h2>
        <input type="range" min="0" max="3" step="1" onchange="handleFanSpeedChange(this.value)">
        <div>Current Fan Speed: <span id="fanSpeedValue"></span></div>
    </div>
    <div class="control-box">
        <h2>Lamp Level</h2>
        <input type="range" min="0" max="3" step="1" onchange="handleLampLevelChange(this.value)">
        <div>Current Lamp Level: <span id="lampLevelValue"></span></div>
    </div>
    <div class="control-box">
        <h2>Audio</h2>
        <button id="audioButton" class="audio-button" onclick="toggleAudio()">OFF</button>
    </div>
</body>
</html>
)rawliteral";


const char DEBUG_PAGE[] = R"rawliteral(
<!DOCTYPE html>
<html>
    <head>
        <title>Debug Page</title>
    </head>
    <body>
        <h1>Debug Page</h1>
        <p>
            <strong>Dummy Value 1:</strong> <span id="dummy1">--</span><br>
            <strong>Dummy Value 2:</strong> <span id="dummy2">--</span><br>
            <strong>Dummy Value 3:</strong> <span id="dummy3">--</span>
        </p>
        <script>
            setInterval(() => {
                fetch('/debugdata').then(response => response.json())
                .then(data => {
                    document.getElementById('dummy1').textContent = data.dummy1;
                    document.getElementById('dummy2').textContent = data.dummy2;
                    document.getElementById('dummy3').textContent = data.dummy3;
                });
            }, 600); // Fetch data every 0,6 second
        </script>
    </body>
</html>
)rawliteral";

