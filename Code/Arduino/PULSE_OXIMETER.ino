#include <ArduinoJson.h>

int PWM_IR = 9;
int PWM_RED = 6;
int PWM_VALUE_IR = 0; //valor de PWM para ajustar la intensida de los LEDs
int PWM_VALUE_RED = 0; //valor de PWM para ajustar la intensida de los LEDs
int PWM_VALUE_MANUAL_IR = 0 ;
int PWM_VALUE_MANUAL_RED = 0 ;
int RED = 10;
int IR = 11;
int READ = 0;
int READ_VALUE = 0;   //valor leido por entrada analógica
int calibracion(); // inicializamos la funcion que calibra automaticamente la intensidad de los led

int value_red = 0; // valor leido con luz roja
int value_ir = 0; // valor leido con luz ir
int value_am = 0; //valor leido con luz ambiente

//char* mode = "STOP"; // almacenamos el valor del estado de nuestro dispositivo

boolean mode = 0;
boolean mode_light = 0; // en esta variable almacenamos la variable que nos indica si el usuario quiere controla la intensidad del LED de forma manual o automática
boolean flag_2 = 0;

String string=""; //aquí almacenamos el valor leido de la comunicacion serie con LABVIEW
String s = "";

char inChar;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(PWM_IR, OUTPUT);
pinMode(PWM_RED, OUTPUT);
pinMode(RED, OUTPUT);
pinMode(IR, OUTPUT);
pinMode(READ , INPUT);


digitalWrite(RED, HIGH); // Apagamos el LED rojo antes de comenzar
digitalWrite(IR, HIGH); // Apagamos el LED ir antes de comenzar
digitalWrite(PWM_IR, LOW); // Ponemos la intensida de los LEDs a cero antes de comenzar
digitalWrite(PWM_RED, LOW); // Ponemos la intensida de los LEDs a cero antes de comenzar


string.reserve(200); 

}

void loop() {
  
  // put your main code here, to run repeatedly:


//leemos lo que nos manda LABVIEW
string = " ";
while(Serial.available() > 0) {

inChar = Serial.read();
 string+=inChar;
}




//HEMOS RECIBIDO UN STRING EN FORMATO JSON, AHORA EXTRAEMOS LOS PARÁMETROS QUE NOS INTERESAN

StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.parseObject(string);

mode  = root["status"];
mode_light      = root["mode"];
PWM_VALUE_MANUAL_IR  = root["LightIntensity_IR"];
PWM_VALUE_MANUAL_RED  = root["LightIntensity_RED"];


//Serial.println(PWM_VALUE_MANUAL);


delay(500);

if (mode == 1){
  flag_2 = 1;
}

while (flag_2 == 1){ // SI LABVIEW ME MANDA 'START' COMENZAMOS...
  
  if(mode_light = 0){ // SI mode_light es un 0 me indica que vamos a trabajar en modo automatico

    //PWM_VALUE_RED = calibracion(PWM_RED); LO DESACTIVO DE MOMENTO
    //PWM_VALUE_IR = calibracion(PWM_IR); LO DESACTIVO DE MOMENTO

    PWM_VALUE_RED = map(PWM_VALUE_MANUAL_RED, 0, 100, 0, 255);
    PWM_VALUE_IR = map(PWM_VALUE_MANUAL_IR, 0, 100, 0, 255);
    }

    
  else { // Si mode_light es un 1 me indica que vamos a trabajar como modo de intensidad de los LEds manual
    PWM_VALUE_RED = map(PWM_VALUE_MANUAL_RED, 0, 100, 0, 255);
    PWM_VALUE_IR = map(PWM_VALUE_MANUAL_IR, 0, 100, 0, 255);
    }

 analogWrite(PWM_IR,PWM_VALUE_IR);
 analogWrite(PWM_RED,PWM_VALUE_RED);

    // AQUÍ PODEMOS COMENZAR CON LOS DISPAROS DE LOS LEDS
 
  // PRIMERA PARTE DEL CICLO: LED ROJO
  
  digitalWrite(RED, LOW); // LED ROJO ON
  digitalWrite(IR, HIGH); // LED IR OFF
 // espera(696);
//espera(4000);
  value_red = analogRead(READ);

delay(1);
 // espera(40);
  
 
 
  // SEGUNDA PARTE DEL CICLO: LED IR

  digitalWrite(RED, HIGH); // LED ROJO OFF
  digitalWrite(IR, LOW); // LED IR ON
// espera(696);

//espera(4000);
delay(1);

  value_ir = analogRead(READ);

//  espera(40);

  

  // TERCERA PARTE DEL CICLO: LED ROJO Y IR OFF. LUZ AMBIENTE

/*
  digitalWrite(RED, HIGH); // LED ROJO OFF
  digitalWrite(IR, HIGH); // LED IR OFF
// espera(696);
espera(10000);

  value_am = analogRead(READ);

  espera(40);


*/
  // AHORA ENVIO LOS DATOS OBTENIDOS DEL SENSOR A TRAVES DEL PUERTO SERIE PARA SER INTERPRETADOS EN LABVIEW

  s = "{\"RED\": ";
  s += String(value_red);
  s += ", \"IR\": ";
  s += String(value_ir);
  s += ", \"AM\": ";
  s += String(value_am);
  s += "}";
  
  Serial.println(s);





  
  // Antes de acabar actualizamos los datos leyendo lo que nos ha enviado LABVIEW
    if (Serial.available() > 0){
      
    string= " ";
    delay(500);
    while (Serial.available() > 0) {
    inChar = Serial.read();
    string+=inChar;
    }
    
    }

// CAMBIAR ESTO PARA QUE SE HAGA CADA CIERTO TIEMPO Y NO SIEMPRE
    
StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.parseObject(string);

mode  = root["status"];
mode_light      = root["mode"];
PWM_VALUE_MANUAL_IR  = root["LightIntensity_IR"];
PWM_VALUE_MANUAL_RED  = root["LightIntensity_RED"];


if (mode == 1){
  flag_2 = 1;
}

if (mode ==0){
  flag_2= 0;
  }
  
  }


}  // FIN DEL CODIGO PRINCIPAL


////************ FUNCION PARA ESPERAR 700 MICROSEGUNDOS *********************////
// Vamos a muestrear cada 480 Hz, esto nos deja un periodo de  2.08 mili segundos. En este periodo vamos a hacer
// tres mediciones una con led Rojo, otra led IR y otra con luz ambiente, por lo que cambiaremos el valor de los led cada 2.08 / 3 mili sengudos lo que nos djea
// aproximadamente 700 microsegundos

void espera (int tiempo){
  int flag = 0;
 


  
unsigned long millisAntes = micros();

  while(flag == 0){
    unsigned long millisAhora = micros();
    
      if((millisAhora-millisAntes) > tiempo) {
      flag = 1;
        }
   
    }
    
    }
  
///////////////////////////*******
int calibracion (int LED){
  int intensidad = 0;
  int i = 0;
  int lim_sup = 800;
  int lim_inf = 700;
  int lectura = 0;

 for (int i=0; i <= 255; i++){
    digitalWrite(LED, i); 
    digitalWrite(RED, LOW); 
    
    delay(500);
    lectura = analogRead(READ);

    if(lectura < lim_sup && lectura > lim_inf){
      intensidad = i;
      break;
      }
        }

  return intensidad;
  }
//////////**************

