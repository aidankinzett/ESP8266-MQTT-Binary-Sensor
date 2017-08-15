/*
 ESP8266 MQTT Binary Sensor by Aidan Kinzett

 Broadcast the state of a switch over the MQTT protocol.
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* wifi_ssid = "";
const char* wifi_password = "";

const char* mqtt_server = "";
const char* mqtt_user = "";
const char* mqtt_password = "";

const int REED_PIN = D2; // Pin connected to reed switch
const int LED_PIN = LED_BUILTIN; // LED pin - active-low (for testing)

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  pinMode(REED_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
}

void setup_wifi() {
  delay(10);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Attempt to connect
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

long lastMsg = 0;

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    int proximity = digitalRead(REED_PIN); // Read the state of the switch
    if (proximity == LOW) // If the pin reads low, the switch is closed.
    {
      Serial.println("Switch closed");
      client.publish("sensor/window", "closed", true);
      digitalWrite(LED_PIN, HIGH);
    }
    else
    {
      Serial.println("Switch open");
      client.publish("sensor/window", "open", true);
      digitalWrite(LED_PIN, LOW);
    }
}
}


