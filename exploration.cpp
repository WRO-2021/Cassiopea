#include "exploration.h"

/**
 * READ ME
 * 
 * for anyone reading this code, we know we can optimize it
 * We just didn't have the time during the past months...
 * We'll probably rewrite the whole code, with the same logic. 
 * 
 * please forgive us for the mess :)
 */


/*
 *
 * Direzioni:
 * 0 : nord -davanti
 * 1 : est - destra
 * 2 : sud - dietro
 * 3 : ovest - sinistra
 *
 *
contenuto campo:

0 : not mapped
s : start - partenza
b : black - nero
c : checkpoint
w : wall - muro
e : empty - vuota
? : unexplored - inesplorato
v : victim - vittima
p : priority 


destra - right
sinistra - left 
avanti - straight
dietro - back
*/


static Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X); //color sensor
static Adafruit_MLX90614 mlx = Adafruit_MLX90614(); //temperature sensor

static int soglia_calore;

static int conta_kit;

static int checkx, checky;

static char campo[100][100]; // campo means maze, that's the map
static int dim_campo;      // dimensione dell'array campo
static int posx, posy;     // posizione x, posizione y
// 0,0 basso sinistra
static int dir;//direzione in cui guarda il robot            // direzione 0 est, 1 nord, 2 ovest, 3 sud notazine di stafano, è cambiata


int absolute_dir_to_relative(int abs) {
    return (abs - dir + 4) % 4;
}

int relative_dir_to_absolute(int rel) {
    return (rel + dir) % 4;
}

int ix(int direction) {
    switch (direction) {
        case 0:
            return 0;
        case 1:
            return 1;
        case 2:
            return 0;
        case 3:
            return -1;
    }
}

int iy(int direction) {
    switch (direction) {
        case 0:
            return -1;
        case 1:
            return 0;
        case 2:
            return 1;
        case 3:
            return 0;
    }
}

void campo_init()// initialization of maze values 
{

    conta_kit = 0;


    dim_campo = sizeof(campo) / sizeof(campo[0]);   // dimensione del campo
    for (int i = 0; i < dim_campo; i++) {
        for (int j = 0; j < dim_campo; j++) {
            campo[i][j] = 0;   // inizializza il campo a 0 (sconosciuto)
        }
    }

    campo[dim_campo / 2][dim_campo / 2] = 's';    // inizializza la cella centrale a start

    posx = dim_campo / 2;          // inizializza la posizione su start
    posy = dim_campo / 2;
    dir = 1;                   // e direzione a nord

    checkx = posx; // ultimo checkpoint è la partenza (all'inizio)
    checky = posy;

    soglia_calore = mlx.readObjectTempC() + 1.5;
}


bool muro_abs(int abs) {
    return muro_rel(absolute_dir_to_relative(abs));
}

bool muro_rel(int rel) {
    Serial.print(tof_read(1));
    int t0, t1;
    switch (rel) {
        case 1:
            t0 = tof_read(1);
            t1 = tof_read(2);
            break;
        case 0:
            t0 = tof_read(7);
            t1 = 255;
            break;
        case 3:
            t0 = tof_read(5);
            t1 = tof_read(6);
            break;
        case 2:
            // ocio alle salite/discese
            if (getOldAngleX() < -15)
                return false;
            t0 = tof_read(3);
            t1 = tof_read(4);
            break;
    }
    return t0 < 255 || t1 < 255;
}


void scan_neighbors() // legge i sensori e modifica la mappa in base a questi
{



    // piastrella attuale nuova, controllo sotto
    if (campo[posx][posy] == '?') // se il tile/piastrella non era ancora stata esplorata
    {
        int val = analogRead(REFLEX);
        if (val > WHITE_SILVER_THESHOLD) {
            campo[posx][posy] = 'c'; // viene segnata come checkpoint
            checkx = posx; // ultimo checkpoint è questa casella
            checky = posy;
        } else {
            campo[posx][posy] = 'e'; // viene segnata come vuota
        }

    }

    // scannerizza i quattro muri
    // imposta i vicini a muri / sconosciuti / non modifica in base alle conoscenze

    for (int d = 0; d < 4; d++) {
        if (muro_rel(d))         // se c'è muro a nord
        {
            //campo[posx-1][posy+1]='w';
            campo[posx + ix(d)][posy + yi(d)] = 'w'; // imposta la cella a nord a muro
            //campo[posx+1][posy+1]='w';
        } else                    // altrimenti
        {
            if (campo[posx + ix(d)][posy + iy(d)] != 'p')    // se non è priorità (va modificata solo al passaggio)
                campo[posx + ix(d)][posy + iy(d)] = 'e';    // impostata a vuota (no muro)

            if (campo[posx + ix(d) * 2][posy + iy(d) * 2] == 0)      // cella successiva // se è sconosciuta
                campo[posx + ix(d) * 2][posy + iy(d) *
                                               2] = '?';    // impostata a inesplorata (non c'è muro quindi ci si può andare MA non è esplorata)
        }
    }


    if (campo[posx][posy] == 'c') {
        digitalWrite(LED_BUILTIN, HIGH);
    } else {
        digitalWrite(LED_BUILTIN, LOW);
    }
}

