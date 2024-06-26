#include <Ethernet.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress server_addr(104,155,173,175); 
char user[] = "nume"; 
char password[] = "nume"; 

EthernetClient client;
MySQL_Connection conn((Client *)&client);

DFRobot_DHT11 DHT;

//PINI leduri
#define LED_BUCATARIE 1
#define LED_SUFRAGERIE 2
#define LED_DORMITOR 3
#define LED_BAIE 4
#define LED_USA 6
#define LED_CENTRALA 9
#define DHT11_PIN 10

float temperatura_ambientala=29.0;

//
//INTEROGE
//
const char QUERY_POP[] = "SELECT value FROM hihome.test";
char query[128];

//
//ADAUGARE
//
char INSERT_SQL[] = "INSERT INTO hihome.test (nume, prenume) VALUES ('Stelian','Balet')";

void setup() {
    Serial.begin(9600);
    while (!Serial); // wait for serial port to connect
    Ethernet.begin(mac_addr);
    Serial.println("Connecting...");
    if (conn.connect(server_addr, 3306, user, password))
        delay(1000);
    else
        Serial.println("Connection failed.");
        
    pinMode(LED_BUCATARIE, OUTPUT);
    pinMode(LED_SUFRAGERIE, OUTPUT);
    pinMode(LED_DORMITOR, OUTPUT);
    pinMode(LED_BAIE, OUTPUT);
    pinMode(LED_USA, OUTPUT);
    pinMode(LED_CENTRALA, OUTPUT);
}

//setat intensitate lumina camera
void lumina_camera(int camera, int intensitate){
    analogWrite(camera, (int)((intensitate/100)*(255-128)+127));
}

//deschidere/inchidere usa
void usa(bool stare_incuietoare){
    if (stare_incuietoare)
        digitalWrite(LED_USA, HIGH);
    else
        digitalWrite(LED_USA, LOW);
}

void setare_temperatura(float temperatura_setata){
    temperatura_ambientala=temperatura_setata;
}

void termostat(){
    if(temperatura_ambientala>DHT.read(DHT11_PIN))
        digitalWrite(LED_CENTRALA, HIGH);
    else
        digitalWrite(LED_CENTRALA, LOW);
}

void loop() {
    delay(1000);
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);

    //
    //INTEROGE
    //

    //USA
    sprintf(query, "SELECT Setare_stare_usa FROM hihome.Control_Usa", 9000000);
    cur_mem->execute(query);
    usa(cur_mem->getBoolean);

    //LUMINI
    sprintf(query, "SELECT Setare_intensitate  FROM hihome.Comenzi_Lumină ", 9000000);
    cur_mem->execute(query);
    row_values *row = NULL;
    int i=1;
    do {
        row = cur_mem->get_next_row();
        if (row != NULL) {
            lumina_camera(i++,row->values[0]);
        }
    } while (row != NULL);

    //CENTRALA
    sprintf(query, "SELECT Setare_temperatura FROM hihome.Setări_Temperatură", 9000000);
    cur_mem->execute(query);
    setare_temperatura(cur_mem->getFloat);
    termostat();


    //
    //ADAUGARE
    //

    //USA
    cur_mem->execute("INSERT INTO hihome.Control_Usa (Citire_stare_usa) VALUES (%s)",digitalRead(LED_USA == HIGH));

    //LUMINI 
    for(int camera=1; camera<=4; camera++){
        cur_mem->execute("INSERT INTO hihome.Comenzi_Lumină (Citire_stare_lumini) VALUES (%s)",digitalRead(camera == HIGH)); 
    }
 
    //CENTRALA 
    cur_mem->execute("INSERT INTO hihome.Setări_Temperatură (Citire_temperatura) VALUES (%.2f)", DHT.read(DHT11_PIN));

    delete cur_mem;
    delay(4000);
}
