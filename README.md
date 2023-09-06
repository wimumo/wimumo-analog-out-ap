# wimumo-analog-out-ap

Complemento receptor para WIMUMO

Este proyecto de Arduino implementa un receptor de los mensajes de WIMUMO en el ESP32 utilizando el protocolo OSC sobre UDP. Los mensajes se obtienen de los Bundles OSC recibidos, se acondicionan para la salida PWM de 10 bits y adicionalmente se envían por el puerto serie.


## Instalación

Todo está hecho en el entorno de Arduino. Deben instalarse algunas librerías para programar el ESP32 y funcionalidad como servidor.

### Tarjeta de ESP32

Para que aparezca la "tarjeta": En Archivo -> preferencias -> gestores adicionales agregar la fuente https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json   

Se seleccionó placa WEMOS D1 MINI ESP32

### Librerías del proyecto: 

- https://github.com/me-no-dev/ESPAsyncWebServer (descargar y descomprimir en Program Files/Arduino/libraries)
