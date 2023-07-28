// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <DHT.h>



#define DHTTYPE DHT11
#define dht_dpin 10  //digital pin 4 for data output of the Temperature sensor
DHT dht (dht_dpin, DHTTYPE);



// Replace with your network credentials
const char* ssid     = "codefest";
const char* password = "OakCodefest@2019";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output13State = "off";

// Assign output variables to GPIO pins
const int output13 = 13;




void setup() {
  Serial.begin(9600);
  // Initialize the output variables as outputs
  pinMode(output13, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output13, HIGH);
 


  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
      Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
   
   float humid = dht.readHumidity(); //temperature data output
  
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
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
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /13/on") >= 0) {
              Serial.println("GPIO 13 on");
              output13State = "on";
              digitalWrite(output13, LOW);
            } else if (header.indexOf("GET /13/off") >= 0) {
              Serial.println("GPIO 13 off");
              output13State = "off";
              digitalWrite(output13, HIGH);
            } 

            /* if (header.indexOf("GET /replace") >= 0) {
              Serial.println("Timer mode");
              digitalWrite(output13, LOW);
              delay(replace); 
            }*/

            if (header.indexOf("GET /auto") >= 0){
              Serial.println("Moisture mode");
              if (humid <= 30)
              {
              digitalWrite(output13, LOW);
              }
            }
              
              
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");
            
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");

            client.println("Humidity: ");
            client.println(humid);
            client.println("<p><a href=\"/test\"><button class=\"button\">TEST</button></a></p>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>GPIO 13 - State " + output13State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output13State=="off") { 
              client.println("<p><a href=\"/13/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/13/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  

            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

