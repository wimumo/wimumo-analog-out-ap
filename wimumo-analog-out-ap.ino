
/************************************************************
 ************************************************************
                     WIMUMO ESP32 - ANALOG OUT

  Complemento con salida analógica para WIMUMO
************************************************************
************************************************************/

/************************************************************
                         INCLUDES
************************************************************/

#include <WiFi.h>
#include <WiFiUdp.h>
#include <driver/gpio.h>
#include <AsyncUDP.h>
#include <string.h>

/************************************************************
                      DEFINES
************************************************************/

// Define your WiFi credentials
const char* ssid = "WIMUMO_ANALOGOUT_28";
const char* password = "wifiwimumo";

// Define the UDP port to listen for OSC messages
const int localPort = 12345;

#define ledPin 12

#define pinPWM1 26
#define pinPWM2 27
#define pinPWM3 14
#define pinPWM4 13

#define pinDIG1 GPIO_NUM_18
#define pinDIG2 GPIO_NUM_19


/************************************************************
                      VARIABLES GLOBALES
************************************************************/

volatile bool led_on = false;
volatile long ms;             // Control de timing "blando" en loop()
volatile long ledrgb_ms;      // Control de timing para datos de performance
AsyncUDP udp;


/************************************************************
 ************************************************************
                  ARDUINO SETUP
************************************************************
************************************************************/


void setup() {

  
  //
  //  Inicialización de periféricos
  //
  
  Serial.begin(115200);

  Serial.println("Comenzando...");
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);
  pinMode(GPIO_NUM_32, INPUT);          // Botón pulsador


  // Se asignan los pines de PWM a los canales correspondientes
  ledcAttachPin(pinPWM1, 1); 
  ledcAttachPin(pinPWM2, 2);
  ledcAttachPin(pinPWM3, 3);
  ledcAttachPin(pinPWM4, 4);

  // PWM a 20 kHz 
  // Resolución de 10 bits (valores de 0 a 1023)
  ledcSetup(1, 20000, 10); 
  ledcSetup(2, 12000, 10);
  ledcSetup(3, 20000, 10);
  ledcSetup(4, 12000, 10);

  // Iniciar red WIFI
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  
  // Iniciar escucha para mensajes OSC
  udp.listen(localPort);

  
  //
  //   SUBRUTINA DE RECEPCIÓN DE PAQUETES OSC
  //
  udp.onPacket([](AsyncUDPPacket packet) {

    // WIMUMO envía BUNDLES OSC. La librería OSC
    // funciona mal para interpretar bundles. Por 
    // lo tanto se hace "a mano"

    // Se reciben los datos en una cadena
    char *cad = (char*)packet.data();

    int val1, val2;
    char numero[4];

    // Si se recibió el bundle de información, 
    // se descarta
    if(buscar(cad, packet.length(), "/info")!=-1){
      return;
    }


    // Detección de dato de canal 1    
    int nch1 = buscar(cad, packet.length(), "/env/ch1");
    if (nch1 != -1) {
      char *pc = cad + ((nch1 / 4) + 1) * 4;
      if ( *(pc) == ',' && *(pc + 1) == 'i')
      {
        pc += 4;
        numero[0] =  *(pc + 3);
        numero[1] =  *(pc + 2);
        numero[2] =  *(pc + 1);
        numero[3] =  *(pc + 0);

        val1 = *((int*)numero);

            
        // Conversión de valor. WIMUMO envía un dato entre
        // 0 y 2^16. Si está capturando señales reales de EMG puede
        // ser a lo sumo 5000. Por lo tanto se satura en 5120 y
        // se divide por 5 para generar una salida entre 
        // 0 y 1023 para el PWM.
        
        if(val1>5120)
          val1 = 5120;
         val1 = val1/5-1;

         if(val1<0)
          val1=0;
        
        ledcWrite(1, val1);
      }
    }

    // Detección de dato de canal 2    
    int nch2 = buscar(cad, packet.length(), "/env/ch2");
    if (nch1 != -1) {
      char *pc = cad + ((nch2 / 4) + 1) * 4;
      if ( *(pc) == ',' && *(pc + 1) == 'i')
      {
        pc += 4;
        numero[0] =  *(pc + 3);
        numero[1] =  *(pc + 2);
        numero[2] =  *(pc + 1);
        numero[3] =  *(pc + 0);

        val2 = *((int*)numero);

        // Conversión de valor 
        if(val2>5120)
          val2 = 5120;
         val2 = val2/5-1;
         if(val2<0)
          val2=0;
        ledcWrite(2, val2);
      }

        Serial.print(val1);
        Serial.print(" ");
        Serial.println(val2);
    }


   
  });

  // Inicialización de variables
  ms = millis();
  ledrgb_ms = millis();
  digitalWrite(ledPin, LOW);
  
}


/************************************************************
 ************************************************************
                  ARDUINO LOOP
************************************************************
************************************************************/
void loop() {



  if ( millis() - ledrgb_ms >= 2000) {
    ledrgb_ms = millis();
    if (led_on == false) {
      digitalWrite(ledPin, HIGH);
      led_on = true;
    }
    else {
      digitalWrite(ledPin, LOW);
      led_on = false;
    }


  }


  
  vTaskDelay(100 / portTICK_PERIOD_MS);
}


// Función auxiliar para la salida PWM
void escribir_pwm(int n, int val) {
  if (val < 0) {
    val = 0;
  }
  else if (val > 1024) {
    val = 1024;
  }

  ledcWrite(n, val);
}


// Función auxiliar para interpretar bundle OSC
int buscar(char* cad1, int n1, char* buscada)
{

  int len = strlen(buscada);
  int lim = n1;
  int i;
  int k = 0;
  for (i = 0; i < lim; i++)
  {

    if (cad1[i] == buscada[k])
    {
      k++;
      if (k == len)
      {
        break;
      }
    }
    else
    {
      k = 0;
    }
  }
  if (k == len)
  {
    return i;
  }
  return -1;
}
