#include "WiFiEsp.h"
#include "SoftwareSerial.h"
#include "Wire.h"
#include "LiquidCrystal_I2C.h"

SoftwareSerial Serial1(8, 9);
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

/* Credenciais da rede */
char ssid[] = "Jason Voorhess";
char pass[] = "S7on&7&mpL&P1Lo7s";

int status = WL_IDLE_STATUS;

WiFiEspServer server(80);
RingBuffer buf(8);

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  lcd.begin(16, 2);

  WiFi.init(&Serial1);
  WiFi.config(IPAddress(192, 168, 15, 15));

  if (WiFi.status() == WL_NO_SHIELD)
  {
    while (true)
      ;
  }

  while (status != WL_CONNECTED)
  {
    status = WiFi.begin(ssid, pass);
  }

  server.begin();
}

void loop()
{
  WiFiEspClient client = server.available();
  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 0);
  lcd.print('oi');

  if (client)
  {
    buf.init();
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        buf.push(c);

        String message = "";
        client.find("?");
        message = client.readStringUntil(' ');
        Serial.println(message);
        //String command = message.substring(0, 1);
        lcd.print(message);
      }
      delay(15);
    }
    client.stop();
  }
}
