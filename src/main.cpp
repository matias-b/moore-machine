#include <Arduino.h>
#include <EasyButton.h>
#include <Servo.h>
#include "jff2machine.h"
#include "MooreMachine.h"

void inicializarHardware();
void inicializarMaquinaMoore();
void leerJFFSerial(); // Leer archivo JFF desde el puerto serie
void moverServo();

#define IR_PIN D2 // Pin del sensor IR
#define FLASH_BUTTON 0 // Pin del botón
EasyButton flashButton(FLASH_BUTTON);
Servo servo;

MooreMachine moore;

// Funciones de salida
bool abriendoBarrera = true;
int servoAngle = 0;

void abrirBarrera()
{
  abriendoBarrera = true;
 // Serial.println("-> Abriendo barrera");
}

void cerrarBarrera()
{
  abriendoBarrera = false;
 // Serial.println("-> Cerrando barrera");
}

void idle()
{
  Serial.println("-> Esperar");
}

void setup()
{
  inicializarHardware();
  inicializarMaquinaMoore();
}
 
void loop()
{
  leerJFFSerial();

  flashButton.read();

  bool ir = digitalRead(IR_PIN);
  if (!ir) //active low
  {
    moore.processInput("barrObst");
  }else{
    moore.processInput("barrLibre");
  }

  moverServo();
  delay(10);
}

void inicializarMaquinaMoore()
{
  /*Entradas (transiciones):
bot (Boton pulsado)
barrObst (Sensor de barrera obstruido)
barrLibre (Sensor de barrera libre)
sAbi (Sensor barrera abierta)
sCer (Sensor barrera cerrada)

Salidas:
idle (No hacer nada)
abr (Abrir barrera)
cer (Cerrar barrera)
lr1 (Encender led rojo)
lr0 (Apagar led rojo)
lv1 (Encender led verde)
lv0 (Apagar led verde)*/

  // Registrar funciones para cada simbolo de salida
  // (𝛤 → función)
  moore.addOutput("abr", abrirBarrera);
  moore.addOutput("cer", cerrarBarrera);
  moore.addOutput("idle", idle);

  // Agregar estados y su simbolo de salida
  // Q, ℎ:𝑄 → 𝛤
  moore.addState("q0", "cer");
  moore.addState("q1", "idle");
  moore.addState("q2", "abr");

  // Estado inicial q0
  moore.setInitialState("q0");

  // Agregar transiciones
  moore.addTransition("q0", "bot", "q1");
  moore.addTransition("q1", "bot", "q2");
  moore.addTransition("q2", "bot", "q0");

  // 
  flashButton.onPressed([]() { // Callback al presionarse el boton
    moore.processInput("bot");
  });

  moore.reset();
}

void leerJFFSerial()
{
 //Read string until @ and call loadMooreMachineFromJFFString
  static std::string jffString = "";
  while (Serial.available())
  {
    char c = Serial.read();
    if (c == '@')
    {
      Serial.println("JFF recibido");
      try
      {
        loadMooreMachineFromJFFString(jffString, moore);
      }
      catch(const std::exception& e)
      {
        std::cerr << "Error al cargar JFF: " << e.what() << '\n';
      }
    
      jffString = "";
    }
    else
    {
      jffString += c;
    }
  }
   
}
/// Funciones especificas

void moverServo()
{
  if (abriendoBarrera)
  {
    servoAngle--;
  }
  else
  {
    servoAngle++;
  }

  if (servoAngle <= 0)
  {
    //Serial.println("Abierto");
   
    moore.processInput("sAbi");
    servoAngle = 0;
  }

  if (servoAngle >= 180)
  {
     //Serial.println("Cerrado");
    moore.processInput("sCer");
    servoAngle = 180;
  }

  servo.write(servoAngle);
}

void inicializarHardware()
{
  Serial.begin(115200); // Inicializa el puerto serie
  // Configurar servo
  servo.attach(D4);
  servo.write(0);
  // Configurar sensor de barrera
  pinMode(IR_PIN, INPUT);

  flashButton.begin(); // Inicializa el botón
}