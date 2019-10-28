/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 Basis: PubSubClient
*/

/*
 * useful resources:
 * - Makerguides.com: How to use HC-SR501 PIR Motion Sensor with Arduino
 *   https://www.makerguides.com/hc-sr501-arduino-tutorial/
 *   
 * - Andreas Spiess at his best: #97 PIR Sensors: Test and comparison. Is the bigger better?
 *   https://www.youtube.com/watch?v=0zgY5x9Zq3M
 *   
 * - HENRY'S BENCH: Arduino HC-SR501 Motion Sensor Tutorial
 *   http://henrysbench.capnfatz.com/henrys-bench/arduino-sensors-and-input/arduino-hc-sr501-motion-sensor-tutorial/
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "credentials.h"

//device anpassen - 1/3
const char* device_name = "pir";

//mqtt 
const char* mqtt_server = "robbi";
// name anpassen - 2/3
const char* mqtt_clientname = device_name;
const char* mqtt_user = device_name;
//topic anpassen - 3/3
const char* mqtt_sub_topic = "devices/pir/soll";
const char* mqtt_pub_topic = "devices/pir/ist";
const char* mqtt_debug_topic = "devices/pir/debug";

//board
const int relayPin = D1;
const int pirPin = D2;

int bewegung = HIGH;
int bewegung_alt = HIGH;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

void setup() {
  pinMode(relayPin, OUTPUT);     // Initialize the relayPin pin as an output
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pirPin, INPUT);        // Initialize the pirPin as an input
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void setup_wifi() {

//??kjm, 2017-10-29  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(relayPin, HIGH);  
    digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level)
    snprintf (msg, 75, "Aktueller Status des Relais: ein");
  } else {
    digitalWrite(relayPin, LOW);   
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
    snprintf (msg, 75, "Aktueller Status des Relais: aus");
  }
  client.publish(mqtt_pub_topic, msg);
  Serial.println(msg);

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_clientname, mqtt_user, mqtt_password)) {
      // Once connected, publish an announcement...
      strcat (msg, device_name);
      strcat (msg, " connected");
      Serial.println(msg);
      client.publish(mqtt_debug_topic, msg);
      // ... and resubscribe
      client.subscribe(mqtt_sub_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  bewegung = digitalRead(pirPin);
  if (bewegung == HIGH) {
    if (bewegung_alt != HIGH) {   // nur, wenn wir vorher keine Bewegung beobachtet haben
      snprintf (msg, 75, "hoppla - da hat sich etwas bewegt!");
      digitalWrite(relayPin, HIGH);  // Turn the Relay on
      digitalWrite(LED_BUILTIN, LOW);
      client.publish(mqtt_pub_topic, msg);
      Serial.println(msg);
      bewegung_alt = HIGH;
    }
    if (now - lastMsg > 500) { // fruehestens nach .5s nochmal 
      lastMsg = now;
      bewegung_alt = HIGH;
    }
  } else {
    if (bewegung_alt == HIGH) {
      snprintf (msg, 75, "keine Bewegung festgestellt");
      digitalWrite(relayPin, LOW);  // Turn the Relay off
      digitalWrite(LED_BUILTIN, HIGH);
      client.publish(mqtt_pub_topic, msg);
      Serial.println(msg);
      bewegung_alt = LOW;
    }
    if (now - lastMsg > 500) {
      lastMsg = now;
      bewegung_alt = LOW;
    }
  }
}
