#include <Arduino.h>
#include <EasyButton.h>
#include <Servo.h>
#include "jff2machine.h"
#include "MooreMachine.h"

void inicializarHardware();
void inicializarMaquinaMoore();
void leerJFFSerial(); // Leer archivo JFF desde el puerto serie
void moverServo();

#define FLASH_BUTTON 0 // Pin del botón
EasyButton flashButton(FLASH_BUTTON);
Servo servo;

MooreMachine moore;

// Funciones de salida
bool dooropening = true;

void prenderLed()
{
  dooropening = false;
  Serial.println("Led encendido");
}

void apagarLed()
{
  dooropening = true;
  Serial.println("Led apagado");
}

void idle()
{
  Serial.println("Esperar");
}

void setup()
{
  inicializarHardware();
  inicializarMaquinaMoore();
}

int servoAngle = 0;

void loop()
{
  flashButton.read();
 leerJFFSerial();
  moverServo();
  delay(10);
}

void inicializarMaquinaMoore()
{
  // Registrar funciones para cada simbolo de salida
  // (𝛤 → función)
  moore.addOutput("led_on", prenderLed);
  moore.addOutput("led_off", apagarLed);
  moore.addOutput("idle", idle);

  // Agregar estados y su simbolo de salida
  // Q, ℎ:𝑄 → 𝛤
  moore.addState("q0", "led_off");
  moore.addState("q1", "idle");
  moore.addState("q2", "led_on");

  // Estado inicial q0
  moore.setInitialState("q0");

  // Agregar transiciones
  moore.addTransition("q0", "boton", "q1");
  moore.addTransition("q1", "boton", "q2");
  moore.addTransition("q2", "boton", "q0");

  // 
  flashButton.onPressed([]() { // Callback al presionarse el boton
    moore.processInput("boton");
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
  if (dooropening)
  {
    servoAngle++;
  }
  else
  {
    servoAngle--;
  }

  if (servoAngle <= 0)
  {
    //Serial.println("Cerrado");
    servoAngle = 0;
  }

  if (servoAngle >= 180)
  {
    //Serial.println("Abierto");
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
  flashButton.begin(); // Inicializa el botón
}