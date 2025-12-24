#include <WiFi.h>
#include "DHT.h"

// Replace with your Wi-Fi credentials
const char* ssid = "ur ssid";
const char* password = "password";

#define RELAY_PIN 15  // Relay for bulb control
#define ENA_PIN 18    // PWM pin for fan speed control
#define IN1_PIN 16    // Motor driver IN1
#define IN2_PIN 17    // Motor driver IN2
#define DHT_PIN 19    // Pin for DHT11 sensor
#define DHT_TYPE DHT11
#define MQ2_PIN 26    // Pin for MQ2 analog output (A0, GPIO 26)


DHT dht(DHT_PIN, DHT_TYPE);


bool bulbState = false;   // Track bulb state (ON/OFF)
int fanSpeed = 0;         // Track fan speed (0 to 100%)
float temperature = 0;    // Track current temperature
bool geyserState = false; // Track geyser state
int gasLevel = 0;         // Current gas level from MQ2 sensor
bool gasDetected = false;
bool smokeDetected = false;
WiFiServer server(80);

void setup() {

  Serial.begin(115200);
  while (!Serial);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);   pinMode(ENA_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  digitalWrite(IN1_PIN, HIGH); 
  digitalWrite(IN2_PIN, LOW);


  analogWrite(ENA_PIN, 0);
  pinMode(MQ2_PIN, INPUT); 

  dht.begin();


  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());


  server.begin();
  Serial.println("Server started. Listening for incoming clients...");
}

void loop() {

  WiFiClient client = server.accept();

  if (client) {
    Serial.println("New client connected!");


    String request = client.readStringUntil('\r');
    Serial.print("Request: ");
    Serial.println(request);
    client.flush(); 


    if (request.indexOf("/bulb/on") != -1) {
      bulbState = true;
      digitalWrite(RELAY_PIN, HIGH); 
      Serial.println("Bulb turned ON.");
    } else if (request.indexOf("/bulb/off") != -1) {
      bulbState = false;
      digitalWrite(RELAY_PIN, LOW); 
      Serial.println("Bulb turned OFF.");
    }


    if (request.indexOf("/fan?speed=") != -1) {
      int index = request.indexOf('=') + 1;
      String speedString = request.substring(index, request.indexOf(' ', index));
      fanSpeed = constrain(speedString.toInt(), 0, 100); // Ensure speed is 0-100%
      int pwmValue = map(fanSpeed, 0, 100, 0, 255); // Map speed to PWM range
      analogWrite(ENA_PIN, pwmValue); // Set fan speed using PWM
      Serial.print("Fan speed set to: ");
      Serial.print(fanSpeed);
      Serial.println("%");
    }


    temperature = dht.readTemperature();


    if (isnan(temperature)) {
      Serial.println("Failed to read temperature from DHT sensor.");
      temperature = 0;
    }


    gasLevel = analogRead(MQ2_PIN); 
    gasDetected = gasLevel > 120; 
    if (gasDetected) {
      if (temperature >= 32) {
        smokeDetected = true;  // High temperature with gas indicates smoke/fire
      } else {
        smokeDetected = false; // Moderate temperature with gas indicates gas leakage
      }
    } else {
      smokeDetected = false;
    }

    // Control geyser based on temperature
    if (temperature < 27) {
      geyserState = true; // Turn on geyser
    } else {
      geyserState = false; // Turn off geyser
    }

    // Generate HTML response
    String htmlResponse = "<html><head><style>";
    htmlResponse += "@import url('https://fonts.googleapis.com/css2?family=Poppins:wght@400;700&display=swap');";
    htmlResponse += "body { font-family: 'Poppins', sans-serif; background-color: #f2f2f2; text-align: center; }";
    htmlResponse += "h1 { font-size: 36px; font-weight: bold; color: #333; }";
    htmlResponse += ".section { background-color: #fff; padding: 20px; margin: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }";
    htmlResponse += ".button { background-color: #4CAF50; color: white; padding: 10px 20px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 10px 2px; border-radius: 5px; }";
    htmlResponse += "img { width: 120px; height: auto; margin: 10px 0; }";
    htmlResponse += "input[type='number'] { width: 80px; padding: 10px; margin: 10px; border: 2px solid #4CAF50; border-radius: 5px; font-size: 16px; }";
    htmlResponse += "input[type='submit'] { background-color: #4CAF50; color: white; border: none; padding: 10px 20px; border-radius: 5px; font-size: 16px; }";

    // Alert style for fire or gas detection
    if (gasDetected || smokeDetected) {
      htmlResponse += ".alert { background-color: red; color: white; font-size: 30px; font-weight: bold; padding: 20px; margin: 20px 0; border-radius: 10px; }";
      htmlResponse += "</style></head><body>";
      htmlResponse += "<h1>Home Automation Control</h1>";
      htmlResponse += "<div class='alert'>ALERT! Fire or Gas Detected!</div>";
    } else {
      htmlResponse += "</style></head><body>";
      htmlResponse += "<h1>Home Automation Control</h1>";
    }

    // Add more sections for bulb, fan, etc.
    htmlResponse += "<div class='section'><h2>Bulb Control</h2>";
    htmlResponse += "<img src='" + String(bulbState ? "https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcRlH-yl435WQJhsIDP54SSfqv8z6G8XbSs-CQ&s" : "https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcRlH-yl435WQJhsIDP54SSfqv8z6G8XbSs-CQ&s") + "' alt='Bulb'>";
    htmlResponse += "<p>Bulb is " + String(bulbState ? "OFF" : "ON") + "</p>";
    htmlResponse += "<a class='button' href='/bulb/on'>Turn OFF</a>";
    htmlResponse += "<a class='button' href='/bulb/off'>Turn ON</a></div>";

    // Fan control section
    htmlResponse += "<div class='section'><h2>Fan Control</h2>";
    htmlResponse += "<img src='" + String(fanSpeed > 0 ? "https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcSIUJDHAOtGaazos3SN8TXy_lAHmHfX93m-3Q&s" : "https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcSIUJDHAOtGaazos3SN8TXy_lAHmHfX93m-3Q&s") + "' alt='Fan'>";
    htmlResponse += "<p>Fan speed is " + String(fanSpeed) + "%</p>";
    htmlResponse += "<form action='/fan' method='GET'>";
    htmlResponse += "Set speed (0-100): <input type='number' name='speed' min='0' max='100' value='" + String(fanSpeed) + "'>";
    htmlResponse += "<input type='submit' value='Set Speed'>";
    htmlResponse += "</form></div>";

    // Environment monitoring section
    htmlResponse += "<div class='section'><h2>Environment Monitoring</h2>";
    htmlResponse += "<p>Temperature: " + String(temperature, 1) + " °C</p>";
    htmlResponse += "<p>Gas Level: " + String(gasLevel) + "</p>";
    htmlResponse += "<p>Geyser is " + String(geyserState ? "ON" : "OFF") + "</p>";
    htmlResponse += "<p>" + String(gasDetected ? (smokeDetected ? "Smoke or Fire Detected!" : "Gas Leakage Detected!") : "No gas or smoke detected.") + "</p>";
    htmlResponse += "</div>";

    htmlResponse += "</body></html>";

    // Send the response to the client
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.print(htmlResponse);
    client.stop();
  }
}
