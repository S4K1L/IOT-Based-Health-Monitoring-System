/*
  NodeMCU MAX30100 HTML WebServer Code 
  http:://www.electronicwings.com
*/

const char html_page[] PROGMEM = R"RawString(
<!DOCTYPE html>
<html>
  <style>
    h1 {text-align: center; font-size: 30px; font-family:sans-serif;}
    p{text-align: center; color: #4CAF50; font-size: 40px;}
    body{ padding:100px 0; }

#heart {
    
    width: 100px;
    height: 90px;
    animation: heartbeat 1s infinite;
    margin:0 auto
}
#heart:before,
#heart:after {
    position: absolute;
    content: "";
    left: 50px;
    top: 0px;
    width: 50px;
    height: 80px;
    background: #c33;
    -moz-border-radius: 50px 50px 0 0;
    border-radius: 50px 50px 0 0;
    -webkit-transform: rotate(-45deg);
       -moz-transform: rotate(-45deg);
        -ms-transform: rotate(-45deg);
         -o-transform: rotate(-45deg);
            transform: rotate(-45deg);
    -webkit-transform-origin: 0 100%;
       -moz-transform-origin: 0 100%;
        -ms-transform-origin: 0 100%;
         -o-transform-origin: 0 100%;
            transform-origin: 0 100%;
}

#heart:after {
    left: 0;
    -webkit-transform: rotate(45deg);
       -moz-transform: rotate(45deg);
        -ms-transform: rotate(45deg);
         -o-transform: rotate(45deg);
            transform: rotate(45deg);
    -webkit-transform-origin: 100% 100%;
       -moz-transform-origin: 100% 100%;
        -ms-transform-origin: 100% 100%;
         -o-transform-origin: 100% 100%;
            transform-origin :100% 100%;
}

      @keyframes heartbeat
      {
        0%
        {
          transform: scale( .75 );
        }
        20%
        {
          transform: scale( 1 );
        }
        40%
        {
          transform: scale( .75 );
        }
        60%
        {
          transform: scale( 1 );
        }
        80%
        {
          transform: scale( .75 );
        }
        100%
        {
          transform: scale( .75 );
        }
      }
      
  </style>

<body>
  <h1>MAX30100 Heart Rate Sensor using NodeMCU</h1><br>
  <div id="heart"></div>
  <p>Heart rate: <span id="heartrate">0</span>bpm</p>
  <p>SpO2 : <span id="spo2">0</span>%</p>
  
<script>
  setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        const text = this.responseText;
        const myArr = JSON.parse(text);
       document.getElementById("heartrate").innerHTML = myArr[0];
        document.getElementById("spo2").innerHTML = myArr[1];
      }
    };
    xhttp.open("GET", "readmax30100", true);
    xhttp.send();
  },50);
</script>
</body>
</html>
)RawString";
