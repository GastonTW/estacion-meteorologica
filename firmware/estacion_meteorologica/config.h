// ============================================================================
//  config.h  ─  Configuración NO secreta de la estación meteorológica
//  Editá estos valores según tu instalación. Las credenciales van en secrets.h
// ============================================================================
#pragma once

// ---------------------------------------------------------------------------
//  Identificación del equipo
// ---------------------------------------------------------------------------
#define DEVICE_ID        "estacion-01"     // identificador de esta estación

// ---------------------------------------------------------------------------
//  Pines de sensores (ESP32 38 pines)  ─  ver docs/01-conexionado.md
// ---------------------------------------------------------------------------
#define PIN_DHT          4                  // DHT22 (dato).  Pull-up 10k a 3V3
#define DHT_TIPO         DHT22              // DHT22 / AM2302
#define PIN_I2C_SDA      21                 // BMP180 SDA
#define PIN_I2C_SCL      22                 // BMP180 SCL

// ---------------------------------------------------------------------------
//  Ventana nocturna para tasa de enfriamiento y mínima estimada
//  (horas locales; la skill permite un horario fijo configurable)
// ---------------------------------------------------------------------------
#define HORA_ATARDECER   20                 // se captura T de referencia ~acá
#define HORA_AMANECER     7                 // hora estimada de mínima

// ---------------------------------------------------------------------------
//  Zona horaria (para timestamps NTP)  ─  Argentina = UTC-3, sin DST
//  Formato POSIX TZ: el offset tiene signo invertido (UTC-3 => "ART3")
// ---------------------------------------------------------------------------
#define TZ_POSIX         "ART3"
#define NTP_SERVER_1     "pool.ntp.org"
#define NTP_SERVER_2     "time.google.com"

// ---------------------------------------------------------------------------
//  Reintentos de red / tiempos
// ---------------------------------------------------------------------------
#define WIFI_TIMEOUT_MS       20000         // espera máx. de conexión WiFi
#define HTTP_TIMEOUT_MS       10000         // timeout de POST HTTP
#define MUESTREO_ARRANQUE_MS  5000          // primer muestreo tras bootear

// ---------------------------------------------------------------------------
//  Modo de prueba: fuerza estados para validar alertas sin esperar una helada.
//  Ver docs/04-testeos.md.  Dejar en 0 en producción.
//      1 = inyecta una temperatura simulada (TEST_TEMP_C) en cada lectura
// ---------------------------------------------------------------------------
#define MODO_TEST         0
#define TEST_TEMP_C      -1.0f
