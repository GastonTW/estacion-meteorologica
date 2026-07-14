// ============================================================================
//  sensores.cpp  ─  DHT22 + BMP180
//
//  Librerías (Library Manager):
//    - "DHT sensor library" (Adafruit) + "Adafruit Unified Sensor"
//    - "Adafruit BMP085 Library"  (compatible con BMP180)
// ============================================================================
#include "sensores.h"
#include "config.h"
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>
#include <math.h>

static DHT           dht(PIN_DHT, DHT_TIPO);
static Adafruit_BMP085 bmp;
static bool          s_bmpOk = false;

bool sensores_init() {
  dht.begin();
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  s_bmpOk = bmp.begin();          // BMP180 responde en 0x77
  return s_bmpOk;
}

bool sensores_leer(Muestra& m) {
  // --- DHT22: hasta 3 intentos (lectura ocasionalmente NAN) ---
  float t = NAN, h = NAN;
  for (int i = 0; i < 3; i++) {
    h = dht.readHumidity();
    t = dht.readTemperature();
    if (!isnan(t) && !isnan(h)) break;
    delay(2100);                   // DHT22: mín. ~2 s entre lecturas
  }
  if (isnan(t) || isnan(h)) return false;

  m.temp    = t;
  m.humedad = h;

  // --- BMP180: presión en Pa → hPa ---
  if (s_bmpOk) {
    float pa = bmp.readPressure();          // Pa
    m.presion = pa / 100.0f;                 // hPa
    m.presionValida = (m.presion > 300.0f && m.presion < 1100.0f);
  } else {
    m.presion = NAN;
    m.presionValida = false;
  }
  return true;
}
