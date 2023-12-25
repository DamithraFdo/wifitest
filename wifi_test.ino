#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
//#include <Adafruit_BMP280.h>

// constants won't change. They're used here to set pin numbers, etc.
// replace with your network credentials
const char* ssid     = "kasun1";
const char* password = "12345678";
const int ledPin = 23;

// define the BMP sensor and web server
//Adafruit_BMP280 bmp;
WiFiServer server(80); // set web server port number to 80

// variables will change:
String header;
bool ledState = false;
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  //bmp.begin(0x76); // [0x76, 0x77]
  // connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // print private IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            client.println("<!DOCTYPE html>");
              client.println("<html>");
                client.println("<head>");
                  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                  client.println("<link rel=\"icon\" href=\"data:,\">");
                  client.println("<style>");
                    client.println("h1 { border-bottom: 1px solid #c0c0c0; margin-bottom: 10px; padding-bottom: 10px; white-space: nowrap; }");
                    client.println("table { border-collapse: collapse; }");
                    client.println("th { cursor: pointer; }");
                    client.println("td { padding-inline-start: 2em; text-align: end; white-space: nowrap; }");
                    client.println("button { border: none; color: white; padding: 16px 40px; text-decoration: none; margin: 2px; cursor: pointer; }");
                  client.println("</style>");
                client.println("</head>");
                client.println("<body>");
                // BMP280 page
                if (header.indexOf("GET /bmp280") >= 0) {
                  client.println("<h1>ESP32 Web Server with BMP280</h1>");
                  client.println("<a href=\"./\">Back</a>");
                  client.println("<br></br>");
                  client.println("<table>");
                    client.println("<tr>");
                      client.println("<th>Measurement</th>");
                      client.println("<th>Value</th>");
                    client.println("</tr>");
                    client.println("<tr>");
                      client.println("<td>Temp. Celsius</td>");
                      client.println("<td>");
                        //client.print(bmp.readTemperature());
                        client.println(" &#176;C");
                      client.println("</td>");
                    client.println("</tr>");
                    client.println("<tr>");
                      client.println("<td>Temp. Fahrenheit</td>");
                      client.println("<td>");
                       // client.print(1.8 * bmp.readTemperature() + 32);
                        client.println(" &#176;F");
                      client.println("</td>");
                    client.println("</tr>");
                    client.println("<tr>");
                      client.println("<td>Pressure</td>");
                      client.println("<td>");
                        //client.print(bmp.readPressure());
                        client.println(" Pa");
                      client.println("</td>");
                    client.println("</tr>");
                    client.println("<tr>");
                      client.println("<td>Approx. Altitude</td>");
                      client.println("<td>");
                        //client.print(bmp.readAltitude());
                        client.println(" m");
                      client.println("</td>");
                    client.println("</tr>");
                  client.println("</table>");
                // LED page
                } else if (header.indexOf("GET /led") >= 0) {
                  client.println("<h1>ESP32 Web Server with LED</h1>");
                  client.println("<a href=\"../\">Back</a>");
                  // when LED on, show "turn off" button
                  if (header.indexOf("GET /led/on") >= 0) {
                    digitalWrite(ledPin, HIGH);
                    client.println("<h3>LED is on</h3>");
                    client.println("<a href=\"/led/off\"><button style=\"background-color: #bf1919;\">Turn it off</button></a>");
                  // when LED off, show "turn on" button
                  } else {
                    digitalWrite(ledPin, LOW);
                    client.println("<h3>LED is off</h3>");
                    client.println("<a href=\"/led/on\"><button style=\"background-color: #48bf19;\">Turn it on</button></a>");
                  }
                // default page
                } else {
                  client.println("<h1>ESP32 Web Server</h1>");
                  client.println("<table>");
                    client.println("<tr>");
                      client.println("<th>Name</th>");
                      client.println("<th>Link</th>");
                    client.println("</tr>");
                    client.println("<tr>");
                      client.println("<td><a href=\"/led/off\">LED</a></td>");
                      client.println("<td>/led/off</td>");
                    client.println("</tr>");
                    client.println("<tr>");
                      client.println("<td><a href=\"/bmp280\">BMP280</a></td>");
                      client.println("<td>/bmp280</td>");
                    client.println("</tr>");
                  client.println("</table>");
                }
                client.println("</body>");
              client.println("</html>");
            client.println();
            // break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // clear the header variable
    header = "";
    // close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
