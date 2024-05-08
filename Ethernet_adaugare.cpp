#include <Ethernet.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server_addr(104,155,173,175); // Adresa Ip public Cloud
char user[] = "nume"; // MySQL user login username
char password[] = "nume"; // MySQL user login password
// Sample query
char INSERT_SQL[] = "INSERT INTO homeautomation.test (nume, prenume) VALUES ('Stelian','Balet')";
EthernetClient client;
MySQL_Connection conn((Client *)&client);

void setup() {
    Serial.begin(9600);
    while (!Serial); // wait for serial port to connect
    Ethernet.begin(mac_addr);
    Serial.println("Connecting...");
    if (conn.connect(server_addr, 3306, user, password)) 
        delay(1000);
    else
        Serial.println("Connection failed.");
}

void loop() {
    delay(2000);
    Serial.println("Recording data.");
    // Initiate the query class instance
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    // Execute the query
    cur_mem->execute(INSERT_SQL);
    // Note: since there are no results, we do not need to read any data
    // Deleting the cursor also frees up memory used
    delete cur_mem;
}