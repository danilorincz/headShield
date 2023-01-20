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

<! INSIDE DATA------------------------->
  <div class="card">  
    <h1> <span style="background-color:white">Inside</span> </h1>
  
    <h2> Pressure: <span style="color:yellow" id="insidePressure">0</span> Pa</h2>
    <h2> Temperature: <span style="color:yellow" id="insideTemperature">0</span> C</h2>
    <h2> Humidity: <span style="color:yellow" id="insideHumidity">0</span> %</h2>
    <h2> CO2: <span style="color:yellow" id="insidePPM">0</span> ppm</h2>
    <h2> TVOC: <span style="color:yellow" id="insideTVOC">0</span> tovc</h2>
  </div>


<!-------------------------JavaScrip------------------------->
  <script>
 

 
  

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



  </script>
</body>
</html>
)***";
