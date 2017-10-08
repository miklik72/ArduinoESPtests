
/* Test comunication from Arduino via ESP-01 */
#include <Arduino.h>
#include <WiFiEsp.h>
#include <HTTPclient.h>

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include <SoftwareSerial.h>
  SoftwareSerial Serial1(10, 11); // RX, TX
#endif

//wifi
#include <WifiAuth.h>                       // in file are only two lines bellow
//char ssid[] = "SIDD";                     // your network SSID (name)
//char pass[] = "...";                      // your network password
int status = WL_IDLE_STATUS;                // the Wifi radio's status

//web
char webserver[] = "www.miklik.cz";
int webport = 80;
String content;

// Initialize the Ethernet client object
WiFiEspClient client;
//HttpClient http();


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setup(){
  Serial.begin(9600);
  Serial1.begin(9600);  // ESP must be set to the same speed AT+UART_DEF=9600,8,1,0,1
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");

  printWifiStatus();

  Serial.println();

  content = "p0=10&p1=20&p2=30&p3=40";
  Serial.println("POST /z3t/insert_test.php HTTP/1.1");
  Serial.println("Host: www.miklik.cz");
  Serial.println("Content-Type: application/x-www-form-urlencoded");
  Serial.print("Content-Length: ");Serial.println(content.length());
  Serial.println(content);

  Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
  if (client.connect(webserver, webport)) {
    Serial.println("Connected to server");
    client.println("POST /insert_test.php HTTP/1.1");
    client.println("Host: z3t.miklik.cz");
    //client.println("User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64;x64; rv:57.0) Gecko/20100101 Firefox/57.0");
    //client.println("Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
    //client.println("Accept-Language: cs,en-GB;q=0.5");
    //client.println("Accept-Encoding: gzip, deflate");
    //client.println("Referer: http://www.miklik.cz/z3t/index.php");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");client.println(content.length());
    //client.println("Connection: keep-alive");
    //client.println("Upgrade-Insecure-Requests: 1");
    client.println();
    client.println(content);
    //client.println("Connection: close");
    //client.println();

  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if the server's disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();

    // do nothing forevermore
    while (true);
  }
}
