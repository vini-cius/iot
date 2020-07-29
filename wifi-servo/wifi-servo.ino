#include <Servo.h>
#include "WiFiEsp.h"
#include "SoftwareSerial.h"

SoftwareSerial Serial1(6, 7); //
Servo servoMotor;
WiFiEspServer server(80);
RingBuffer buf(8);

int status = WL_IDLE_STATUS;

char ssid[] = "Jason Voorhess";
char pass[] = "S7on&7&mpL&P1Lo7s";

int statusLed = LOW;
int pinoLed = 3;
int pinServo = 2;

void setup() {
  pinMode(pinoLed, OUTPUT);
  digitalWrite(pinoLed, LOW);
  servoMotor.attach(pinServo);

  Serial.begin(9600);
  Serial1.begin(9600); // initialize serial for ESP module

  WiFi.init(&Serial1);
  WiFi.config(IPAddress(192, 168, 15, 15));

  //INÍCIO - VERIFICA SE O SHIELD ESTÁ ACOPLADO AO ARDUINO, CONECTA A REDE SEM FIO E INICIA O WEBSERVER
  if (WiFi.status() == WL_NO_SHIELD) {
    while (true);
  }

  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
  }

  server.begin();
}

void loop() {
  WiFiEspClient client = server.available(); //ATENDE AS SOLICITAÇÕES DO CLIENTE

  if (client) { //SE CLIENTE TENTAR SE CONECTAR, FAZ
    buf.init();
    while (client.connected()) {
      if (client.available()) { //SE EXISTIR REQUISIÇÃO DO CLIENTE, FAZ
        char c = client.read();
        buf.push(c);

        //IDENTIFICA O FIM DA REQUISIÇÃO HTTP E ENVIA UMA RESPOSTA
        if (buf.endsWith("\r\n\r\n")) {
          sendHttpResponse(client);
          break;
        }

        if (buf.endsWith("GET /H")) {
          digitalWrite(pinoLed, HIGH);
          statusLed = 1;
          servoMotor.write(360);
        }
        else {
          if (buf.endsWith("GET /L")) {
            digitalWrite(pinoLed, LOW);
            statusLed = 0;
          }
        }
      }
    }
    client.stop(); //FINALIZA A REQUISIÇÃO HTTP E DESCONECTA O CLIENTE
  }
}

//MÉTODO DE RESPOSTA A REQUISIÇÃO HTTP DO CLIENTE
void sendHttpResponse(WiFiEspClient client) {
  client.println("HTTP/1.1 200 OK"); //ESCREVE PARA O CLIENTE A VERSÃO DO HTTP
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println("<title>Modulo WiFi ESP8266 para Arduino</title>");
  client.println("</head>");

  client.println("<body>");
  client.println("<p style='line-height:2'><font>Modulo WiFi ESP8266 para Arduino</font></p>");
  client.println("<font>ESTADO ATUAL DO LED</font>");

  if (statusLed == HIGH) {
    client.println("<p style='line-height:0'><font color='green'>LIGADO</font></p>");
    client.println("<a href=\"/L\">APAGAR</a>");
  } else {
    if (statusLed == LOW) {
      client.println("<p style='line-height:0'><font color='red'>DESLIGADO</font></p>");
      client.println("<a href=\"/H\">ACENDER</a>");
    }
  }
  client.println("</body>");
  client.println("</html>");

  delay(1);
}
