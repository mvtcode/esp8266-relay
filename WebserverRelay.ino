#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "MVTIOT_ESP8266S1";
const char* password = "12345678jqk";

IPAddress local_IP(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

const int led = 0;

void handleRoot() {
  String s = "<!DOCTYPE html> <html lang='vi'> <head> <meta charset='UTF-8'> <meta name='viewport' content='width=device-width, initial-scale=1.0'> <meta http-equiv='X-UA-Compatible' content='ie=edge'> <title>Điều khiển thiết bị từ xa qua wifi</title> <style> body {font-family: Arial, Helvetica, sans-serif; max-width: 800px; margin: 0 auto; background-color: #eee; text-align: center; } h1 {font-size: 24px; } #status {font-size: 20px; margin-bottom: 1rem; } #status.on {color: #1cca1c; } #status.off {color: #F00; } #button {margin-top: 2rem; padding: 0.2rem 1rem; font-size: 16px; border: 1px solid #bbb; border-radius: 4px; cursor: pointer; } #button:focus {outline: none; } </style> <script> onloadPage = () => {const divStatus = document.getElementById('status'); const button = document.getElementById('button'); const GET = (url, callback) => {const req = new XMLHttpRequest(); req.open('GET', url); req.addEventListener('load', event => {const res = event.currentTarget; callback(res.response); }); req.addEventListener('error', event => {alert(event); }); req.send(); }; const updateStatus = status => {divStatus.innerText = status; if (status === 'on') {button.innerText = 'Off'; divStatus.classList.add('on'); divStatus.classList.remove('off'); } else {button.innerText = 'On'; divStatus.classList.add('off'); divStatus.classList.remove('on'); } }; GET('/status', status => {updateStatus(status); }); button.addEventListener('click', () => {const url = divStatus.innerText === 'on' ? 'off' : 'on'; GET('/' + url, status => {updateStatus(status); }); }); }; </script> </head> <body onload='onloadPage()'> <h1>Điều khiển thiết bị từ xa qua wifi</h1> <h3>Vui lòng liên hệ Mạc Tân - 0964 335 688</h3> <div id='status'>Đang kiểm tra...</div> <button type='button' id='button'>On/Off</button> </body> </html>";
  server.send(200, "text/html", s);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, gateway, subnet);

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/status", []() {
    server.send(200, "text/plain", (digitalRead(led) == HIGH ? "on" : "off"));
  });

  server.on("/on", []() {
    digitalWrite(led, 1);
    server.send(200, "text/plain", "on");
  });

  server.on("/off", []() {
    digitalWrite(led, 0);
    server.send(200, "text/plain", "off");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