void found_victim(int kits)//o 1 kit o niente
{
    // lampeggia
    for (int i = 0; i < 4; i++) {
        digitalWrite(3, HIGH);
        delay(1000);
        digitalWrite(3, LOW);
        delay(1000);
    }

    if (kits) {

        //memorizza la direzione in cui si è girato
        //1 destra, 2 sinistra, 3 180 gradi
        int turn = 0;
        switch (dir) {
            case 0:
                if (muro_nord()) {
                    gira_destra();
                    turn = 1;
                } else if (muro_est()) {
                    gira_180();
                    turn = 3;
                } else if (muro_sud()) {
                    gira_sinistra();
                    turn = 2;
                }
                break;
            case 1:
                if (muro_nord()) {
                    gira_180();
                    turn
                    3;
                } else if (muro_est()) {
                    gira_sinistra();
                    turn = 2;
                } else if (muro_ovest()) {
                    gira_destra();
                    turn
                    1;
                }
                break;
            case 2:
                if (muro_nord()) {
                    gira_sinistra();
                    turn = 2;
                } else if (muro_ovest()) {
                    gira_180();
                    turn = 3;
                } else if (muro_sud()) {
                    gira_destra();
                    turn = 1;
                }
                break;
            case 3:
                if (muro_sud()) {
                    gira_180();
                    turn = 3;
                } else if (muro_ovest()) {
                    gira_sinistra();
                    turn = 2;
                } else if (muro_est()) {
                    gira_destra();
                    turn = 1;
                }
                break;
        }

        //smolla il/i kit
        drop_kits(1);

        switch (turn) {
            case 1:
                gira_sinistra();
                break;
            case 2:
                gira_destra();
                break;
            case 3:
                gira_180();
                break;
        }
        conta_kit++;//ho usato un kit
    }
}

void check_for_victims() {
    int t0 = tof_read(1);
    int t1 = tof_read(2);
    if (t0 < 150 && t1 < 150 && campo[posx][posy] != 'v' && campo[posx][posy] != 's') {
        if (mlx.readObjectTempC() > soglia_calore) //controllare dopo la prima gara
        {
            //VITTIMAAA DI CALORE
            motor_break();//non dovrebbe servire
            campo[posx][posy] = 'v';

            digitalWrite(LED_B, LOW);

            found_victim(1);
            digitalWrite(LED_B, HIGH);
        } else {
            MUX(0);

            uint16_t red, green, blue, clear;

            tcs.setInterrupt(false);  // turn on LED //(sempre disabilitato btw)

            delay(60);  // takes 50ms to read

            tcs.getRawData(&red, &green, &blue, &clear);

            tcs.setInterrupt(true);  // turn off LED (sempre disabilitato btw)

            if (int(clear) <= 110) {
                //VITTIMA COLORATA
                motor_break();//non dovrebbe servire
                campo[posx][posy] = 'v';

                if (red > green) {
                    digitalWrite(LED_R, LOW);
                    //ROSSO
                    found_victim(1);

                    digitalWrite(LED_R, HIGH);
                } else {

                    digitalWrite(LED_G, LOW);
                    //VERDE
                    found_victim(0);

                    digitalWrite(LED_G, HIGH);
                }

                //l'importante è che mantenga i 5 secondi
            }
        }
    }
}

void gira_destra() {
    turn_degrees(90);
    allinea_muro();
    distanzia_muro();

    dir += 3;
    dir %= 4;

    check_for_victims();
}

void gira_sinistra() {
    turn_degrees(-90);
    allinea_muro();
    distanzia_muro();

    dir += 1;
    dir %= 4;

    check_for_victims();
}

void gira_180() {
    gira_destra();
    gira_destra();
}

