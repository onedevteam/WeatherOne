
#include <ESP8266WiFi.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

const char* ssid     = "your-wifi-ssid";     
const char* password = "your-wifi-password"; 

const char* host = "where-your-API-is" 

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
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

int value = 0;

void loop() {

if(!bmp.begin()) {
  Serial.print("Failed to read from BMP sensor!!");
  while(1);
}

  sensors_event_t event;

  float hpsTot = 0;
  float temperature;
  int i;

  //for better accuracy, 
  //we are getting an average of 5 pressure sensor readings during 1.5 seconds
  for (i = 0; i < 5; i++) {
  
      bmp.getEvent(&event);  
      Serial.print("Pressure:         ");
      Serial.print(event.pressure);
      Serial.println(" hPa ");  

      hpsTot = hpsTot + event.pressure;
      delay(300);

      Serial.println(hpsTot);
  }  

  Serial.println("hPs total: " + String(hpsTot));
  
  bmp.getTemperature(&temperature);
  Serial.print("Temperature: ");
  Serial.println(temperature);

  ++value;

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80; // or httpport of your API
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    Serial.println(host);
    return;
  }

  float hpsAvg = hpsTot / 5;
  
  // We now create a URI for the request
  String url = "/path/to/API"; 
  Serial.print("Requesting URL: ");
  Serial.println(url);


  // Create JSon object to send 
  String PostData="{";
  PostData = PostData + //json string 
  PostData=PostData+"}"; 

    Serial.println(PostData);
  
  // This will send the request to the server
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                "User-Agent: Arduino/1.0" + "\r\n" + 
                "Host: " + host + "\r\n" + 
                "Content-Type: application/json;" + "\r\n" + 
                "Content-Length:" +  PostData.length() + "\r\n" +
                "Connection: close\r\n\r\n");
  client.print(PostData + "\r\n\r\n");
                
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  delay(300000);

}

