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
        #stateText {
            display: inline-block;
            padding: 4px 12px;
            border-radius: 12px;
            font-size: 1.5em;
        }
    </style>
</head>
<body>

<h1>Premet Powered<br>Air Purifying Respirator</h1>

<div class="card">
    <h2>System</h2>
    <p id="state">State: <span id="stateText"></span></p>
    <p id="battery">Battery: </p>
</div>

<div class="card">
    <h2>Enviroment</h2>
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
            // ... Existing Code ...

            let stateText = document.getElementById('stateText');
            switch (data.state) {
                case 0:
                    stateText.innerText = "OFF";
                    stateText.style.backgroundColor = "black";
                    stateText.style.color = "white";
                    break;
                case 1:
                    stateText.innerText = "Filter/Fan Error";
                    stateText.style.backgroundColor = "red";
                    stateText.style.color = "white";
                    break;
                case 2:
                    stateText.innerText = "Normal";
                    stateText.style.backgroundColor = "green";
                    stateText.style.color = "white";
                    break;
                case 3:
                    stateText.innerText = "Airflow Error";
                    stateText.style.backgroundColor = "yellow";
                    stateText.style.color = "black";
                    break;
                case 4:
                    stateText.innerText = "Malfunction";
                    stateText.style.backgroundColor = "purple";
                    stateText.style.color = "white";
                    break;
                default:
                    stateText.innerText = "Unknown";
                    stateText.style.backgroundColor = "gray";
                    stateText.style.color = "white";
            }
        });
    }

    // Fetch data every 2 seconds
    setInterval(fetchData, 2000);
</script>

</body>
</html>
)=====";
