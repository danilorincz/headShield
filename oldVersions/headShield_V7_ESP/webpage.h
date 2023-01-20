//============
//Webpage Code
//============

String webpageCode = R"***(
<!DOCTYPE html>
<head>
  <title> ESP32 Web Server </title>
</head>
<html>
<!----------------------------CSS---------------------------->
<style>
  body {background-color: rgba(128, 128, 128, 0.884)}
  h4 {font-family: arial; text-align: center; color: white;}
  .card
  {
     max-width: 450px;
     min-height: 100px;
     background: rgba(255, 0, 0, 0.521);
     padding: 10px;
     font-weight: bold;
     font: 15px calibri;
     text-align: center;
     box-sizing: border-box;
     color: blue;
     margin:20px;
     box-shadow: 0px 2px 15px 15px rgba(0,0,0,0.75);
  }
</style>
<!----------------------------HTML--------------------------->


<body>
<! USER DATA------------------------->
  <div class="card">  
    <h1> <span style="background-color:white">Inside</span> </h1>
  
    <h2> Since start: <span style="color:yellow" id="sinceStart">0</span> [hh:mm:ss]</h2>
    <h2> Battery: <span style="color:yellow" id="batteryCharge">0</span> %</h2>
    <h2> Speed: <span style="color:yellow" id="RPM">0</span> RPM</h2>
    <h2> Delta pressure: <span style="color:yellow" id="deltaPressure">0</span> Pa</h2>
    <h2> State: <span style="color:yellow" id="statusState">0</span> </h2>
  </div>

<! INSIDE DATA------------------------->
  <div class="card">  
    <h1> <span style="background-color:white">Inside</span> </h1>
  
    <h2> Pressure: <span style="color:yellow" id="insidePressure">0</span> Pa</h2>
    <h2> Temperature: <span style="color:yellow" id="insideTemperature">0</span> C</h2>
    <h2> Humidity: <span style="color:yellow" id="insideHumidity">0</span> %</h2>
    <h2> CO2: <span style="color:yellow" id="insidePPM">0</span> ppm</h2>
    <h2> TVOC: <span style="color:yellow" id="insideTVOC">0</span> tovc</h2>
  </div>

<! OUTSIDE DATA------------------------->
  <div class="card">  
    <h1> <span style="background-color:white">OUTSIDE</span> </h1>
  
    <h2> Pressure: <span style="color:yellow" id="outsidePressure">0</span> Pa</h2>
    <h2> Temperature: <span style="color:yellow" id="outsideTemperature">0</span> C</h2>
    <h2> Humidity: <span style="color:yellow" id="outsideHumidity">0</span> %</h2>
  </div>


<! CONTROL PANEL------------------------->
  <div class="card">
  <h1> <span style="background-color:white">Control panel</span> </h1>
    <p class="card-title">Fan speed</p>
      <p class="switch">
      <input type="range" onchange="updateSliderPWM(this)" id="slider1" min="0" max="100" step="1" value ="0" class="slider">
      </p>
    <p class="state">Speed: <span id="sliderValue1"></span> &percnt;</p>
    


    <h2> <button class="button" onclick="send(1)">LED ON</button> <h2>
    <h2> <button class="button" onclick="send(0)">LED OFF</button><BR> <h2>

  </div>


