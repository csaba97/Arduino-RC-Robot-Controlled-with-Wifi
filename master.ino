#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h> 
#include <Wire.h>

int distance = 0;
int len = 0;
/* Set these to your desired credentials. */
const char *ssid = "yournetwork";
const char *password = "minimum8chars";
int milisInterval = 2000;
int count = 0;
int buzerPin = D3; // pin for attaching the piezoelectric speaker
ESP8266WebServer server(80);

void handleRoot() {
  String html ="<html> <head> <meta name=\"viewport\" content=\"width=device-width, minimumscale=1.0, maximum-scale=1.0, initial-scale=1\"> <style> body { text-align: center; max-width: 400px; margin: 10px auto; } #updateView { max-width: 400px; display: none; margin-top: 30px; } #dataVals { max-width: 400px; display: block; margin-top: 30px; } .convertButton { clear: both; position: relative; margin-top: 30px; -moz-box-shadow: 0px 1px 0px 0px #fff6af; -webkit-box-shadow: 0px 1px 0px 0px #fff6af; box-shadow: 0px 1px 0px 0px #fff6af; background: -webkit-gradient(linear, left top, left bottom, color-stop(0.05, #ffec64), color-stop(1, #ffab23)); background: -moz-linear-gradient(top, #ffec64 5%, #ffab23 100%); background: -webkit-linear-gradient(top, #ffec64 5%, #ffab23 100%); background: -o-linear-gradient(top, #ffec64 5%, #ffab23 100%); background: -ms-linear-gradient(top, #ffec64 5%, #ffab23 100%); background: linear-gradient(to bottom, #ffec64 5%, #ffab23 100%); filter: progid:DXImageTransform.Microsoft.gradient(startColorstr='#ffec64', endColorstr='#ffab23', GradientType=0); background-color: #ffec64; -moz-border-radius: 6px; -webkit-border-radius: 6px; border-radius: 6px; border: 1px solid #ffaa22; display: inline-block; cursor: pointer; color: #333333; font-family: Arial; font-size: 21px; padding: 14px 69px; text-decoration: none; text-shadow: 0px 1px 0px #ffee66; } .convertButton:hover { background: -webkit-gradient(linear, left top, left bottom, color-stop(0.05, #ffab23), color-stop(1, #ffec64)); background: -moz-linear-gradient(top, #ffab23 5%, #ffec64 100%); background: -webkit-linear-gradient(top, #ffab23 5%, #ffec64 100%); background: -o-linear-gradient(top, #ffab23 5%, #ffec64 100%); background: -ms-linear-gradient(top, #ffab23 5%, #ffec64 100%); background: linear-gradient(to bottom, #ffab23 5%, #ffec64 100%); filter: progid:DXImageTransform.Microsoft.gradient(startColorstr='#ffab23', endColorstr='#ffec64', GradientType=0); background-color: #ffab23; } .convertButton:active { position: relative; top: 1px; } </style> </head> <body> <a ontouchstart=\"park()\" onmousedown=\"park()\"><button style=\"height:100px;width:100px\">Park Robot</button></a> <a ontouchstart=\"sound()\" onmousedown=\"sound()\"><button style=\"height:100px;width:100px\">Sound</button></a> <div id=\"dataVals\"> <h1>Distance</h1> <div id=\"light\">2</div> <h1>Parking place width</h1> <div id=\"countNum\">0</div> </div> <div id=\"updateView\"> <label for=\"fader\">Update rate in Milliseconds</label><br> <input type=\"range\" style=\"width: 300px\" min=\"50\" max=\"5000\" value=\"50\" id=\"fader\" step=\"1\" oninput=\"outputUpdate(value)\"> <output for=\"fader\" id=\"volume\">50</output></div> <script> var changed = false; function changeView() { if (changed == false) { document.getElementById('updateView').style.display = 'block'; document.getElementById('dataVals').style.display = 'none'; document.getElementById('viewButton').innerHTML = \"Show Data\"; changed = true; } else { document.getElementById('updateView').style.display = 'none'; document.getElementById('dataVals').style.display = 'block'; document.getElementById('viewButton').innerHTML = \"Update Rate\"; changed = false; } } function loadDoc() { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function() { if (this.readyState == 4 && this.status == 200) { var obj = JSON.parse(this.responseText); document.getElementById(\"light\").innerHTML = obj.data[0].dataValue; document.getElementById(\"countNum\").innerHTML = obj.data[1].dataValue; } }; xhttp.open(\"GET\", \"/data\", true); xhttp.send(); } var timedEvent = setInterval(function() { loadDoc(); }, 250); function outputUpdate(vol) { clearInterval(timedEvent); timedEvent = setInterval(function() { loadDoc(); }, vol); document.querySelector('#volume').value = vol; delayVal = vol; } </script> <script> function updateSpeed(val){ document.querySelector('#volumeSpeed').value = val; var url = \"/speed?speed=\"+val; var xhttp = new XMLHttpRequest(); xhttp.open(\"GET\",url , true); xhttp.send(); } function updateSpeedDif(val){ document.querySelector('#volumeSpeedDif').value = val; var url = \"/speedDif?speed=\"+val; var xhttp = new XMLHttpRequest(); xhttp.open(\"GET\",url , true); xhttp.send(); } function sound() { var xhttp = new XMLHttpRequest(); xhttp.open(\"GET\", \"/sound\", true); xhttp.send(); } function park() { var xhttp = new XMLHttpRequest(); xhttp.open(\"GET\", \"/park\", true); xhttp.send(); } function mouseDown1() { var xhttp = new XMLHttpRequest(); xhttp.open(\"GET\", \"/button1\", true); xhttp.send(); } function mouseUp1() { var xhttp = new XMLHttpRequest(); xhttp.open(\"GET\", \"/button1off\", true); xhttp.send(); } function mouseDown2() { var xhttp = new XMLHttpRequest(); xhttp.open(\"GET\", \"/button2\", true); xhttp.send(); } function mouseUp2() { var xhttp = new XMLHttpRequest(); xhttp.open(\"GET\", \"/button2off\", true); xhttp.send(); } function mouseDown3() { var xhttp = new XMLHttpRequest(); xhttp.open(\"GET\", \"/button3\", true); xhttp.send(); } function mouseUp3() { var xhttp = new XMLHttpRequest(); xhttp.open(\"GET\", \"/button3off\", true); xhttp.send(); } function mouseDown4() { var xhttp = new XMLHttpRequest(); xhttp.open(\"GET\", \"/button4\", true); xhttp.send(); } function mouseUp4() { var xhttp = new XMLHttpRequest(); xhttp.open(\"GET\", \"/button4off\", true); xhttp.send(); } </script> <div id=\"speedDif\"> <label for=\"fader\">Speed Difference</label><br> <input type=\"range\" style=\"width: 300px;height: 60px\" min=\"-50\" max=\"50\" value=\"0\" id=\"fader\" step=\"1\" oninput=\"updateSpeedDif(value)\"> <output for=\"fader\" id=\"volumeSpeedDif\">0</output></div> <div id=\"speedL\"> <label for=\"fader\">Speed</label><br> <input type=\"range\" style=\"width: 300px;height: 60px\" min=\"50\" max=\"150\" value=\"50\" id=\"fader\" step=\"1\" oninput=\"updateSpeed(value)\"> <output for=\"fader\" id=\"volumeSpeed\">50</output></div> <div> <a ontouchend=\"mouseUp1()\" ontouchstart=\"mouseDown1()\" onmousedown=\"mouseDown1()\" onmouseup=\"mouseUp1()\"><button style=\"margin:auto;display:block;height:100px;width:100px\"></button></a> </div> <div> <a ontouchend=\"mouseUp2()\" ontouchstart=\"mouseDown2()\" onmousedown=\"mouseDown2()\" onmouseup=\"mouseUp2()\"><button style=\"float: left;height:100px;width:100px\"></button></a> </div> <div> <a ontouchend=\"mouseUp3()\" ontouchstart=\"mouseDown3()\" onmousedown=\"mouseDown3()\" onmouseup=\"mouseUp3()\"><button style=\"float: right;height:100px;width:100px\"></button></a> </div> <p>&nbsp;</p> <div> <a ontouchend=\"mouseUp4()\" ontouchstart=\"mouseDown4()\" onmousedown=\"mouseDown4()\" onmouseup=\"mouseUp4()\"><button style=\"margin:auto;display:block;height:100px;width:100px\"></button></a> </div> <span style=\"margin: 0px auto; border: 2px dotted rgb(0, 0, 0); position: absolute; z-index: 2147483647; visibility: hidden; left: 492px; width: 0px; top: 414px; height: 0px;\"></span> <span style=\"z-index: 2147483647; position: absolute; visibility: hidden; left: 477px; width: 50px; top: 399px; height: 20px; font-size: 10px; color: black;\"></span> </body> </html>";
  server.send(200, "text/html", html);
}

