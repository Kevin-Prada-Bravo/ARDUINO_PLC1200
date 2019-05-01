#include <SPI.h>
#include <Ethernet.h>
#include "Settimino.h"
//INGRESE UNA DIRECCION MAC Y UNA IP
// LA DIRECCION IP DEPENDE DE LA RED LOCAL
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0F, 0x08, 0xE11 };
  
IPAddress Local(192,168,0,50); // DIRECCION IP DE ARDUINO
IPAddress PLC(192,168,0,10);   // DIRECCION IP DEL PLC
 
byte Buffer[1024]; //BUFFER DONDE SE ALMACENA LOS DATOS DEL DB
//DEFINIMOS SENSORES ANALOGICOS
#define ANALOG_1 A0
#define ANALOG_2 A1
#define ANALOG_3 A2
#define ANALOG_4 A3
#define ANALOG_5 A4
//DEFINIMOS SENSORES DIGITALES
#define PIR 30
#define BUTTON_1 31
#define BUTTON_2 32
#define BUTTON_3 33
#define BUTTON_4 34  
//DEFINIMOS LOS PINES DE LOS ACTUADORES
#define LED 35
#define BUZZER 36
#define LED_1 37
#define LED_2 38
#define LED_3 39
#define LED_4 40
#define LED_5 41
#define LED_6 42
#define LED_7 43
#define LED_8 44
#define LED_RUN 45
#define LED_ERROR 46
//ARRAY DE LOS SENSORES BOOLEANOS
int INPUTS[]={PIR,BUTTON_1,BUTTON_2,BUTTON_3,BUTTON_4};
//ARRAY DE LOS VALORES BOOLEANOS
int INPUTS_VAL[5]={};
//ARRAY DE LOS SENSORES ANALOGICOS 
int SEN_ANALOG[]={ANALOG_1,ANALOG_2,ANALOG_3,ANALOG_4,ANALOG_5};
//ARRAY DE LOS VALORES ANALOGICOS
float VAL_SEN_A0[5]={};
//ARRAY DE SALIDAS
int OUTPUTS[]={LED,BUZZER,LED_RUN,LED_ERROR,LED_1,LED_2,LED_3,LED_4,LED_5,LED_6,LED_7,LED_8};
// INICIO DE LA LIBRERIA SETTIMINO
S7Client Client;
unsigned long Elapsed; // TIEMPO DE EJECUCION
//----------------------------------------------------------------------
// CONFIGURACION: INICIO ETHERNET Y PUERTO SERIAL 
//----------------------------------------------------------------------
void setup(){
    // ABRIR LA COMUNICACION SERIAL 
    Serial.begin(9600);
     while (!Serial) {
      ; // ESPERA QUE EL PUERTO SERIAL SE CONECTE
    }
    // INICIA LA LIBRERIA ETHERNET 
    Ethernet.begin(mac, Local);
    //CONFIGURACION DEL TIEMPO: ESPERAMOS 2000
    delay(2000); 
    Serial.println(Ethernet.localIP());
    //CONFIGURACION: PINE COMO ENTRADA
   for(int i=0;i<4;i++){
    pinMode(INPUTS[i],INPUT);
    };
    //CONFIGURACION: PINES COMO SALIDA 
   for(int i=0;i<13;i++){
    pinMode(OUTPUTS[i],OUTPUT);
    }
}
//----------------------------------------------------------------------
// CONECTAR AL PLC 
//----------------------------------------------------------------------
bool Connect(){
    int Result=Client.ConnectTo(PLC, 
                                  0,  // RACK
                                  1); // SLOT
    Serial.print("Connecting to ");
    Serial.println(PLC);  
    if (Result==0) 
    {
      Serial.print("Connected ! PDU Length = ");
      Serial.println(Client.GetPDULength());
    }
    else
      Serial.println("ERROR AL CONECTAR");
      digitalWrite(LED_ERROR,!digitalRead(LED_ERROR));
    return Result==0;
}
//----------------------------------------------------------------------
// IMPRIMIR EL NUMERO DEL ERROR
//----------------------------------------------------------------------
void CheckError(int ErrNo){
  Serial.print("Error No. 0x");
  Serial.println(ErrNo, HEX);
  
  // VERIFICA SI TENEMOS UN ERROR GRAVE ==> PARA DESCONECTAR
    if (ErrNo & 0x00FF)
    {
    Serial.println("SEVERE ERROR, disconnecting.");    
    digitalWrite(LED_ERROR,!digitalRead(LED_ERROR));    
    Client.Disconnect(); 
    
    }
}
//----------------------------------------------------------------------
// CONVERSION FLOAT / DWORD / DINT 
//----------------------------------------------------------------------
void Reverse4(void *ptr){
  byte *pb;
  byte tmp;
  pb=(byte*)(ptr);
  // INTERCAMBIAR BYTE 4 CON BYTE 1
  tmp=*(pb+3);
  *(pb+3)=*pb;
  *pb=tmp;
  // INTERCAMBIAR BYTE 3 CON BYTE 2
  tmp=*(pb+2);
  *(pb+2)=*(pb+1);
  *(pb+1)=tmp;
}
//----------------------------------------------------------------------
// CONVERSION WORD/INT
//----------------------------------------------------------------------
void Reverse2(void *ptr){
  byte *pb;
  byte tmp;
  pb=(byte*)(ptr);
  // INTERCAMBIAR BYTE 2 CON BYTE 1
  tmp=*(pb+1);
  *(pb+1)=*pb;
  *pb=tmp;
}
//----------------------------------------------------------------------
// BUCLE PRINCIPAL
//----------------------------------------------------------------------
void loop() {
  uint16_t Size;
  int Result;
  Size = sizeof(Buffer);
  //CONEXION
  while (!Client.Connected)
  {
    if (!Connect())
      delay(500);
  }  
  /*Serial.println("==================");   
  Serial.println("Uploading DB2"); */
  // LECTURA 
  /**LECTURA DE VARIABLES
    Result=Client.ReadArea(S7AreaDB,  ==> LEER DB
                           100,       ==> N° DE DB
                           0,         ==> BYTE INICIAL
                           2,         ==> BYTES A LEER
                           &Buffer);  ==> BUFFER INTERNO
  **/
  Result=Client.ReadArea(S7AreaDB,100,0,2,&Buffer);// COMO SALIDA SE TIENE LOS DATOS LEIDOS 
  /** LECTURAD DE DATOS 
    digitalWrite(OUTPUTS[0],      ==> VARIABLE DONDE SE GUARDA EL DATO
                  S7.BitAt(0, 0)  ==> VARIABLE LEIDA DESDE EL PLC S7.BitAt(BYTE, BIT)
                 );               
  **/
  if (Result==0){ 
  //LECTURA DE VARABLES DEL PLC 
  digitalWrite(OUTPUTS[0],S7.BitAt(0, 0));
  digitalWrite(OUTPUTS[1],S7.BitAt(0, 1));  
  digitalWrite(LED_1,S7.BitAt(0, 2));
  digitalWrite(LED_2,S7.BitAt(0, 3));
  digitalWrite(LED_3,S7.BitAt(0, 4));
  digitalWrite(LED_4,S7.BitAt(0, 5));
  digitalWrite(LED_5,S7.BitAt(0, 6));
  digitalWrite(LED_6,S7.BitAt(0, 7));
  digitalWrite(LED_7,S7.BitAt(1, 0));
  digitalWrite(LED_8,S7.BitAt(1, 1));  
  /** ESCRITURA SENSORES ANALOGICOS 
    Client.WriteArea(S7AreaDB,        ==> DB
                     200,             ==> N° DB
                     0,               ==> DIRECION DE VARIABLE PLC
                     sizeof(float),   ==> TIPO DE VARIABLE
                     &VAL_SEN_A0[0]); ==> VALOR 
  **/
  
  Reverse4(&VAL_SEN_A0[0]); //CONVERTIMOS A DWORD 
  Client.WriteArea(S7AreaDB, 200, 0, sizeof(float), &VAL_SEN_A0[0]);
  Reverse4(&VAL_SEN_A0[1]);
  Client.WriteArea(S7AreaDB, 200, 4, sizeof(float), &VAL_SEN_A0[1]);   
  Reverse4(&VAL_SEN_A0[2]);
  Client.WriteArea(S7AreaDB, 200, 8, sizeof(float), &VAL_SEN_A0[2]); 
  Reverse4(&VAL_SEN_A0[3]);
  Client.WriteArea(S7AreaDB, 200, 12, sizeof(float), &VAL_SEN_A0[3]);
  Reverse4(&VAL_SEN_A0[4]);
  Client.WriteArea(S7AreaDB, 200, 16, sizeof(float), &VAL_SEN_A0[4]);
  
  //ESCRITURA SENSORES BOLEANOS
  Reverse2(&INPUTS_VAL[0]);
  Client.WriteArea(S7AreaDB, 200, 26, sizeof(int), &INPUTS_VAL[0]);  
  Reverse2(&INPUTS_VAL[1]);
  Client.WriteArea(S7AreaDB, 200, 20, sizeof(int), &INPUTS_VAL[1]); 
  Reverse2(&INPUTS_VAL[2]);
  Client.WriteArea(S7AreaDB, 200, 22, sizeof(int), &INPUTS_VAL[2]); 
  Reverse2(&INPUTS_VAL[3]);
  Client.WriteArea(S7AreaDB, 200, 24, sizeof(int), &INPUTS_VAL[3]); 
  Reverse2(&INPUTS_VAL[4]);
  Client.WriteArea(S7AreaDB, 200, 28, sizeof(int), &INPUTS_VAL[4]);

  //LED DE ERROR
  digitalWrite(LED_RUN,!digitalRead(LED_RUN)); // LED DE RUN
  digitalWrite(LED_ERROR,LOW); //LED ERROR 
  } 
  else{
    CheckError(Result);  
    digitalWrite(LED_RUN,LOW);
   
  }
  //LECTURA DE ENTRADA ANALOGICOS 
    for(int i=0;i<5;i++){
      VAL_SEN_A0[i]=analogRead(SEN_ANALOG[i]);
      }
  //LECTURA DE ENTRADAS BOOLEANAS  
      for(int i=0;i<5;i++){
      INPUTS_VAL[i]=digitalRead(INPUTS[i]);
      }
      delay(10);
}
