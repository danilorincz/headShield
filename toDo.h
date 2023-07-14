Problems in the code
    //* create a method to turn off the fans completely
    //* long touch input registers tap first
    //* headSensor optimized
    // the sensor must be connected automatically
    // optimize the fan speed reading
    // do a touch calibration before every start
    // do a tachometer calibration
    // battery reading



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


        function setFanRPMText(speed_rpm) {
            var fanRPMElement = document.getElementById('fanRPM');
            var statusText = "";
            switch(speed_rpm) {
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
                    statusText = "NOT ENOUGH AIRFLOW OR FILTER IS CLOGGED";
                    break;
                case 5:
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

        <div class="item">Airflow: <span id="fanRPM"></span></div>
        <script>
        setFanRPMText(0); // Set initial fan RPM to 0
        </script>

    </div>
    <a href="/">Back to Main</a>
</body>
</html>