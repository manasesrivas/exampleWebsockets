#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <WiFi.h>


AsyncWebServer server(80);
WebSocketsServer Websockets(81);

#define ledPin 2

const char* page="<!doctype html>"
"<html lang=en>"
"<head>"
"<meta charset=UTF-8>"
"<meta name=viewport content='width=device-width,initial-scale=1'>"
"<title>Document</title>"
"</head>"
"<body>"
"<div style='text-align:center'>"
"<h1>ESP32 control panel</h1>"
"<h3>led</h3>"
"<button onclick='turnOn()'>encender</button>"
"<button onclick='turnOff()'>apagar</button>"
"</div>"
"<script>let socketConnetion=new WebSocket(`ws://${location.hostname}:81/`);const turnOff=()=>socketConnetion.send('ledoff'),turnOn=()=>socketConnetion.send('ledon')</script>"
"</body>"
"</html>";


void notFound(AsyncWebServerRequest *request){
    request->send(404, "text/plain", "pagina no encontrada");
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t lenght){
    switch(type){
        case WStype_DISCONNECTED:
            Serial.printf("[%u]] desconectado\n", num);
            break;
        
        case WStype_CONNECTED:
            {
                IPAddress ip = Websockets.remoteIP(num);
                Serial.printf("[%u] conectado en %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
                Websockets.sendTXT(num, "conectado en servidor: ");
            }
            break;
        
        case WStype_TEXT:
            Serial.printf("[%u] mensaje recibido: %s\n", num, payload);
            String msg = String((char *)(payload));

            if(msg.equalsIgnoreCase("ledon")){
                digitalWrite(ledPin, HIGH);
            }
            if(msg.equalsIgnoreCase("ledoff")){
                digitalWrite(ledPin, LOW);
            }
    }   
}


void setup(){
    Serial.begin(115200);
    Serial.println("INICIANDO COMUNICACION SERIAL");

    WiFi.softAP("manases", "");
    Serial.println("\n\n\nmanases");
    Serial.println(WiFi.softAPIP());

    


    Serial.println("\n\n\niniciando pines");
    pinMode(ledPin, OUTPUT);

    

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {request->send(200, "text/html", page);});
    
    server.onNotFound(notFound);
    server.begin();

    Websockets.begin();
    Websockets.onEvent(webSocketEvent);
    
}

void loop(){
    Websockets.loop();
}
