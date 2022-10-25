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

#define VA_A_DESTRA false //true for follow the right wall, false for the left wall


static Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X); //color sensor
static Adafruit_MLX90614 mlx = Adafruit_MLX90614(); //temperature sensor

static int soglia_calore;

static Servo servo_kit;
static int conta_kit;

static int checkx, checky;

static char campo[100][100]; // campo means maze, that's the map
static int dim_campo;      // dimensione dell'array campo
static int posx, posy;     // posizione x, posizione y
                    // 0,0 basso sinistra
static int dir;            // direzione 0 est, 1 nord, 2 ovest, 3 sud

//prints on serial a "graphic" visualization of the mapped maze
void campo_stampa()
{
  int i1=0;
  int i2=dim_campo-1;
  int j1=0;
  int j2=dim_campo-1;
  int i,j;
  i=0;
  bool c=true;//clear
  while(c)
  {
    for(j=0;j<dim_campo;j++)
    {
      if(campo[i1][j])
        c=false;
    }
    if(c)
      i1++;
  }
  c=true;
  while(c)
  {
    for(j=0;j<dim_campo;j++)
    {
      if(campo[i2][j])
        c=false;
    }
    if(c)
      i2--;
  }
  
  c=true;//clear
  while(c)
  {
    for(i=0;i<dim_campo;i++)
    {
      if(campo[i][j1])
        c=false;
    }
    if(c)
      j1++;
  }
  c=true;
  while(c)
  {
    for(i=0;i<dim_campo;i++)
    {
      if(campo[i][j2])
        c=false;
    }
    if(c)
      j2--;
  }
  
    for(i=i1;i<=i2;i++)
    {
        for(j=j1;j<=j2;j++)
        {
            //Serial.print((campo[i][j])?campo[i][j]:' ');   // inizializza il campo a 0 (sconosciuto)
            switch(campo[i][j])
            {
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


void campo_init()// initialization of maze values 
{
  tcs.begin();//color
  mlx.begin();//temperature
  servo_kit.attach(5);
  servo_kit.write(95);
  conta_kit=0;
  
  
    dim_campo=sizeof(campo)/sizeof(campo[0]);   // dimensione del campo
    for(int i=0;i<dim_campo;i++)
    {
        for(int j=0;j<dim_campo;j++)
        {
            campo[i][j]=0;   // inizializza il campo a 0 (sconosciuto)
        }
    }

    campo[dim_campo/2][dim_campo/2]='s';    // inizializza la cella centrale a start

    posx=dim_campo/2;          // inizializza la posizione su start
    posy=dim_campo/2;
    dir=1;                   // e direzione a nord

    checkx = posx; // ultimo checkpoint è la partenza (all'inizio)
    checky = posy;

    soglia_calore = mlx.readObjectTempC()+1.5;
}



bool muro_nord()    // ritorna se c'è un muro a NORD
{                   // ATTENZIONE a NORD nella mappa NON il sensore avanti, il sensore dipende dalla direzione (dir)
  Serial.print(tof_read(1));
  int t0, t1;  
  switch(dir)
  {
  case 0:
    t0 = tof_read(1);
    t1 = tof_read(2);
    break;
  case 1:
    t0 = tof_read(7);
    t1 = 255;
    break;
  case 2:
    t0 = tof_read(5);
    t1 = tof_read(6);
    break;
  case 3:
    if(getOldAngleX()<-15)
      return false;
    t0 = tof_read(3);
    t1 = tof_read(4);
    break;
  }
  printf("")
  return t0<255 || t1<255;
}

bool muro_est()
{
  int t0, t1;  
  switch(dir)
  {
  case 3:
    t0 = tof_read(1);
    t1 = tof_read(2);
    break;
  case 0:
    t0 = tof_read(7);
    t1 = 255;
    break;
  case 1:
    t0 = tof_read(5);
    t1 = tof_read(6);
    break;
  case 2:
    if(getOldAngleX()<-15)
      return false;
    t0 = tof_read(3);
    t1 = tof_read(4);
    break;
  }
  return t0<255 || t1<255;
}

bool muro_sud()
{
  int t0, t1;  
  switch(dir)
  {
  case 2:
    t0 = tof_read(1);
    t1 = tof_read(2);
    break;
  case 3:
    t0 = tof_read(7);
    t1 = 255;
    break;
  case 0:
    t0 = tof_read(5);
    t1 = tof_read(6);
    break;
  case 1:
    if(getOldAngleX()<-15)
      return false;
    t0 = tof_read(3);
    t1 = tof_read(4);
    break;
  }
  return t0<255 || t1<255;
}

bool muro_ovest()
{
  int t0, t1;  
  switch(dir)
  {
  case 1:
    t0 = tof_read(1);
    t1 = tof_read(2);
    break;
  case 2:
    t0 = tof_read(7);
    t1 = 255;
    break;
  case 3:
    t0 = tof_read(5);
    t1 = tof_read(6);
    break;
  case 0:
    if(getOldAngleX()<-15)
      return false;
    t0 = tof_read(3);
    t1 = tof_read(4);
    break;
  }
  return t0<255 || t1<255;
}

void scan_neighbors() // legge i sensori e modifica la mappa in base a questi
{

  if(digitalRead(12) == HIGH) //se il pulsante è premuto //CODICE FELIPRO44
  {
    motor_break();
    digitalWrite(2, HIGH);
    delay(2000); //pausa di sicurezza (no doppi click ecc...)
    
    while(digitalRead(12) == LOW) //si aspetta che sia premuto di nuovo//CODICE FELIPRO44
    {
      /*Serial.print("pos:\t");
      Serial.print(posx);
      Serial.print("\t");
      Serial.println(posy);
      Serial.print("check:\t");
      Serial.print(checkx);
      Serial.print("\t");
      Serial.println(checky);
      Serial.println();*/
    }
    digitalWrite(2, LOW);
    
    delay(1800); //pausa di sicurezza (non parte mentre si sta ancora toccando)
    
    digitalWrite(2, HIGH);
    delay(200);
    digitalWrite(2, LOW);
    
    posx=checkx;
    posy=checky;
    dir=1;

  }

    if(campo[posx][posy]=='?') // se il tile/piastrella non era ancora stata esplorata
    {
      int val = analogRead(REFLEX);
      if(val > WHITE_SILVER_THESHOLD)
      {
        campo[posx][posy]='c'; // viene segnata come checkpoint
        checkx = posx; // ultimo checkpoint è questa casella
        checky = posy;
      }
      else
      {
        campo[posx][posy]='e'; // viene segnata come vuota
      }
      
    }

    campo[posx+1][posy+1]='w';      // muri negli angoli
    campo[posx-1][posy+1]='w';      // sbagliato, se si forma una stanza salva un muro/colonna in mezzo
    campo[posx+1][posy-1]='w';
    campo[posx-1][posy-1]='w';

    // scannerizza i quattro muri
    // imposta i vicini a muri / sconosciuti / non modifica in base alle conoscenze

    if(muro_nord())         // se c'è muro a nord
    {
        //campo[posx-1][posy+1]='w';
        campo[posx][posy+1]='w'; // imposta la cella a nord a muro
        //campo[posx+1][posy+1]='w';
    }
    else                    // altrimenti
    {
        if(campo[posx][posy+1]!='p')    // se non è priorità (va modificata solo al passaggio)
            campo[posx][posy+1]='e';    // impostata a vuota (no muro)

        if(campo[posx][posy+2]==0)      // cella successiva // se è sconosciuta
            campo[posx][posy+2]='?';    // impostata a inesplorata (non c'è muro quindi ci si può andare MA non è esplorata)
    }


    if(muro_est()) // vedi sopra
    {
        //campo[posx+1][posy-1]='w';
        campo[posx+1][posy]='w';
        //campo[posx+1][posy+1]='w';
    }
    else
    {
        if(campo[posx+1][posy]!='p')
            campo[posx+1][posy]='e';

        if(campo[posx+2][posy]==0)
            campo[posx+2][posy]='?';
    }


    if(muro_sud()) // vedi sopra
    {
        //campo[posx-1][posy-1]='w';
        campo[posx][posy-1]='w';
        //campo[posx+1][posy-1]='w';
    }
    else
    {
        if(campo[posx][posy-1]!='p')
            campo[posx][posy-1]='e';

        if(campo[posx][posy-2]==0)
            campo[posx][posy-2]='?';
    }


    if(muro_ovest()) // vedi sopra
    {
        //campo[posx-1][posy-1]='w';
        campo[posx-1][posy]='w';
        //campo[posx-1][posy+1]='w';
    }
    else
    {
        if(campo[posx-1][posy]!='p')
            campo[posx-1][posy]='e';

        if(campo[posx-2][posy]==0)
            campo[posx-2][posy]='?';
    }

  if(campo[posx][posy]=='c')
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void found_victim(int kits)//o 1 kit o niente
{
  if(kits)
  {
      //TODO girati di 90, smolla e rigirati

      //lampeggia il led
      digitalWrite(3, HIGH);
      delay(1000);
      digitalWrite(3, LOW);
      delay(1000);
      digitalWrite(3, HIGH);
      delay(1000);
      digitalWrite(3, LOW);

      //TODO capire da che lato è la vittima

      //si gira di 90
      gira_destra();

      //smolla il/i kit
      if(conta_kit%2 == 0)
      {
        servo_kit.write(145);
      }
      else
      {
        servo_kit.write(45);
      }
      delay(500);
      if(conta_kit%2 == 0)
      {
        servo_kit.write(85);
      }
      else
      {
        servo_kit.write(105);
      }
      delay(500);
      servo_kit.write(95);//riallinea il servo

      //si gira di 90
      gira_sinistra();
      
      digitalWrite(3, HIGH);//altro lampeggio

      delay(2000);
      digitalWrite(3, LOW);
      conta_kit++;//ho usato un kit
  }
  else
  {
    //lampeggia di più
    digitalWrite(3, HIGH);
    delay(1000);
    digitalWrite(3, LOW);
    delay(1000);
    digitalWrite(3, HIGH);
    delay(1000);
    digitalWrite(3, LOW);
    delay(1000);
    digitalWrite(3, HIGH);
    delay(2000);
    digitalWrite(3, LOW);
  }
}

void check_for_victims()///////////////////////////////////////////////////////////////////////////////////////////////////
{
  int t0 = tof_read(1);
  int t1 = tof_read(2);
  if(t0<150 && t1<150 && campo[posx][posy]!='v' && campo[posx][posy]!='s')
  {
    if(mlx.readObjectTempC()> soglia_calore) //controllare dopo la prima gara
    {
      //VITTIMAAA DI CALORE
      motor_break();//non dovrebbe servire
      campo[posx][posy]='v';
      
      digitalWrite(LED_B, LOW);
      
      found_victim(1);
      digitalWrite(LED_B, HIGH);
    }
    else
    {
      MUX(0);
      
      uint16_t red, green, blue, clear;
  
      tcs.setInterrupt(false);  // turn on LED //(sempre disabilitato btw)
    
      delay(60);  // takes 50ms to read
    
      tcs.getRawData(&red, &green, &blue, &clear);
      
      tcs.setInterrupt(true);  // turn off LED (sempre disabilitato btw)

      if(int(clear)<=110) 
      {
        //VITTIMA COLORATA
        motor_break();//non dovrebbe servire
        campo[posx][posy]='v';

        if(red>green)
        {
          digitalWrite(LED_R, LOW);
          //ROSSO
          found_victim(1);
          
          digitalWrite(LED_R, HIGH);
        }
        else
        {
          
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

void gira_destra()
{
    turn_degrees(90);
    allinea_muro();
    distanzia_muro();
    
    dir+=3;
    dir%=4;
    
    check_for_victims();
}

void gira_sinistra()
{
    turn_degrees(-90);
    allinea_muro();
    distanzia_muro();
    
    dir+=1;
    dir%=4;
    
    check_for_victims();
}

void gira_180()
{
    gira_destra();
    gira_destra();
}

void avanti()
{
    //allinea_muro_bene();
    bool done = move_cm_avoid_black(32);///////////////////////// cm da tyle a tyle 
    allinea_muro(); 
    distanzia_muro();

    if(done)
    {
      switch(dir) // modifica la posizione a seconda della direzione
      {
      case 0:
          posx+=2;
          break;
      case 1:
          posy+=2;
          break;
      case 2:
          posx-=2;
          break;
      case 3:
          posy-=2;
          break;
      }
    }
    else
    {
      switch(dir) // modifica la piastrella nera a seconda della direzione
      {
      case 0:
          campo[posx+2][posy]='b';
          break;
      case 1:
          campo[posx][posy+2]='b';
          break;
      case 2:
          campo[posx-2][posy]='b';
          break;
      case 3:
          campo[posx][posy-2]='b';
          break;
      }
    }
    
    check_for_victims();
    
}


bool priority_path_to(char dest) // trova la casella contenente dest più vicina e imposta il percorso per raggiungerlo a p (priority) non va con b (black)
{
    int dim=100;
    if(dim<=0)
        return false;

    int x[dim];
    int y[dim];
    int prec[dim];
    int i=0;
    int lung=1;

    x[0]=posx;
    y[0]=posy;
    prec[0]=0;

    while(i<lung && lung+3 < dim && campo[x[i]][y[i]] != dest)
    {

        //printf("> %d, %d\n",x[i],y[i]);
        //printf("  i:     %d\n",i);
        //printf("  lung:  %d\n",lung);

        if(campo[x[i]+1][y[i]]!='w' && campo[x[i]+2][y[i]]!=0 && campo[x[i]+2][y[i]]!='b')// se non è un muro ed è già stato esplorata la direzione e non è muro
        {
            bool alreadyThere=false;
            int j=0;
            while(j<lung && !alreadyThere)
            {
                if(x[j]==x[i]+2 && y[j]==y[i])
                    alreadyThere=true;
                j++;
            }
            if(!alreadyThere)
            {
                x[lung]=x[i]+2;
                y[lung]=y[i];
                prec[lung]=i;
                lung++;

            }
        }
        //printf(".\n");
        if(campo[x[i]][y[i]+1]!='w' && campo[x[i]][y[i]+2]!=0 && campo[x[i]][y[i]+2]!='b')
        {
            bool alreadyThere=false;
            int j=0;
            while(j<lung && !alreadyThere)
            {
                if(x[j]==x[i] && y[j]==y[i]+2)
                    alreadyThere=true;
                j++;
            }
            if(!alreadyThere)
            {
                x[lung]=x[i];
                y[lung]=y[i]+2;
                prec[lung]=i;
                lung++;
            }
        }

    
        if(campo[x[i]-1][y[i]]!='w' && campo[x[i]-2][y[i]]!=0 && campo[x[i]-2][y[i]]!='b')
        {
            bool alreadyThere=false;
            int j=0;
            while(j<lung && !alreadyThere)
            {
                if(x[j]==x[i]-2 && y[j]==y[i])
                    alreadyThere=true;
                j++;
            }
            if(!alreadyThere)
            {
                x[lung]=x[i]-2;
                y[lung]=y[i];
                prec[lung]=i;
                lung++;
            }
        }

       
        if(campo[x[i]][y[i]-1]!='w' && campo[x[i]][y[i]-2]!=0 && campo[x[i]][y[i]-2]!='b')
        {
            bool alreadyThere=false;
            int j=0;
            while(j<lung && !alreadyThere)
            {
                if(x[j]==x[i] && y[j]==y[i]-2)
                    alreadyThere=true;
                j++;
            }
            if(!alreadyThere)
            {
                x[lung]=x[i];
                y[lung]=y[i]-2;
                prec[lung]=i;
                lung++;
            }
        }
        //printf(".\n");

        i++;

    }


    if(i>=lung || campo[x[i]][y[i]] != dest)
        return false;

    while(i!=0)
    {
        //printf("%d, %d\n",x[i],y[i]);

        campo[(x[i]+x[prec[i]])/2][(y[i]+y[prec[i]])/2]='p';


        i=prec[i];
    }
    return true;
}

bool priority_path_to(int destx, int desty)
{
    return false; // da implementare, praticamente identico a priority_path_to(char dest)
}

bool esplora()
{
    bool finished=false;

    while(!finished)
    {

        // modalità esplorazione
        // ci si basa su ciò che si ha intorno per procedere (celle conosiute o sconosiute)
        // se tutte conosciute si entra in modalità ricerca percorso per la sconosciuta più vicina o, se non ce ne sono, per start

        //exploration mode
        scan_neighbors();
        int dir_scan=0;
        bool trovato=false;

        if(campo[posx+1][posy]=='p'||
           campo[posx][posy+1]=='p'||
           campo[posx-1][posy]=='p'||
           campo[posx][posy-1]=='p')
        {
            trovato=true;
            if(campo[posx+1][posy]=='p')
            {
                dir_scan=(8-dir+1+0)%4;
                campo[posx+1][posy]='e';
            }

            else if(campo[posx][posy+1]=='p')
            {
                dir_scan=(8-dir+1+1)%4;
                campo[posx][posy+1]='e';
            }

            else if(campo[posx-1][posy]=='p')
            {
                dir_scan=(8-dir+1+2)%4;
                campo[posx-1][posy]='e';
            }

            else if(campo[posx][posy-1]=='p')
            {
                dir_scan=(8-dir+1+3)%4;
                campo[posx][posy-1]='e';
            }
        }
        else
        {
          if(VA_A_DESTRA)
          {
            while(dir_scan<4&&!trovato)
            {
                switch((dir+dir_scan+3)%4)//direzione robot + direzione scan
                {
                case 0:
                    if(campo[posx+1][posy]!='w' && campo[posx+2][posy]=='?') // priority O se non c'è muro in questa direzione  E  se la casella da quella è sconosciuta
                    {
                        trovato=true;
                    }
                    else
                    {
                        dir_scan++;
                    }
                    break;

                case 1:
                    if(campo[posx][posy+1]!='w' && campo[posx][posy+2]=='?') // se non c'è muro in questa direzione  E  se la casella da quella è sconosciuta
                    {
                        trovato=true;
                    }
                    else
                    {
                        dir_scan++;
                    }
                    break;

                case 2:
                    if(campo[posx-1][posy]!='w' && campo[posx-2][posy]=='?') // se non c'è muro in questa direzione  E  se la casella da quella è sconosciuta
                    {
                        trovato=true;
                    }
                    else
                    {
                        dir_scan++;
                    }
                    break;

                case 3:
                    if(campo[posx][posy-1]!='w' && campo[posx][posy-2]=='?') // se non c'è muro in questa direzione  E  se la casella da quella è sconosciuta
                    {
                        trovato=true;
                    }
                    else
                    {
                        dir_scan++;
                    }
                    break;
                }
            }
            
          }
          else//sinistra
          {
            while(dir_scan<4&&!trovato)
            {
                switch((dir+(6-dir_scan)+3)%4)//direzione robot + direzione scan
                {
                case 0:
                    if(campo[posx+1][posy]!='w' && campo[posx+2][posy]=='?') // priority O se non c'è muro in questa direzione  E  se la casella da quella è sconosciuta
                    {
                        trovato=true;
                    }
                    else
                    {
                        dir_scan++;
                    }
                    break;

                case 1:
                    if(campo[posx][posy+1]!='w' && campo[posx][posy+2]=='?') // se non c'è muro in questa direzione  E  se la casella da quella è sconosciuta
                    {
                        trovato=true;
                    }
                    else
                    {
                        dir_scan++;
                    }
                    break;

                case 2:
                    if(campo[posx-1][posy]!='w' && campo[posx-2][posy]=='?') // se non c'è muro in questa direzione  E  se la casella da quella è sconosciuta
                    {
                        trovato=true;
                    }
                    else
                    {
                        dir_scan++;
                    }
                    break;

                case 3:
                    if(campo[posx][posy-1]!='w' && campo[posx][posy-2]=='?') // se non c'è muro in questa direzione  E  se la casella da quella è sconosciuta
                    {
                        trovato=true;
                    }
                    else
                    {
                        dir_scan++;
                    }
                    break;
                }
            }
            dir_scan=(6-dir_scan)%4;
          }
        }

        if(!trovato)
        {
            //ricerca percorso
            if(!priority_path_to('?'))
            {
                //Nessun percorso per ? trovato
                if(campo[posx][posy]=='s')
                    return true;

                if(!priority_path_to('s'))
                    return false;//Nessun percorso per s trovato
            }
        }
        else
        {
            /*switch(dir_scan)
            {
            case 0:
                gira_destra();
                break;
            case 1:
                avanti();
                break;
            case 2:
                gira_sinistra();
                break;
            case 3:
                gira_destra();
                break;
            }*/
            switch(dir_scan%4)
            {
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
