#include <Arduino.h>
#include <EasyButton.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

#include "jff2machine.h"
#include "MooreMachine.h"

void inicializarHardware();
void inicializarMaquinaMoore();
void setLeds(uint8_t r, uint8_t g, uint8_t b);
void leerJFFSerial(); // Leer archivo JFF desde el puerto serie
void moverServo();
void verificarIR();

#define LED_PIN D6
#define NUMPIXELS 3

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

#define LED_BRIGHTNESS 180
#define IR_PIN D2      // Pin del sensor IR
#define FLASH_BUTTON 0 // Pin del botón
EasyButton button(FLASH_BUTTON);
Servo servo;

bool abriendoBarrera = true;
int servoAngle = 0;

MooreMachine moore;

/////////////////////////// Funciones de salida ///////////////////////////
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

void ledRojo()
{
  setLeds(255, 0, 0);
}

void ledVerde()
{
  setLeds(0, 255, 0);
}

void ledAmarillo()
{
  setLeds(255, 255, 0);
}

void apagarLeds()
{
  pixels.clear();
  pixels.show();
}

/////////////////////////// Fin funciones de salida ///////////////////////////
void setup()
{
  inicializarHardware();
  inicializarMaquinaMoore();
}

void loop()
{
  // Leer archivo JFLAP desde el puerto serie
  leerJFFSerial();

  // Actualizar botón
  button.read();

  verificarIR();

  moverServo();
  delay(10);
}

void inicializarMaquinaMoore()
{
  /*
  Entradas (transiciones):
    _out (Disparada despues de cada estado)
    bot (Boton pulsado)
    barrObst (Sensor de barrera obstruido)
    barrLibre (Sensor de barrera libre)
    sAbi (Sensor barrera abierta)
    sCer (Sensor barrera cerrada)

  Salidas:
    idle (No hacer nada)
    abr (Abrir barrera)
    cer (Cerrar barrera)
    rojo (Encender luz roja)
    amarillo (Encender luz amarilla)
    verde (Encender luz verde)
    ledOff (Apagar leds)
  */

  // Registrar funciones para cada simbolo de salida
  // (𝛤 → función)
  moore.addOutput("idle", idle);
  moore.addOutput("abr", abrirBarrera);
  moore.addOutput("cer", cerrarBarrera);
  moore.addOutput("rojo", ledRojo);
  moore.addOutput("verde", ledVerde);
  moore.addOutput("amarillo", ledAmarillo);
  moore.addOutput("ledOff", apagarLeds);

  // Entradas segun perifericos
  button.onPressed([]() {      // Callback al presionarse el boton
    moore.processInput("bot"); // Simbolo de entrada
  });

  // Maquina por defecto:

  // Agregar estados y su simbolo de salida
  // Q, ℎ:𝑄 → 𝛤
  moore.addState("q0", "abr");
  moore.addState("q1", "verde");
  moore.addState("q2", "rojo");
  moore.addState("q3", "cer");

  // Estado inicial q0
  moore.setInitialState("q0");

  // Agregar transiciones
  moore.addTransition("q0", "sAbi", "q1");
  moore.addTransition("q1", "bot", "q2");
  moore.addTransition("q2", "_out", "q3");
  moore.addTransition("q3", "bot", "q0");

  moore.reset();
}

// Leer archivo JFLAP desde el puerto serie (terminado con @) y carga la máquina de Moore
void leerJFFSerial()
{
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
      catch (const std::exception &e)
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

// Verificar si hay un obstáculo en la barrera
void verificarIR()
{
  bool ir = digitalRead(IR_PIN);
  if (!ir) // es active low
  {
    moore.processInput("barrObst");
  }
  else
  {
    moore.processInput("barrLibre");
  }
}

// Setea el color de los leds
void setLeds(uint8_t r, uint8_t g, uint8_t b)
{
  for (int i = 0; i < NUMPIXELS; i++)
    pixels.setPixelColor(i, pixels.Color(r, g, b));
  pixels.show();
}

void moverServo()
{
  // Mover según sentido
  if (abriendoBarrera)
  {
    servoAngle--;
  }
  else
  {
    servoAngle++;
  }

  // Fines de carrera virtuales
  if (servoAngle <= 0)
  {
    moore.processInput("sAbi");
    servoAngle = 0;
  }

  if (servoAngle >= 180)
  {
    moore.processInput("sCer");
    servoAngle = 180;
  }

  // Mover servo
  servo.write(servoAngle);
}

void inicializarHardware()
{
  Serial.begin(115200); // Inicializa el puerto serie

  // Configurar leds y probar
  pixels.begin();
  pixels.setBrightness(LED_BRIGHTNESS);
  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  pixels.setPixelColor(1, pixels.Color(0, 255, 0));
  pixels.setPixelColor(2, pixels.Color(0, 0, 255));
  pixels.show();
  delay(1000);
  pixels.clear();
  pixels.show();

  // Configurar servo
  servo.attach(D4);
  servo.write(0);

  // Configurar sensor de barrera
  pinMode(IR_PIN, INPUT);

  button.begin(); // Inicializa el botón
}