void gira(int direction){
    switch (direction) {
        case 0:
            gira_destra();
            break;
        case 1:
            gira_sinistra();
            break;
        case 2:
            gira_180();
            break;
    }
}


void avanti() {
    //allinea_muro_bene();
    bool done = move_cm_avoid_black(32);///////////////////////// cm da tyle a tyle
    allinea_muro();
    distanzia_muro();

    if (done) {
        posx += ix(dir);
        posy += iy(dir);
    } else {
        // setta la piastrella nera
        campo[posx + ix(*2dir)][posy + iy(dir)*2] = 'b';
    }

    check_for_victims();
}


bool priority_path_to(char dest) // trova la casella contenente dest più vicina e imposta il percorso per raggiungerlo a p (priority) non va con b (black)
{
    int dim = 100;
    if (dim <= 0)
        return false;

    int x[dim];
    int y[dim];
    int prec[dim];
    int i = 0;
    int lung = 1

    x[0] = posx;
    y[0] = posy;
    prec[0] = 0;

    while (i < lung && lung + 3 < dim && campo[x[i]][y[i]] != dest) {

        //printf("> %d, %d\n",x[i],y[i]);
        //printf("  i:     %d\n",i);
        //printf("  lung:  %d\n",lung);

        if (campo[x[i] + 1][y[i]] != 'w' && campo[x[i] + 2][y[i]] != 0 &&
            campo[x[i] + 2][y[i]] != 'b')// se non è un muro ed è già stato esplorata la direzione e non è muro
        {
            bool alreadyThere = false;
            int j = 0;
            while (j < lung && !alreadyThere) {
                if (x[j] == x[i] + 2 && y[j] == y[i])
                    alreadyThere = true;
                j++;
            }
            if (!alreadyThere) {
                x[lung] = x[i] + 2;
                y[lung] = y[i];
                prec[lung] = i;
                lung++;

            }
        }
        //printf(".\n");
        if (campo[x[i]][y[i] + 1] != 'w' && campo[x[i]][y[i] + 2] != 0 && campo[x[i]][y[i] + 2] != 'b') {
            bool alreadyThere = false;
            int j = 0;
            while (j < lung && !alreadyThere) {
                if (x[j] == x[i] && y[j] == y[i] + 2)
                    alreadyThere = true;
                j++;
            }
            if (!alreadyThere) {
                x[lung] = x[i];
                y[lung] = y[i] + 2;
                prec[lung] = i;
                lung++;
            }
        }


        if (campo[x[i] - 1][y[i]] != 'w' && campo[x[i] - 2][y[i]] != 0 && campo[x[i] - 2][y[i]] != 'b') {
            bool alreadyThere = false;
            int j = 0;
            while (j < lung && !alreadyThere) {
                if (x[j] == x[i] - 2 && y[j] == y[i])
                    alreadyThere = true;
                j++;
            }
            if (!alreadyThere) {
                x[lung] = x[i] - 2;
                y[lung] = y[i];
                prec[lung] = i;
                lung++;
            }
        }


        if (campo[x[i]][y[i] - 1] != 'w' && campo[x[i]][y[i] - 2] != 0 && campo[x[i]][y[i] - 2] != 'b') {
            bool alreadyThere = false;
            int j = 0;
            while (j < lung && !alreadyThere) {
                if (x[j] == x[i] && y[j] == y[i] - 2)
                    alreadyThere = true;
                j++;
            }
            if (!alreadyThere) {
                x[lung] = x[i];
                y[lung] = y[i] - 2;
                prec[lung] = i;
                lung++;
            }
        }
        //printf(".\n");

        i++;

    }


    if (i >= lung || campo[x[i]][y[i]] != dest)
        return false;

    while (i != 0) {
        //printf("%d, %d\n",x[i],y[i]);

        campo[(x[i] + x[prec[i]]) / 2][(y[i] + y[prec[i]]) / 2] = 'p';


        i = prec[i];
    }
    return true;
}


