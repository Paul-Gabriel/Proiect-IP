#include <Ethernet.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server_addr(104,155,173,175); 
char user[] = "nume"; 
char password[] = "nume"; 

EthernetClient client;
MySQL_Connection conn((Client *)&client);

//
//INTEROGE
//
const char QUERY_POP[] = "SELECT value FROM homeautomation.test";
char query[128];

//
//ADAUGARE
//
char INSERT_SQL[] = "INSERT INTO homeautomation.test (nume, prenume) VALUES ('Stelian','Balet')";

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
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);

    //
    //INTEROGE
    //
    Serial.println("Citeste din baza de date");
    sprintf(query, QUERY_POP, 9000000);
    //ia din baza de date
    cur_mem->execute(query);
    //ia capul de tabel
    column_names *cols = cur_mem->get_columns();
    for (int f = 0; f < cols->num_fields; f++) {
        Serial.print(cols->fields[f]->name);
        if (f < cols->num_fields-1) {
            Serial.print(',');
        }
    }
    Serial.println();
    //citeste randurile si le afisaza
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
    
    //Cred ca aici o sa interpretam ce am luat din baza de date 
    //si dupa scriem inapoi in baza de date

    //
    //ADAUGARE
    //
    Serial.println("Adaugare in baza de date.");
    //adauga in baza de date
    cur_mem->execute(INSERT_SQL);
    
    delete cur_mem;
    delay(4000);
}
