#include <SPI.h>
#include <Ethernet.h>

#define DOOR_PIN 2
#define BUZZER_PIN 8
#define CAMERA_PIN 10
#define LED_COUNTER1 3
#define LED_COUNTER2 4
#define LED_COUNTER3 5
#define LED_COUNTER4 6
#define LED_DISARMED 7
#define LED_ARMED 9
#define PINPAD A0

//mac adddress
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EthernetClient client;

int    HTTP_PORT   = 80;
String HTTP_METHOD = "GET";
char   HOST_NAME[] = "maker.ifttt.com";
String PATH_NAME   = "/trigger/Door_opened/with/key/kumJif1RkSz9cRaL33g20lYpLocuDZOJdNyauL1BL_h";
bool armed = false;
volatile bool door_opened = false;
bool runOnce = true;
int button_signal;
int button_value;
int password_input[4];
int password[4] = {1, 2, 3, 4};
int count = 0;

void setup() 
{
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_COUNTER1, OUTPUT);
  pinMode(LED_COUNTER2, OUTPUT);
  pinMode(LED_COUNTER3, OUTPUT);
  pinMode(LED_COUNTER4, OUTPUT);
  pinMode(LED_DISARMED, OUTPUT);
  pinMode(LED_ARMED, OUTPUT);
  pinMode(PINPAD, INPUT_PULLUP);
}

void loop() 
{
  if (armed) {
    door_opened = digitalRead(DOOR_PIN);
    if (door_opened) {
      if (runOnce) {
        Serial.println("Alarm set off");
        alarmBuzzer();
        initialiseEthernet();
        sendEmail();
        door_opened = false;
        runOnce = false;
      }
    }
    
  } else if (!armed) {
    door_opened = false;
    turnAlarmOff();
  }
    
  button_signal = analogRead(PINPAD);
  button_value = buttonValue(button_signal);
  if (button_value > 0) {
    passwordInput(button_value);
  }
  ledIndication();
  if (count == 4) {
    if (arrayCompare(password, password_input)) {
      armed = !armed;
      if (armed) {
        runOnce = true;
      }
    }
    count = 0;
  }
  armedLedIndication();
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
  digitalWrite(BUZZER_PIN, HIGH);
  
}

int buttonValue(int analog) 
{
  if (analog >= 768 && analog <= 848) {
    return 1;
  } else if (analog >= 573 && analog <= 653) {
    return 2;
  } else if (analog >= 373 && analog <= 453) {
    return 3;
  } else if (analog >= 177 && analog <= 257) {
    return 4;
  } else {
    return 0;
  }
}


void passwordInput(int button_value)
{
  password_input[count] = button_value;
  count++;
  delay(350);
}

bool arrayCompare(int p[], int p_in[]) 
{
  return (p[0] == p_in[0] && p[1] == p_in[1] && p[2] == p_in[2] && p[3] == p_in[3]);
}

void ledIndication(void) 
{
  if (count == 0) {
    digitalWrite(LED_COUNTER1, LOW);
    digitalWrite(LED_COUNTER2, LOW);
    digitalWrite(LED_COUNTER3, LOW);
    digitalWrite(LED_COUNTER4, LOW);
  } else if (count == 1){
    digitalWrite(LED_COUNTER1, HIGH);
    digitalWrite(LED_COUNTER2, LOW);
    digitalWrite(LED_COUNTER3, LOW);
    digitalWrite(LED_COUNTER4, LOW);
  } else if (count == 2) {
    digitalWrite(LED_COUNTER1, HIGH);
    digitalWrite(LED_COUNTER2, HIGH);
    digitalWrite(LED_COUNTER3, LOW);
    digitalWrite(LED_COUNTER4, LOW);
  } else if (count == 3) {
    digitalWrite(LED_COUNTER1, HIGH);
    digitalWrite(LED_COUNTER2, HIGH);
    digitalWrite(LED_COUNTER3, HIGH);
    digitalWrite(LED_COUNTER4, LOW);
  } else if (count == 4) {
    digitalWrite(LED_COUNTER1, HIGH);
    digitalWrite(LED_COUNTER2, HIGH);
    digitalWrite(LED_COUNTER3, HIGH);
    digitalWrite(LED_COUNTER4, HIGH);
    delay(500);
  }
}

void turnAlarmOff(void)
{
  digitalWrite(BUZZER_PIN, LOW);
}

void armedLedIndication(void)
{
  if (armed) {
    digitalWrite(LED_ARMED, HIGH);
    digitalWrite(LED_DISARMED, LOW);
  } else {
    digitalWrite(LED_ARMED, LOW);
    digitalWrite(LED_DISARMED, HIGH);
  }
}