bool esplora() {
    bool finished = false;

    while (!finished) {

        // modalità esplorazione
        // ci si basa su ciò che si ha intorno per procedere (celle conosiute o sconosiute)
        // se tutte conosciute si entra in modalità ricerca percorso per la sconosciuta più vicina o, se non ce ne sono, per start

        //exploration mode
        scan_neighbors();
        int dir_scan = 0;
        bool trovato = false;

        if (campo[posx + 1][posy] == 'p' ||
            campo[posx][posy + 1] == 'p' ||
            campo[posx - 1][posy] == 'p' ||
            campo[posx][posy - 1] == 'p') {
            trovato = true;
            if (campo[posx + 1][posy] == 'p') {
                dir_scan = (8 - dir + 1 + 0) % 4;
                campo[posx + 1][posy] = 'e';
            } else if (campo[posx][posy + 1] == 'p') {
                dir_scan = (8 - dir + 1 + 1) % 4;
                campo[posx][posy + 1] = 'e';
            } else if (campo[posx - 1][posy] == 'p') {
                dir_scan = (8 - dir + 1 + 2) % 4;
                campo[posx - 1][posy] = 'e';
            } else if (campo[posx][posy - 1] == 'p') {
                dir_scan = (8 - dir + 1 + 3) % 4;
                campo[posx][posy - 1] = 'e';
            }
        } else {

            while (dir_scan < 4 && !trovato) {
                switch ((dir + (6 - dir_scan) + 3) % 4)//direzione robot + direzione scan
                {
                    case 0:
                        if (campo[posx + 1][posy] != 'w' && campo[posx + 2][posy] ==
                                                            '?') // priority O se non c'è muro in questa direzione  E  se la casella da quella è sconosciuta
                        {
                            trovato = true;
                        } else {
                            dir_scan++;
                        }
                        break;

                    case 1:
                        if (campo[posx][posy + 1] != 'w' && campo[posx][posy + 2] ==
                                                            '?') // se non c'è muro in questa direzione  E  se la casella da quella è sconosciuta
                        {
                            trovato = true;
                        } else {
                            dir_scan++;
                        }
                        break;

                    case 2:
                        if (campo[posx - 1][posy] != 'w' && campo[posx - 2][posy] ==
                                                            '?') // se non c'è muro in questa direzione  E  se la casella da quella è sconosciuta
                        {
                            trovato = true;
                        } else {
                            dir_scan++;
                        }
                        break;

                    case 3:
                        if (campo[posx][posy - 1] != 'w' && campo[posx][posy - 2] ==
                                                            '?') // se non c'è muro in questa direzione  E  se la casella da quella è sconosciuta
                        {
                            trovato = true;
                        } else {
                            dir_scan++;
                        }
                        break;
                }
            }
            dir_scan = (6 - dir_scan) % 4;
        }

        if (!trovato) {
            //ricerca percorso
            if (!priority_path_to('?')) {
                //Nessun percorso per ? trovato
                if (campo[posx][posy] == 's')
                    return true;

                if (!priority_path_to('s'))
                    return false;//Nessun percorso per s trovato
            }
        } else {

            switch (dir_scan % 4) {
                case 0:
                    gira_destra();
                    break;
                case 1:
                    //spostare avanti() qui (togliendolo sotto) dovrebbe far scannerizzare anche dopo aver girato
                    break;
                case 2:
                    gira_sinistra();
                    break;
                case 3:
                    gira_180();
                    break;
            }
            avanti();
        }
    }
    return true;
}


//prints on serial a "graphic" visualization of the mapped maze
void campo_stampa() {
    int i1 = 0;
    int i2 = dim_campo - 1;
    int j1 = 0;
    int j2 = dim_campo - 1;
    int i, j;
    i = 0;
    bool c = true;//clear
    while (c) {
        for (j = 0; j < dim_campo; j++) {
            if (campo[i1][j])
                c = false;
        }
        if (c)
            i1++;
    }
    c = true;
    while (c) {
        for (j = 0; j < dim_campo; j++) {
            if (campo[i2][j])
                c = false;
        }
        if (c)
            i2--;
    }

    c = true;//clear
    while (c) {
        for (i = 0; i < dim_campo; i++) {
            if (campo[i][j1])
                c = false;
        }
        if (c)
            j1++;
    }
    c = true;
    while (c) {
        for (i = 0; i < dim_campo; i++) {
            if (campo[i][j2])
                c = false;
        }
        if (c)
            j2--;
    }

    for (i = i1; i <= i2; i++) {
        for (j = j1; j <= j2; j++) {
            //Serial.print((campo[i][j])?campo[i][j]:' ');   // inizializza il campo a 0 (sconosciuto)
            switch (campo[i][j]) {
                case 0:
                case 'e':
                    Serial.print("  ");
                    break;
                case 'w':
                    Serial.print("W ");
                    break;
                default:
                    Serial.print(campo[i][j]);
                    Serial.print(' ');
                    break;
            }
        }
        Serial.println();
    }
    Serial.println();
    Serial.println();
    Serial.println();
}