void getData() {   
   //This is a JSON formatted string that will be served. You can change the values to whatever like.
   // {"data":[{"dataValue":"1024"},{"dataValue":"23"}]} This is essentially what is will output you can add more if you like
  Wire.requestFrom(8, 2);
     if(Wire.available()){
        //read distance
        distance = Wire.read();
        //Serial.print("distance ="); 
        //Serial.println(distance);
        //read len
        len = Wire.read();
        //Serial.print("len ="); 
        //Serial.println(len);
     }
     
  String text2 = "{\"data\":[";
  text2 += "{\"dataValue\":\"";
  text2 += distance;
  text2 += "\"},";
  text2 += "{\"dataValue\":\"";
  text2 += len;
  text2 += "\"}";
  text2 += "]}";
  server.send(200, "text/html", text2);
}

void handleButton1() {                         
  Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write("#output1on");  /* sends hello string */
  Wire.endTransmission();    /* stop transmitting */
  server.send(200, "text/html", "success1");
}

void handleButton1off() {                         
  Wire.beginTransmission(8); /* begin with device address 8 */
 Wire.write("#output1off");  /* sends hello string */
 Wire.endTransmission();    /* stop transmitting */ 
  server.send(200, "text/html", "success12");
}

void handleButton2() { 
  Serial.println("left");                     
   Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write("#output2on");  /* sends hello string */
  Wire.endTransmission();    /* stop transmitting */
  server.send(200, "text/html", "success2");
}

