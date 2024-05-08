#include <Ethernet.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server_addr(104,155,173,175); // IP-ul Public din cloud
char user[] = "nume"; // user-ul db-ului
char password[] = "nume"; // parola db-ului
// Sample query
//
// Notice the "%lu" - that's a placeholder for the parameter we will
// supply. See sprintf() documentation for more formatting specifier
// options
const char QUERY_POP[] = "SELECT value FROM homeautomation.test";
char query[128];
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
    delay(1000);
    Serial.println("> Running SELECT with dynamically supplied parameter");
    // Initiate the query class instance
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    // Supply the parameter for the query
    // Here we use the QUERY_POP as the format string and query as the
    // destination. This uses twice the memory so another option would be
    // to allocate one buffer for all formatted queries or allocate the
    // memory as needed (just make sure you allocate enough memory and
    // free it when you're done!).
    sprintf(query, QUERY_POP, 9000000);
    // Execute the query
    cur_mem->execute(query);
    // Fetch the columns and print them
    column_names *cols = cur_mem->get_columns();
    for (int f = 0; f < cols->num_fields; f++) {
        Serial.print(cols->fields[f]->name);
        if (f < cols->num_fields-1) {
            Serial.print(',');
        }
    }
    Serial.println();
    // Read the rows and print them
    row_values *row = NULL;
    do {
        row = cur_mem->get_next_row();
        if (row != NULL) {
            for (int f = 0; f < cols->num_fields; f++) {
                Serial.print(row->values[f]);
                if (f < cols->num_fields-1) {
                    Serial.print(',');
                }
            }
        Serial.println();
        }
    } while (row != NULL);
    // Deleting the cursor also frees up memory used
    delete cur_mem;
}