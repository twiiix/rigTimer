/*
  Auteur: data_twiiix
  from: Bruxelles, Belgium
  BTC / ETH address: 115K3ZpjPrtkw1fyQSEM2dNzUsgrPiD7Ns / 0x6D7Adc94EEd5645467E1087B5b3CBe4F9f56E4d6
*/
// include the library code:
#include <Wire.h> // consulter la doc de Wire pour les E/S du module DS1307 en fonction de votre carte arduino
#include "RTClib.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EtherCard.h>
#include <avr/pgmspace.h>
//define variable
#define ONE_WIRE_BUS A0 //Sonde DS18B20
#define start 2         // pin qui controle l'optocoupleur
#define relais 3        //pin du relais de sécurité qui permet de coupé l'alim. du Rig
#define runCheck A1     //pin brancher sur la pin 3.3v du TMP Header de ma carte mère.
#define REQUEST_RATE 5000 // milliseconds to ethercard
char rxChar = 0;        // var pour les entrées series.
unsigned int sinceH;    //  """""""""""" contennir l'heure du dernier démarage.
unsigned int sinceM;
unsigned int sinceS;
unsigned int sinceDay;
unsigned int sinceMonth;
unsigned int sinceYear;
long int timeStart;


//----------------------------------------------
// configuration a editer par l"utilisateur
// déclaration de l'horaire de démarage
const int hStart = 22; // Heurre de démarage du rig
const int mStart = 30;  // Minute (sans "0" devant le chiffre) de démarage du rig
const int sStart = 0;  // Seconde de démarage du rig

// déclaration de l'horaire d'arrêt
const int hStop = 6; // Heurre d'arrêt du rig
const int mStop = 30; // Minute (sans "0" devant le chiffre) de démarage du rig
const int sStop = 0; // Seconde d'arrêt du rig
//fin edition utilisateur
//----------------------------------------------

//config ethernet
// ethernet interface mac address
// ethernet mac address - must be unique on your network
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 };
byte Ethernet::buffer[900]; // tcp/ip send and receive buffer


//ajouter  RTClib
RTC_DS1307 rtc;
//ajout de oneWire
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//BufferFiller bfill;

int isWorking = false;
int timerWorking = true;



void logTemp(void){
  if (isWorking)
  {
    if (millis()%8 == 0)
    {
      sensors.requestTemperatures();
      float tempValue = sensors.getTempCByIndex(0);
      Serial.print("Température: ");
      Serial.print("");
      Serial.print(tempValue);
      Serial.print(" C°");
      Serial.println("");
    }
  }
}

void debuging(void){
  timeStart = millis();
  if (timeStart %(1000*60) == 0){
  Serial.println(" ");
  Serial.print("La valeur de isWorking est = a");
  Serial.print(" ");
  Serial.print(isWorking);
  Serial.println(" ");
  int runCheckValue = analogRead(runCheck);
  Serial.println(" ");
  Serial.print("runCheckValue = ");
  Serial.print(runCheckValue);
  Serial.println(" ");
  }
}

void isRunning(){ // verifie l'état de fonctionnement du rig via la lecture d'un pin sur la cm

  int runCheckValue = analogRead(runCheck);
  if (runCheckValue < 500) {
    isWorking = false;
  } else {
    isWorking = true;
  }
}

long since(){
  DateTime now = rtc.now();
  sinceH = now.hour();
  sinceM = now.minute();
  sinceS = now.second();
  sinceDay = now.day();
  sinceMonth = now.month();
}

void timers(){ // horaire du rig.
  DateTime now = rtc.now();
  if (!isWorking && timerWorking)
  {
    if (now.hour() == hStart && now.minute() == mStart && now.second() == sStart)
    {
      Serial.println("");
      Serial.print("Démarré automatiquement à:");
      Serial.print(" ");
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.print(now.second(), DEC);
      Serial.println("");
      digitalWrite(start, HIGH);
      delay(150);
      digitalWrite(start, LOW);
      delay(1000);
      isWorking = !isWorking;
      since();
    }
  }
  if (isWorking && timerWorking)
  {
    if (now.hour() == hStop && now.minute() == mStop && now.second() == sStop)
    {
      Serial.println("");
      Serial.print("Arrêté automatiquement à:");
      Serial.print(" ");
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.print(now.second(), DEC);
      Serial.println();
      digitalWrite(start, HIGH);
      delay(150);
      digitalWrite(start, LOW);
      delay(1000);
      isWorking = !isWorking;
    }
  }
}

float readTemp(){
  sensors.requestTemperatures();
  float tempValue = sensors.getTempCByIndex(0);
  return tempValue;
}