void handleButton2off() {                    
  Wire.beginTransmission(8); /* begin with device address 8 */
 Wire.write("#output2off");  /* sends hello string */
 Wire.endTransmission();    /* stop transmitting */ 
  server.send(200, "text/html", "success22");
}

void handleButton3() { 
  Serial.println("right");                     
   Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write("#output3on");  /* sends hello string */
  Wire.endTransmission();    /* stop transmitting */
  server.send(200, "text/html", "success3");
}

void handleButton3off() {                    
 Wire.beginTransmission(8); /* begin with device address 8 */
 Wire.write("#output3off");  /* sends hello string */
 Wire.endTransmission();    /* stop transmitting */ 
 server.send(200, "text/html", "success32");
}

void handleButton4() {                         
  Wire.beginTransmission(8); /* begin with device address 8 */
  Wire.write("#output4on");  /* sends hello string */
  Wire.endTransmission();    /* stop transmitting */
  server.send(200, "text/html", "success4");
}

void handleButton4off() {                         
 Wire.beginTransmission(8); /* begin with device address 8 */
 Wire.write("#output4off");  /* sends hello string */
 Wire.endTransmission();    /* stop transmitting */ 
 server.send(200, "text/html", "success42");
}

void handleParking(){
  Serial.println("park");
 Wire.beginTransmission(8); /* begin with device address 8 */
 Wire.write("#park");  /* sends hello string */
 Wire.endTransmission();    /* stop transmitting */ 
 server.send(200, "text/html", "Parking started");
}

void handleSound(){
 analogWrite(buzerPin, 50);
 delay(500);
 analogWrite(buzerPin, 0);
 Serial.println("sound...");
 server.send(200, "text/html", "sound");
}


void handleSpeed(){
 Serial.print("speed changed to...");
 if (server.arg("speed") != ""){
    int newSpeed = server.arg("speed").toInt();
    Serial.println(newSpeed);
    Wire.beginTransmission(8); /* begin with device address 8 */
    String outp = "#speed"+String(newSpeed);
    Wire.write(outp.c_str());  /* sends hello string */
    Wire.endTransmission();    /* stop transmitting */ 
    server.send(200, "text/html", "speed_parameter_given");
  }
 else server.send(200, "text/html", "speed_parameter_not_found");
}


void handleSpeedDif(){
 Serial.print("speed difference changed to...");
 if (server.arg("speed") != ""){
    int newSpeed = server.arg("speed").toInt();
    Serial.println(newSpeed);
    Wire.beginTransmission(8); /* begin with device address 8 */
    String outp = "#speedDif"+String(newSpeed);
    Wire.write(outp.c_str());  /* sends hello string */
    Wire.endTransmission();    /* stop transmitting */ 
    server.send(200, "text/html", "speed_parameter_given");
  }
 else server.send(200, "text/html", "speed_parameter_not_found");
}

void setup() {
  //
   pinMode(buzerPin, OUTPUT);
   //i2c
   Serial.begin(9600); /* begin serial for debug */
   Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
   //
   //
  delay(1000);
  //Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.on("/data", getData);
  server.on("/button1", handleButton1);
  server.on("/button1off", handleButton1off);
  server.on("/button2", handleButton2);
  server.on("/button2off", handleButton2off);
   server.on("/button3", handleButton3);
  server.on("/button3off", handleButton3off);
  server.on("/button4", handleButton4);
  server.on("/button4off", handleButton4off);
  server.on("/park", handleParking);
  server.on("/sound", handleSound);
  server.on("/speed", handleSpeed);
  server.on("/speedDif", handleSpeedDif);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
 server.handleClient();
}
