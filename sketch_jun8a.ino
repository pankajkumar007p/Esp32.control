/*
 * ESP32 Web Server to Control an LED
 * -----------------------------------
 * This sketch creates a simple web server on the ESP32 that allows you to
 * control the built-in LED (usually on GPIO 2) by visiting specific URLs.
 * * It is designed to work with the provided HTML control panel.
 * * Instructions:
 * 1. Update the `ssid` and `password` variables with your Wi-Fi credentials.
 * 2. Upload the code to your ESP32.
 * 3. Open the Serial Monitor at 115200 baud to find the ESP32's IP address.
 * 4. Enter this IP address into the HTML control panel to control the LED.
 */

#include <WiFi.h>
#include <WebServer.h>

// --- IMPORTANT: UPDATE WITH YOUR WI-FI CREDENTIALS ---
const char* ssid = "Airtel_pank_7587";
const char* password = "air49561";
// ----------------------------------------------------

// Create a WebServer object on port 80 (standard HTTP port)
WebServer server(80);

// Define the GPIO pin for the built-in LED. On most ESP32 dev kits, this is GPIO 2.
// If your board's LED is on a different pin, change the value here.
const int ledPin = 2;

// --- Handler for turning the LED ON ---
void handleLedOn() {
  Serial.println("GET request received for /led/on");
  digitalWrite(ledPin, HIGH); // Turn the LED on
  // Send a response back to the client. The CORS header is crucial for allowing
  // the web browser to access this server from a different origin (the HTML file).
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "LED is now ON");
}

// --- Handler for turning the LED OFF ---
void handleLedOff() {
  Serial.println("GET request received for /led/off");
  digitalWrite(ledPin, LOW); // Turn the LED off
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "LED is now OFF");
}

// --- Handler for requests to non-existent pages ---
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(404, "text/plain", message);
}

// --- Setup Function: Runs once on boot ---
void setup() {
  // Start the serial communication for debugging purposes
  Serial.begin(115200);

  // Set the LED pin as an output
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Start with the LED off

  // --- Connect to Wi-Fi ---
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // Wait for the Wi-Fi connection to be established
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // --- Connection Successful ---
  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); // This is the IP you need for the web interface

  // --- Define Server Routes ---
  // When the server receives a request to "/led/on", it will call the handleLedOn function.
  server.on("/led/on", HTTP_GET, handleLedOn);

  // When the server receives a request to "/led/off", it will call the handleLedOff function.
  server.on("/led/off", HTTP_GET, handleLedOff);
  
  // Setup the handler for 404 Not Found errors
  server.onNotFound(handleNotFound);

  // Start the server
  server.begin();
  Serial.println("HTTP server started. Ready for commands.");
}

// --- Loop Function: Runs repeatedly ---
void loop() {
  // This line is crucial. It tells the server to check if any clients
  // (like your web browser) have sent a request.
  server.handleClient();
}