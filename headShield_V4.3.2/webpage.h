const char *webpageCode = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Premet Powered Air Purifying Respirator</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 20px;
            background-color: #f4f4f4;
        }
        .card {
            background-color: #fff;
            border-radius: 8px;
            box-shadow: 0 4px 8px rgba(0,0,0,0.1);
            margin-bottom: 20px;
            padding: 5px;  /* Reduced padding */
            display: inline-block;  /* Added */
        }
        h2 {
            margin: 0;  /* Added */
            padding: 0;  /* Added */
        }
        p {
            margin: 5px 0;  /* Modified */
        }
    </style>
</head>
<body>

<h1>Premet Powered Air Purifying Respirator</h1>

<div class="card">
    <h2>State & Battery</h2>
    <p>State: <span id="state"></span></p>
    <p>Battery: <span id="battery"></span></p>
</div>

<div class="card">
    <h2>Environmental Data</h2>
    <p>Temperature: <span id="temp"></span></p>
    <p>Pressure: <span id="press"></span></p>
    <p>Humidity: <span id="humi"></span></p>
</div>

<div class="card">
    <h2>Air Quality</h2>
    <p>AQI: <span id="AQI"></span></p>
    <p>TVOC: <span id="TVOC"></span></p>
    <p>ECO2: <span id="ECO2"></span></p>
</div>

<script>
    function fetchData() {
        fetch('/helmetData')
        .then(response => response.json())
        .then(data => {
            document.getElementById('state').innerText = data.state;
            document.getElementById('battery').innerText = data.battery;
            document.getElementById('temp').innerText = data.temp;
            document.getElementById('press').innerText = data.press;
            document.getElementById('humi').innerText = data.humi;
            document.getElementById('AQI').innerText = data.AQI;
            document.getElementById('TVOC').innerText = data.TVOC;
            document.getElementById('ECO2').innerText = data.ECO2;
        });
    }

    // Fetch data every 2 seconds
    setInterval(fetchData, 2000);
</script>

</body>
</html>
)=====";
