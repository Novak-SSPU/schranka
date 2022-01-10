#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> // Knihovna pro Telegram bota od Briana Lougha: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>
#define Hall_sensor 35 //definace pinů pro Hallovy sensory
#define Hall_sensor2 34
int Hal1 = 0;
int Hal2= 0;

const char *wifi_jmeno = "NOVACI_WIFI";  //Název Wi-fi
const char *heslo = "K@TK@&F1L1P&KUB1K"; //heslo na Wi-fi

// Nastavení pro bota
#define BOTtoken "2107324741:AAELdq0KJMGP6mD7PMTysN3Lx1Afgvswpks" //Telegram ID mojeho bota

// Přes @myidbot na Telegramu jsem vyhledal svoje Telegram ID
#define CHAT_ID "2110767400" //Zapsání mého telegram ID (popřípadě od jiného uživatele) pro bota k rozeznání

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

//Kontroluje pro nové zprávy každou vteřinu.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int ledPin = 2;
bool ledState = LOW;

// Pracuje s novými zprávami.
void handleNewMessages(int numNewMessages)
{
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++)
  {
    // Kontroluje správnost uživatelského ID
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID)
    {
      bot.sendMessage(chat_id, "Neznámý uživatel", "");
      continue;
    }

    // Vypíše poslanou zprávu
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") //bot při zprávě /start pošle následující zprávu
    {
      String welcome = "Ahoj, " + from_name + ".\n";
      welcome += "Použij následující příkazy pro ovládání bota\n";
      welcome += "/stav pro momentální stav schránky.\n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/stav") //bot podle hodnoty Hallova senzoru pošle zprávu o stavu schránky
    {
      if (digitalRead(Hal1))
      {
        bot.sendMessage(chat_id, "Schránka je otevřená", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Schránka je zavřená", "");
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  // Připojování k Wi-fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_jmeno, heslo);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Přidání kořenového certifikátu pro api.telegram.org
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Připojování se k WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  pinMode(Hall_sensor, INPUT);
  pinMode(Hall_sensor2, INPUT);
}
void postaIn(int value)
{
  String chat_id = CHAT_ID;
  if (value == 0)
  {
    bot.sendMessage(chat_id, "Do schránky byla vložena pošta.", "");
    delay(5000);
  }
}

void postaOut(int value)
{
  String chat_id = CHAT_ID;
  if (value == 0)
  {
    bot.sendMessage(chat_id, "Schránka byla vybrána.", "");
    delay(5000);
  }
}

void loop()
{

  if (millis() > lastTimeBotRan + botRequestDelay)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }

  Hal1 = digitalRead(Hall_sensor); //Čtení a zapsání hodnoty Z Hallova sensoru
  Hal2 = digitalRead(Hall_sensor2);
  Serial.print(Hal1);
  Serial.print("\t");
  Serial.print(Hal2);
  Serial.print("\t");
  delay(1000);

  postaIn(Hal1);
  postaOut(Hal2);


}
