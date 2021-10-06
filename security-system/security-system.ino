#include <SPI.h>
#include <Ethernet.h>

#define BUZZER_PIN 8
#define CAMERA_PIN 7

// replace the MAC address below by the MAC address printed on a sticker on the Arduino Shield 2
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EthernetClient client;

int    HTTP_PORT   = 80;
String HTTP_METHOD = "GET";
char   HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME   = "/trigger/door_opened/with/key/kumJif1RkSz9cRaL33g20lYpLocuDZOJdNyauL1BL_h";
bool armed = false;
bool door_opened = false;
bool turn_alarm_off = false;

void setup() 
{
  Serial.begin(9600);
  initialiseEthernet();
}

void loop() 
{
  if (armed && door_opened) {
    sendEmail();
    alarmBuzzer();
    triggerCamera();
  }
}





//Initialise Ethernet Shield, connects to web server
void initialiseEthernet(void)
{
  // initialize the Ethernet shield using DHCP:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to obtaining an IP address using DHCP");
    while(true);
  }

  // connect to web server on port 80:
  if(client.connect(HOST_NAME, HTTP_PORT)) {
    // if connected:
    Serial.println("Connected to server");

  } else {// if not connected:
    Serial.println("connection failed");
  }
}


//Sends an email by triggering a webhook, also reads the incoming message from the server and prints to serial monitor
void sendEmail(void) 
{
    // make a HTTP request:
    // send HTTP header
    client.println("GET " + PATH_NAME + " HTTP/1.1");
    client.println("Host: " + String(HOST_NAME));
    client.println("Connection: close");
    client.println(); // end HTTP header

    while(client.connected()) {
      if(client.available()){
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        Serial.print(c);
      }
    }
}

//Disconnects client from internet
void disconnect(void)
{
    client.stop();
    Serial.println("Disconnected");
}


void alarmBuzzer(void)
{
  while (armed && !turn_alarm_off) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite(BUZZER_PIN, LOW);
    delay(500);
}

void triggerCamera(void)
{
  digitalWrite(CAMERA_PIN, HIGH);
  delay(500);
  digitalWrite(CAMERA_PIN, LOW);
}