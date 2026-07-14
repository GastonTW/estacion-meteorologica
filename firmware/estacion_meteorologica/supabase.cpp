// ============================================================================
//  supabase.cpp  ─  Inserta lecturas vía PostgREST
//
//  Librería: "ArduinoJson" (Benoit Blanchon).
//  Nota TLS: usamos setInsecure() (no valida el certificado del servidor) para
//  no depender de un bundle de CA en el sketch. El anon key viaja igual por
//  HTTPS cifrado. Ver consideraciones en docs/03-configuracion-nube.md.
// ============================================================================
#include "supabase.h"
#include "config.h"
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <math.h>

// Agrega la clave con valor float, o null si es NAN.
static void addFloat(JsonObject& o, const char* k, float v) {
  if (isnan(v)) o[k] = nullptr; else o[k] = roundf(v * 100) / 100.0f;
}

bool supabase_enviarLectura(const Muestra& m, const Evaluacion& e, const char* tsISO) {
  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(HTTP_TIMEOUT_MS / 1000);

  HTTPClient http;
  String url = String(SUPABASE_URL) + "/rest/v1/readings";
  if (!http.begin(client, url)) {
    Serial.println("[Supabase] http.begin fallo");
    return false;
  }
  http.setTimeout(HTTP_TIMEOUT_MS);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("apikey", SUPABASE_ANON_KEY);
  http.addHeader("Authorization", String("Bearer ") + SUPABASE_ANON_KEY);
  http.addHeader("Prefer", "return=minimal");

  StaticJsonDocument<512> doc;
  JsonObject o = doc.to<JsonObject>();
  o["device_id"]    = DEVICE_ID;
  if (tsISO && tsISO[0]) o["ts"] = tsISO;   // si no hay hora NTP, la tabla usa now()
  addFloat(o, "temp_c",          e.tempSuavizada);
  addFloat(o, "humidity_pct",    m.humedad);
  addFloat(o, "pressure_hpa",    m.presionValida ? m.presion : NAN);
  addFloat(o, "dew_point_c",     e.puntoRocio);
  addFloat(o, "cooling_rate_cph",e.tasaEnfriamiento);
  addFloat(o, "est_min_c",       e.tempMinEstimada);
  addFloat(o, "delta_p_6h",      e.deltaPresion6h);
  o["risk_score"]   = e.score;
  o["status"]       = agro_estadoClave(e.estado);

  String body;
  serializeJson(doc, body);

  int code = http.POST(body);
  bool ok = (code >= 200 && code < 300);
  if (ok) {
    Serial.printf("[Supabase] OK (%d)\n", code);
  } else {
    Serial.printf("[Supabase] ERROR %d: %s\n", code, http.getString().c_str());
  }
  http.end();
  return ok;
}
