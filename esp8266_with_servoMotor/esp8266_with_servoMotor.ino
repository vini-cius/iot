#include <SoftwareSerial.h>
#include <Servo.h>

#define DEBUG true
SoftwareSerial esp8266(8,9); //definir pino RX = 5 e TX = 4 (do ESP8266)

const int pinoServo = 10; 
Servo S;
int pos;

void setup(){
    //definir servos e definir posição inicial
    S.attach(pinoServo);
    S.write(0);
   
  //iniciar comunicação serial
  Serial.begin(9600);
  esp8266.begin(9600);

  sendData("AT+RST\r\n", 2000, DEBUG); //resetar modulo
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG); //definir modo station
  sendData("AT+CWJAP=\"Jason Voorhess\",\"S7on&7&mpL&P1Lo7s\"\r\n", 2000, DEBUG); //conectar rede wi-fi
  //sendData("AT+CWJAP=\"Redmi\",\"vinic987\"\r\n", 2000, DEBUG); //conectar rede wi-fi
  while(!esp8266.find("OK")) { 
    //aguardar conexão
  }
   
  sendData("AT+CIFSR\r\n", 1000, DEBUG); //mostrar endereço IP
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG); //permitir múltiplas conexões
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); //iniciar web server na porta 80
}


void loop(){
  if (esp8266.available()){  //verificar se existem dados disponíveis no ESP8266
    if (esp8266.find("+IPD,")){ //se houver um novo comando
      String msg;
      esp8266.find("?"); //correr o cursor até encontrar o conteúdo da mensagem
      msg = esp8266.readStringUntil(' '); //ler a mensagem
      String command = msg.substring(0, 3); //os comandos são informados nos três primeiros caracteres.
      String valueStr = msg.substring(4);   //os 3 caracteres seguintes informam o ângulo desejado

      int value = valueStr.toInt();

      // mover os servos
      if(command == "sr1") {
        S.attach(pinoServo);
        for(pos = 0; pos < 180; pos++){ 
            S.write(pos);
            delay(15);
        }
        delay(10);
         for(pos = 180; pos >= 0; pos--){ 
         S.write(pos);
         delay(15);
        }
      }
    }
  }
}

//Enviar dados para o ESP8266
String sendData(String command, const int timeout, boolean debug){
  String response = "";
  esp8266.print(command);
  long int time = millis();

  while ((time + timeout) > millis()) {
    while (esp8266.available()){
      char c = esp8266.read();
      response += c;
    }
  }
  if (debug) {
    Serial.print(response);
  }
  return response;
}
