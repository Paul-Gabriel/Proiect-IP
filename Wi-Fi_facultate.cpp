#include <ESP8266WiFi.h>
#include <DFRobot_DHT11.h>

#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

#ifndef STASSID
#define STASSID "AIA"
#define STAPSK "UPT-aia!"
#endif
DFRobot_DHT11 DHT;
#define DHT11_PIN 3

//trimiterea in baza de date in loop
const char* ssid = STASSID;
const char* wifiPassword = STAPSK;
IPAddress server_addr(104,155,173,175); // IP of the MySQL server here
char user[] = "nume"; // MySQL user login username
char password[] = "nume"; // MySQL user login password
// Use WiFiClient class to create TCP connections
WiFiClient client;
MySQL_Connection conn((Client *)&client);
// char INSERT_SQL[] = "INSERT INTO hihome.test (nume,prenume) VALUES ('Stelian','22.33')";

void sendData();

void setup() {
    Serial.begin(9600);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
    would try to act as both a client and an access-point and could cause
    network-issues with your other WiFi-devices on your WiFi-network. */
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, wifiPassword);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    //MySql section
    Serial.println("Connecting...");    
}

char INSERT_SQL[] = "INSERT INTO hihome.Setări_Temperatură (Citire_temperatura) VALUES (";

void sendData(float temperatura){
    Serial.println("Recording data.");
    // Initiate the query class instance
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);

    //Instructiunea de punere in BD a temperaturii
    char query[100];
    sprintf(query, "%s%.2f)", INSERT_SQL, temperatura);

    // Execute the query
    //adauga in baza de date
    cur_mem->execute(query);
    // Note: since there are no results, we do not need to read any data
    // Deleting the cursor also frees up memory used
    delete cur_mem;
}

void loop(){
    float temperatura = DHT.read(DHT11_PIN);
    // Serial.print("temp:");
    // Serial.print(DHT.temperature);
    // Serial.print("  humi:");
    // Serial.println(DHT.humidity);
    delay(1000);
    if (conn.connect(server_addr, 3306, user, password)) {
        sendData(temperatura);
        delay(1000);
        conn.close();
    }
    else
    {
        Serial.println("Connection failed.");
    }
}