void menu(void){
  Serial.println(F(""));
  Serial.println(F("^^^ Liste des commandes: ^^^"));
  Serial.println(F("? -> Affice le Menu."));
  Serial.println(F("r -> Affiche le résumé des options du Rig."));
  Serial.println(F("d -> Démarre le Rig."));
  Serial.println(F("s -> Arrête le Rig."));
  Serial.println(F("a -> active/désactive le timer."));
}

void resumer(void){
  DateTime now = rtc.now();
  Serial.println(F(" "));
  Serial.println(F("Résumé de l'activité minière:"));
  Serial.println(F("*****************************"));
  Serial.print(F("Heure courante:"));
  Serial.print(F(" "));
  if (now.hour() < 10)
  {
    Serial.print(F("0"));
    Serial.print(now.hour(), DEC);
  }
  else
  {
    Serial.print(now.hour(), DEC);
  }
  Serial.print(':');
  if (now.minute() < 10)
  {
    Serial.print("0");
    Serial.print(now.minute(), DEC);
  }
  else
  {
    Serial.print(now.minute(), DEC);
  }
  Serial.print(':');
  if (now.second() < 10)
  {
    Serial.print(F("0"));
    Serial.print(now.second(), DEC);
  }
  else
  {
    Serial.print(now.second(), DEC);
  }
  Serial.println();
  Serial.print(F("Heure de démarrage:"));
  if (hStart < 10)
  {
    Serial.print(F("0"));
    Serial.print(hStart);
  }
  else
  {
    Serial.print(hStart);
  }
  Serial.print(':');
  if (mStart < 10)
  {
    Serial.print(F("0"));
    Serial.print(mStart);
  }
  else
  {
    Serial.print(mStart);
  }
  Serial.print(F(":"));
  if (sStart < 10)
  {
    Serial.print(F("0"));
    Serial.print(sStart);
  }
  else
  {
    Serial.print(sStart);
  }
  Serial.println();
  Serial.print(F("Heure d'arrêt:"));
  if (hStop < 10)
  {
    Serial.print(F("0"));
    Serial.print(hStop);
  }
  else
  {
    Serial.print(hStop);
  }
  Serial.print(F(":"));
  if (mStop < 10)
  {
    Serial.print(F("0"));
    Serial.print(mStop);
  }
  else
  {
    Serial.print(mStop);
  }
    Serial.print(F(":"));
  if (sStop < 10)
  {
    Serial.print(F("0"));
    Serial.print(sStop);
  }
  else
  {
    Serial.print(sStop);
  }
  Serial.println();
  Serial.print(F("Date et heure du dernier démarage:"));
  Serial.print(F(" "));
  Serial.print(sinceDay);
  Serial.print(F("/"));
  Serial.print(sinceMonth);
  Serial.print(F("/"));
  Serial.print(sinceYear);
  Serial.print(F(" - "));
  if (sinceH < 10)
  {
    Serial.print(F("0"));
    Serial.print(sinceH);
    Serial.print(F(":"));
  }
  else
  {
    Serial.print(sinceH);
    Serial.print(F(":"));
  }
  if (sinceM < 10)
  {
    Serial.print(F("0"));
    Serial.print(sinceM);
    Serial.print(F(":"));
  }
  else
  {
    Serial.print(sinceM);
    Serial.print(F(":"));
  }
  if (sinceS < 10)
  {
    Serial.print(F("0"));
    Serial.print(sinceS);
  }
  else
  {
    Serial.print(sinceS);
  }
  Serial.println(F(""));
  Serial.print(F("Le Rig tourne:"));
  if (isWorking)
  {
    Serial.print(F(" "));
    Serial.print(F("Oui!"));
    Serial.println(F(""));
  }
  else
  {
    Serial.print(F(" "));

    Serial.print(F("Non!"));
    Serial.println(F(""));
  }

  Serial.print(F("Etat du Timer:"));
  if (timerWorking)
  {

    Serial.print(" ");

    Serial.print("Activé!");
  }
  else
  {
    Serial.print(F(" "));
    Serial.print(F("Désactivé :(( - entre 'd' pour faire courir ton rig."));
  }
  Serial.println();
  Serial.print(F("Température au seins de la plate-forme:"));
  Serial.print(F(" "));
  Serial.print(readTemp());
  Serial.print(F("°"));
  Serial.print(F(" Celsius."));
  Serial.println();
}

