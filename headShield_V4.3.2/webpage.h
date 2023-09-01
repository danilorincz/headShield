const char *webpageCode = R"=====(
<!DOCTYPE html>
<html>
<head>
    <title>Premet Powered Air Purifying Respirator</title>
    <style>
        .card {
            box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2);
            max-width: 300px;
            margin: auto;
            text-align: center;
            padding: 10px;
            border-radius: 12px;  /* Rounded corners */
        }
        h1 {
            text-align: center;
        }
    </style>
</head>
<body>

<h1>Premet Powered<br>Air Purifying Respirator</h1>

<div class="card">
    <h2>System</h2>
    <p id="state">State: </p>
    <p id="battery">Battery: </p>
</div>

<div class="card">
    <h2>Enviromental</h2>
    <p id="temp">Temperature: </p>
    <p id="press">Pressure: </p>
    <p id="humi">Humidity: </p>
</div>

<div class="card">
    <h2>Air Quality</h2>
    <p id="AQI">AQI: </p>
    <p id="TVOC">TVOC: </p>
    <p id="ECO2">ECO2: </p>
</div>

<script>
    function fetchData() {
        fetch('/helmetData')
        .then(response => response.json())
        .then(data => {
            document.getElementById('state').innerText = "State: " + data.state;
            document.getElementById('battery').innerText = "Battery: " + data.battery + "%";
            document.getElementById('temp').innerText = "Temperature: " + data.temp + " Celsius";
            document.getElementById('press').innerText = "Pressure: " + data.press + " Pa";
            document.getElementById('humi').innerText = "Humidity: " + data.humi + "%";
            document.getElementById('AQI').innerText = "AQI: " + data.AQI;
            document.getElementById('TVOC').innerText = "TVOC: " + data.TVOC + " ppb";
            document.getElementById('ECO2').innerText = "ECO2: " + data.ECO2 + " ppm";
        });
    }

    // Fetch data every 2 seconds
    setInterval(fetchData, 2000);
</script>

</body>
</html>
)=====";