<!-------------------------JavaScrip------------------------->
  <script>
    // AJAX->SINCE START--------------------------------
    setInterval(function()
    {
      getSinceStart();
    }, 1000);
    function getSinceStart()
    {
      var sinceStartRequest = new XMLHttpRequest();
      sinceStartRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("sinceStart").innerHTML =
          this.responseText;
        }
      };
      sinceStartRequest.open("GET", "readSinceStart", true);
      sinceStartRequest.send();
    }
    // AJAX->BATTERY CHARGE--------------------------------
    setInterval(function()
    {
      getBatCharge();
    }, 10000);
    function getBatCharge()
    {
      var batChargeRequest = new XMLHttpRequest();
      batChargeRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("batteryCharge").innerHTML =
          this.responseText;
        }
      };
      batChargeRequest.open("GET", "readCharge", true);
      batChargeRequest.send();
    }

    // AJAX->GET RPM--------------------------------
    setInterval(function()
    {
      getRPM();
    }, 1000);
    function getRPM()
    {
      var RPMRequest = new XMLHttpRequest();
      RPMRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("RPM").innerHTML =
          this.responseText;
        }
      };
      RPMRequest.open("GET", "readRPM", true);
      RPMRequest.send();
    }
    // AJAX->DELTA PRESSURE--------------------------------
    setInterval(function()
    {
      getDeltaPressure();
    }, 1000);
    function getDeltaPressure()
    {
      var deltaPressureRequest = new XMLHttpRequest();
      deltaPressureRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("deltaPressure").innerHTML =
          this.responseText;
        }
      };
      deltaPressureRequest.open("GET", "readDeltaPressure", true);
      deltaPressureRequest.send();
    }

    // AJAX->INSIDE PRESSURE--------------------------------
    setInterval(function()
    {
      getInsidePressure();
    }, 1000);
    function getInsidePressure()
    {
      var insidePressureRequest = new XMLHttpRequest();
      insidePressureRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("insidePressure").innerHTML =
          this.responseText;
        }
      };
      insidePressureRequest.open("GET", "readInsidePressure", true);
      insidePressureRequest.send();
    }



    // AJAX->INSIDE TEMPERATURE--------------------------------
    setInterval(function()
    {
      getInsideTemperature();
    }, 4000);
    function getInsideTemperature()
    {
      var insideTemperatureRequest = new XMLHttpRequest();
      insideTemperatureRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("insideTemperature").innerHTML =
          this.responseText;
        }
      };
      insideTemperatureRequest.open("GET", "readInsideTemp", true);
      insideTemperatureRequest.send();
    }


    // AJAX->INSIDE HUMIDITY--------------------------------
    setInterval(function()
    {
      getInsideHumidity();
    }, 4000);
    function getInsideHumidity()
    {
      var insideHumidityRequest = new XMLHttpRequest();
      insideHumidityRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("insideHumidity").innerHTML =
          this.responseText;
        }
      };
      insideHumidityRequest.open("GET", "readInsideHum", true);
      insideHumidityRequest.send();
    }


    // AJAX->INSIDE PPM--------------------------------
    setInterval(function()
    {
      getInsidePPM();
    }, 1000);
    function getInsidePPM()
    {
      var insidePPMRequest = new XMLHttpRequest();
      insidePPMRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("insidePPM").innerHTML =
          this.responseText;
        }
      };
      insidePPMRequest.open("GET", "readInsidePPM", true);
      insidePPMRequest.send();
    }


    // AJAX->INSIDE TVOC--------------------------------
    setInterval(function()
    {
      getInsideTVOC();
    }, 1000);
    function getInsideTVOC()
    {
      var insideTVOCRequest = new XMLHttpRequest();
      insideTVOCRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("insideTVOC").innerHTML =
          this.responseText;
        }
      };
      insideTVOCRequest.open("GET", "readInsideTVOC", true);
      insideTVOCRequest.send();
    }

    // AJAX->OUTSIDE PRESSURE--------------------------------
    setInterval(function()
    {
      getOutsidePressure();
    }, 1000);
    function getOutsidePressure()
    {
      var outsidePressureRequest = new XMLHttpRequest();
      outsidePressureRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("outsidePressure").innerHTML =
          this.responseText;
        }
      };
      outsidePressureRequest.open("GET", "readOutsidePressure", true);
      outsidePressureRequest.send();
    }
    // AJAX->OUTSIDE TEMPERATURE--------------------------------
    setInterval(function()
    {
      getOutsideTemperature();
    }, 4000);
    function getOutsideTemperature()
    {
      var outsideTemperatureRequest = new XMLHttpRequest();
      outsideTemperatureRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("outsideTemperature").innerHTML =
          this.responseText;
        }
      };
      outsideTemperatureRequest.open("GET", "readOutsideTemp", true);
      outsideTemperatureRequest.send();
    }
    // AJAX->OUTSIDE HUMIDITY--------------------------------
    setInterval(function()
    {
      getOutsideHumidity();
    }, 4000);
    function getOutsideHumidity()
    {
      var outsideHumidityRequest = new XMLHttpRequest();
      outsideHumidityRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("outsideHumidity").innerHTML =
          this.responseText;
        }
      };
      outsideHumidityRequest.open("GET", "readOutsideHum", true);
      outsideHumidityRequest.send();
    }







    // SET FAN SPEED------------------------------------------
    function updateSliderPWM(element) 
    {
        var xhttp = new XMLHttpRequest();

        var sliderNumber = element.id.charAt(element.id.length-1);
        var sliderValue = document.getElementById(element.id).value;
        document.getElementById("sliderValue"+sliderNumber).innerHTML = sliderValue;
        console.log(sliderValue);

        xhttp.open("GET", "slider?state="+sliderValue.toString(), true);
        xhttp.send();
    }






    function send(led_sts) 
    {
      var xhttp = new XMLHttpRequest();
      xhttp.onreadystatechange = function() 
      {
        if (this.readyState == 4 && this.status == 200) 
        {
          document.getElementById("state").innerHTML = this.responseText;
        }
      };
      xhttp.open("GET", "led_set?state="+led_sts, true);
      xhttp.send();
    }

    setInterval(function()
    {
      getStatusState();
    }, 5000);
    function getStatusState()
    {
      var statusStateRequest = new XMLHttpRequest();
      statusStateRequest.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("statusState").innerHTML =
          this.responseText;
        }
      };
      statusStateRequest.open("GET", "statusState", true);
      statusStateRequest.send();
    }


  </script>
</body>
</html>
)***";
