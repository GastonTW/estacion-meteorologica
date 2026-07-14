// ============================================================================
//  ESTACION METEOROLOGICA ANTIHELADAS  ─  Cerezos
//  Placa: ESP32 (38 pines) · Sensores: DHT22 (temp/hum) + BMP180 (presion)
//
//  Flujo: lee sensores -> calcula riesgo (skill agronomica) -> postea a
//  Supabase -> alerta por Telegram si el estado sube de nivel.
//
//  Lógica de dominio: .claude/skills/skill-agronomica  (FUENTE DE VERDAD)
//  Config editable: config.h   ·   Credenciales: secrets.h (copiar del example)
// ============================================================================
#include "config.h"
#include "agronomia.h"
#include "sensores.h"
#include "conectividad.h"
#include "supabase.h"
#include "telegram.h"

static unsigned long s_proximoMuestreo = 0;
static Estado        s_estadoPrevio    = MONITOREO_NORMAL;
static bool          s_primera         = true;

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("\n=== Estacion Meteorologica Antiheladas ===");

  if (!sensores_init())
    Serial.println("[Sensores] BMP180 no responde (se continua solo con DHT22)");

  agro_init();
  net_conectarWifi();
  net_iniciarNTP();

  telegram_enviar(String("\xF0\x9F\x9F\xA2 Estacion ") + DEVICE_ID +
                  " iniciada y monitoreando.");

  s_proximoMuestreo = millis();   // primer muestreo inmediato
}

void loop() {
  if ((long)(millis() - s_proximoMuestreo) < 0) return;   // aún no toca

  net_asegurarWifi();

  Muestra m;
  if (!sensores_leer(m)) {
    Serial.println("[Sensores] Lectura DHT22 invalida, reintento en 10 s");
    s_proximoMuestreo = millis() + 10000;
    return;
  }

#if MODO_TEST
  m.temp = TEST_TEMP_C;           // inyecta temperatura de prueba (docs/04)
  Serial.printf("[TEST] Temperatura forzada a %.1f C\n", m.temp);
#endif

  time_t ahora    = net_ahora();
  int    horaLoc  = net_horaLocal();
  Evaluacion e    = agro_evaluar(m, ahora, horaLoc);

  char tsISO[24] = "";
  if (ahora > 0) net_isoUTC(tsISO, sizeof(tsISO));

  // --- Log serie por Serial ---
  Serial.printf("[Muestra] T=%.1fC HR=%.0f%% P=%.1fhPa Td=%.1fC tasa=%.2fC/h "
                "min~=%.1fC score=%d -> %s\n",
                e.tempSuavizada, m.humedad, m.presionValida ? m.presion : NAN,
                e.puntoRocio, e.tasaEnfriamiento, e.tempMinEstimada,
                e.score, agro_estadoClave(e.estado));

  // --- Nube ---
  supabase_enviarLectura(m, e, tsISO);

  // --- Alerta al subir/cambiar de nivel ---
  if (!s_primera && e.estado != s_estadoPrevio) {
    telegram_alertaEstado(m, e, s_estadoPrevio, tsISO);
  }
  s_estadoPrevio = e.estado;
  s_primera = false;

  // --- Reprograma según el estado (tasa de muestreo adaptativa) ---
  s_proximoMuestreo = millis() + agro_intervaloMuestreo(e.estado);
}
