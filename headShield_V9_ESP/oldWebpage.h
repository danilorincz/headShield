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

<!  DATA------------------------->
  <div class="card">  
    <h1> <span style="background-color:white">SENSOR</span> </h1>
  
    <h2> Pressure: <span style="color:yellow" id="pressure">0</span> Pa</h2>
    <h2> Temperature: <span style="color:yellow" id="temperature">0</span> C</h2>
    <h2> Humidity: <span style="color:yellow" id="humidity">0</span> %</h2>
    <h2> CO2: <span style="color:yellow" id="PPM">0</span> ppm</h2>
    <h2> TVOC: <span style="color:yellow" id="TVOC">0</span> tovc</h2>
  </div>


<!-------------------------JavaScrip------------------------->
  <script>
    setInterval(function()
    {
      getPressure();
    }, 800);
    setInterval(function()
    {
      getTemperature();
    }, 800);
   setInterval(function()
    {
      getHumidity();
    }, 800);
    setInterval(function()
    {
      getPPM();
    }, 800);
    setInterval(function()
    {
      getTVOC();
    }, 800);

    function getPressure()
    {
      var request = new XMLHttpRequest();
      request.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("pressure").innerHTML = this.responseText;
        }
      };
      request.open("GET", "readPressure", true);
      request.send();
    }

    function getTemperature()
    {
      var request = new XMLHttpRequest();
      request.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("temperature").innerHTML = this.responseText;
        }
      };
      request.open("GET", "readTemperature", true);
      request.send();
    }

    function getHumidity()
    {
      var request = new XMLHttpRequest();
      request.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("humidity").innerHTML = this.responseText;
        }
      };
      request.open("GET", "readHumidity", true);
      request.send();
    }

    function getPPM()
    {
      var request = new XMLHttpRequest();
      request.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("PPM").innerHTML = this.responseText;
        }
      };
      request.open("GET", "readPPM", true);
      request.send();
    }

    function getTVOC()
    {
      var request = new XMLHttpRequest();
      request.onreadystatechange = function()
      {
        if(this.readyState == 4 && this.status == 200)
        {
          document.getElementById("TVOC").innerHTML = this.responseText;
        }
      };
      request.open("GET", "readTVOC", true);
      request.send();
    }

  </script>
</body>
</html>
)***";
