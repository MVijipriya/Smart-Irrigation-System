/****************************************
 * Include Libraries
 ****************************************/
#include <WiFi.h>
#include <PubSubClient.h>

/****************************************
 * Define Constants
 ****************************************/
#define WIFISSID "Karu" // Put your WifiSSID here
#define PASSWORD "9791119742" // Put your wifi password here
#define TOKEN "A1E-FilFENRpg9jOka8CFrszBUgB0lwd2i" // Put your Ubidots' TOKEN
#define MQTT_CLIENT_NAME "3C:71:BF:4C:8F:C0" // MQTT client Name, please enter your own 8-12 alphanumeric character ASCII string; 
                                           //it should be a random and unique ascii string and different from all other devices

#define VARIABLE_LABEL_SUBSCRIBE "led" // Assing the variable label
#define DEVICE_LABEL "homeautomation" // Assig the device label

#define HOME_BULB 23 // Set the GPIO23 as LED

char mqttBroker[]  ="things.ubidots.com";//"industrial.api.ubidots.com";
char payload[100];
char topic[150];
char topicSubscribe[100];

/****************************************
 * Auxiliar Functions
 ****************************************/
WiFiClient ubidots;
PubSubClient client(ubidots);

void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);
  if (message == "1") {
    digitalWrite(HOME_BULB, LOW);
  } else {
    Serial.println("OFF");
    digitalWrite(HOME_BULB, HIGH);
  }
  
  Serial.write(payload, length);
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    
    // Attemp to connect
    if (client.connect(MQTT_CLIENT_NAME, TOKEN, "")) {
      Serial.println("Connected");
      client.subscribe(topicSubscribe);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      // Wait 2 seconds before retrying
      delay(2000);
    }
  }
}

/****************************************
 * Main Functions
 ****************************************/
void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFISSID, PASSWORD);
  // Assign the pin as OUTPUT 
  pinMode(HOME_BULB, OUTPUT);

  Serial.println();
  Serial.print("Wait for WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqttBroker, 1883);
  client.setCallback(callback);

  sprintf(topicSubscribe, "/v1.6/devices/%s/%s/lv", DEVICE_LABEL, VARIABLE_LABEL_SUBSCRIBE);
  
  client.subscribe(topicSubscribe);
}

void loop() {
  if (!client.connected()) {
    reconnect();
    client.subscribe(topicSubscribe);   
  }
  client.loop();
}
