#pragma once

const char* webpageCode = R"rawliteral(
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
    </style>
    <script>
        function refreshData() {
            var xhr = new XMLHttpRequest();
            xhr.onreadystatechange = function() {
                if (xhr.readyState == 4 && xhr.status == 200) {
                    var data = JSON.parse(xhr.responseText);
                    document.getElementById('visorState').innerHTML = data.visorState;
                    document.getElementById('IRState').innerHTML = data.IRState;
                    document.getElementById('fanLevel').innerHTML = data.fanLevel;
                    document.getElementById('lampLevel').innerHTML = data.lampLevel;
                    document.getElementById('batteryLevel').innerHTML = data.batteryLevel;
                    document.getElementById('audioState').innerHTML = data.audioState;
                    document.getElementById('fanRPM').innerHTML = data.fanRPM;
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
        <div class="item">Visor state: <span id="visorState"></span></div>
        <div class="item">Helmet active: <span id="IRState"></span></div>
        <div class="item">Fan speed: <span id="fanLevel"></span></div>
        <div class="item">Lamp sensitivity: <span id="lampLevel"></span></div>
        <div class="item">Battery level: <span id="batteryLevel"></span></div>
        <div class="item">Audio state: <span id="audioState"></span></div>
        <div class="item">Fan RPM: <span id="fanRPM"></span></div>
    </div>
    <a href="/">Back to Main</a>
</body>
</html>
)rawliteral";