char const page[] PROGMEM = {
    "HTTP/2.0 200 OK\r\n"
    "Content-Type: text/html\r\n"
    //"Pragma: no-cache\r\n"
    "\r\n"
    "<meta http-equiv='refresh' content='1'/>"
    "<title>RigControl</title>"
    "<h1>DashBoard Arduino Rig1</h1></br>"
    "<div>The syntax: http://192.168.0.20/?OFF or ON</div>"
    "<span id=on><a href='http://192.168.0.20/?ON'>ON</a></span></br>"
    "<a href='http://192.168.0.20/?OFF'>OFF</a>"
}
;
void setup() {
  Wire.begin();
  Serial.begin(57600);
  //setup ethernet
   // Change 'SS' to your Slave Select pin, if you arn't using the default pin
  Serial.println(F("Trying to get an IP…"));
 
  Serial.print(F("MAC: "));
  for (byte i = 0; i < 6; ++i) {
  Serial.print(mymac[i], HEX);
  if (i < 5)
  Serial.print(":");
  }
  Serial.println();
   
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
  {
  Serial.println(F("Failed to access Ethernet controller"));
  }
  else
  {
  Serial.println(F("Ethernet controller access: OK"));
  }
  
   
  #if STATIC
  Serial.println(F("Getting static IP."));
  if (!ether.staticSetup(myip, gwip)){
  Serial.println(F("could not get a static IP"));
  blinkLed(); // blink forever to indicate a problem
  }
  #else
   
  Serial.println(F("Setting up DHCP"));
  if (!ether.dhcpSetup()){
  Serial.println(F("DHCP failed"));
  //blinkLed(); // blink forever to indicate a problem
  }
  #endif
  rtc.begin();
  sensors.begin();
  //rtc.adjust(DateTime(__DATE__,__TIME__));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  pinMode(start, OUTPUT);
  pinMode(A1, INPUT);
  Serial.flush(); // effacage du buffer memoire de réception
  menu();
}

void loop(){
  //ethernet loop
    word len = ether.packetReceive();
    word pos = ether.packetLoop(len);
 
    //start rig website
    if (!isWorking){
    if(strstr((char *)Ethernet::buffer + pos, "GET /?ON") != 0) {
    Serial.println(F("Received ON command"));
    digitalWrite(start, HIGH);
    delay(150);
    digitalWrite(start, LOW);
    delay(1000);
    isWorking = !isWorking;
    }}
    if (isWorking){
    // stop rig website
    if(strstr((char *)Ethernet::buffer + pos, "GET /?OFF") != 0) {
    Serial.println(F("Received OFF command"));
    digitalWrite(start, HIGH);
    delay(150);
    digitalWrite(start, LOW);
    delay(1000);
    isWorking = !isWorking;
    }}
    
    // show some data to the user
//   memcpy_P(ether.tcpOffset(), page, sizeof page);
//   ether.httpServerReply(sizeof page - 1);
   // ether.httpServerReply(dhtValuePage()); // send web page data
  //Serial.println(readTemp());
  //logTemp();
  //    tempCheck()
  DateTime now = rtc.now();
  timers();
  isRunning();
  debuging();
  //Serial.println(runCheck);
  if (Serial.available() > 0)
  {                         //  verifie si le port serie est près a recevoir
    rxChar = Serial.read(); //  recupère les caractères passée via le voie serier dans une var.
    Serial.flush();         //  vide la mem. tempon d réception.
    switch (rxChar)
    {
    case 'a':
    case 'A':
      if (timerWorking)
      {
        Serial.println(F(" "));
        Serial.println(F("Désactivation du timer"));
        Serial.println(F(" "));
        timerWorking = !timerWorking;
      }
      else
      {
        Serial.println(F(" "));
        Serial.println(F("Réactivation du timer"));
        Serial.println(F(" "));
        timerWorking = !timerWorking;
      }
      break;
    case 'r':
    case 'R':
      resumer();
      break;
    case 'd':
    case 'D':
      if (!isWorking)
      {
        digitalWrite(start, HIGH);
        delay(150);
        digitalWrite(start, LOW);
        isWorking = !isWorking;
        since();
        delay(1000);
        Serial.println("");
        Serial.print("Démarré à:");
        Serial.print(" ");
        Serial.print(now.hour(), DEC);
        Serial.print(':');
        Serial.print(now.minute(), DEC);
        Serial.print(':');
        Serial.print(now.second(), DEC);
        Serial.println(" ");
      }
      else
      {
        Serial.println("");
        Serial.print(F("Le rig cours déjà?! "));
        Serial.println(F(""));
      }
      break;
    case 's':
    case 'S':
      if (isWorking)
      {
        digitalWrite(start, HIGH);
        delay(150);
        digitalWrite(start, LOW);
        isWorking = !isWorking;
        delay(1000);
        Serial.println(F(""));
        Serial.print(F("Arrêté à:"));
        Serial.print(" ");
        Serial.print(now.hour(), DEC);
        Serial.print(':');
        Serial.print(now.minute(), DEC);
        Serial.print(':');
        Serial.print(now.second(), DEC);
        Serial.println();
      }
      else
      {
        Serial.println(F(""));
        Serial.print(F("Le rig est déjà éteint?! non?"));
        Serial.println(F(""));
      }
      break;
    case '?':
      menu(); //appel du menu par ça fonction
      break;
    }
  }
}
