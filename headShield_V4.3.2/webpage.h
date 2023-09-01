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
        display: flex;
        flex-direction: column;
        align-items: center;
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
    #AQI {
    display: inline-block;
    padding: 4px 12px;
    border-radius: 12px;
    font-size: 1.5em;
    }
    .battery {
        width: 50px;
        height: 20px;
        border: 2px solid black;
        position: relative;
    }

    .battery:before {
        content: "";
        width: 5px;
        height: 14px;
        position: absolute;
        top: 50%;
        left: 100%;
        background-color: black;
        transform: translateY(-50%);
    }

    .battery-level {
        height: 100%;
        color: white;
        font-weight: bold;
        text-align: center;  /* To center the text */
    }
</style>

</head>
<body>

<h1>Premet Powered<br>Air Purifying Respirator</h1>

<div class="card">
    <h2>System</h2>
    <p id="state">State: <span id="stateText"></span></p>
    <div class="battery">
        <div id="batteryLevel" class="battery-level"></div>
    </div>
    <p id="batteryPercent"></p>
</div>


<div class="card">
    <h2>Outside Air Quality</h2>
    <p>Air Quality Index: <span id="AQI"></span></p>
    <p id="TVOC">TVOC: </p>
    <p id="ECO2">ECO2: </p>
</div>

<div class="card">
    <h2>Enviroment</h2>
    <p id="temp">Temperature: </p>
    <p id="press">Pressure: </p>
    <p id="humi">Humidity: </p>
</div>

<script>
    function fetchData() {
        fetch('/helmetData')
        .then(response => response.json())
        .then(data => {
            // Handling the State
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

            // Handling the Battery
            let batteryLevel = document.getElementById('batteryLevel');
            let percent = data.battery;
            batteryLevel.style.width = `${percent}%`;
            batteryLevel.innerText = `${percent}%`; // Displaying percentage inside the indicator
            if (percent > 50) {
                batteryLevel.style.backgroundColor = 'green';
            } else if (percent > 20) {
                batteryLevel.style.backgroundColor = 'yellow';
            } else {
                batteryLevel.style.backgroundColor = 'red';
            }

        let AQIElement = document.getElementById('AQI');
        switch (data.AQI) {
            case 1:
                AQIElement.innerText = "Excelent";
                AQIElement.style.backgroundColor = "blue";
                AQIElement.style.color = "white";
                break;
            case 2:
                AQIElement.innerText = "Good";
                AQIElement.style.backgroundColor = "green";
                AQIElement.style.color = "white";
                break;
            case 3:
                AQIElement.innerText = "Moderate";
                AQIElement.style.backgroundColor = "yellow";
                AQIElement.style.color = "black";
                break;
            case 4:
                AQIElement.innerText = "Poor";
                AQIElement.style.backgroundColor = "#DAA520"; // darker yellow
                AQIElement.style.color = "white";
                break;
            case 5:
                AQIElement.innerText = "Unhealthy";
                AQIElement.style.backgroundColor = "red";
                AQIElement.style.color = "white";
                break;
            default:
                AQIElement.innerText = "Unknown";
                AQIElement.style.backgroundColor = "gray";
                AQIElement.style.color = "white";
        }
            // Updating Other Data
            document.getElementById('temp').innerText = `Temperature: ${data.temp} Celsius`;
            document.getElementById('press').innerText = `Pressure: ${data.press} Pa`;
            document.getElementById('humi').innerText = `Humidity: ${data.humi} %`;
            document.getElementById('TVOC').innerText = `TVOC: ${data.TVOC} ppb`;
            document.getElementById('ECO2').innerText = `ECO2: ${data.ECO2} ppm`;
        });
    }

    // Fetch the data every 2 seconds
    setInterval(fetchData, 2000);
</script>


</body>
</html>
)=====";