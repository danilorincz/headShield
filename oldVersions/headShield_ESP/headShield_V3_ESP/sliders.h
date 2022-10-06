<!-- Complete project details: https://randomnerdtutorials.com/esp32-web-server-websocket-sliders/ -->

<!DOCTYPE html>
<html>
<head>
    <title>ESP IOT DASHBOARD</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="icon" type="image/png" href="favicon.png">
    <link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
    <div class="topnav">
        <h1>Multiple Sliders</h1>
    </div>
    <div class="content">
        <div class="card-grid">
            <div class="card">
                <p class="card-title">Fader 1</p>
                <p class="switch">
                    <input type="range" onchange="updateSliderPWM(this)" id="slider1" min="0" max="100" step="1" value ="0" class="slider">
                </p>
                <p class="state">Brightness: <span id="sliderValue1"></span> &percnt;</p>
            </div>
            <div class="card">
                <p class="card-title"> Fader 2</p>
                <p class="switch">
                    <input type="range" onchange="updateSliderPWM(this)" id="slider2" min="0" max="100" step="1" value ="0" class="slider">
                </p>
                <p class="state">Brightness: <span id="sliderValue2"></span> &percnt;</p>
            </div>
            <div class="card">
                <p class="card-title"> Fader 3</p>
                <p class="switch">
                    <input type="range" onchange="updateSliderPWM(this)" id="slider3" min="0" max="100" step="1" value ="0" class="slider">
                </p>
                <p class="state">Brightness: <span id="sliderValue3"></span> &percnt;</p>
            </div>
        </div>
    </div>
    <script src="script.js"></script>
</body>
</html>