// ============================================================================
//  telegram.cpp  ─  Bot API de Telegram
// ============================================================================
#include "telegram.h"
#include "config.h"
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <math.h>

static const char* iconoEstado(Estado e) {
  switch (e) {
    case HELADA_ACTIVA:   return "\xF0\x9F\x94\xB4";   // 🔴
    case ALERTA_CRITICA:  return "\xF0\x9F\x9F\xA0";   // 🟠
    case ALERTA_TEMPRANA: return "\xF0\x9F\x9F\xA1";   // 🟡
    default:              return "\xF0\x9F\x9F\xA2";   // 🟢
  }
}

bool telegram_enviar(const String& texto) {
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  String url = String("https://api.telegram.org/bot") + TELEGRAM_BOT_TOKEN + "/sendMessage";
  if (!http.begin(client, url)) return false;
  http.setTimeout(HTTP_TIMEOUT_MS);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<1024> doc;
  doc["chat_id"] = TELEGRAM_CHAT_ID;
  doc["text"]    = texto;
  String body; serializeJson(doc, body);

  int code = http.POST(body);
  bool ok = (code >= 200 && code < 300);
  if (!ok) Serial.printf("[Telegram] ERROR %d: %s\n", code, http.getString().c_str());
  else     Serial.println("[Telegram] mensaje enviado");
  http.end();
  return ok;
}

static void agregarFloat(String& s, const char* etiqueta, float v, const char* unidad) {
  s += etiqueta;
  if (isnan(v)) s += "n/d";
  else          s += String(v, 1);
  s += unidad;
  s += "\n";
}

bool telegram_alertaEstado(const Muestra& m, const Evaluacion& e,
                           Estado anterior, const char* tsISO) {
  String msg;
  msg += iconoEstado(e.estado);
  msg += " ESTACION ";
  msg += DEVICE_ID;
  msg += "\n";
  msg += agro_estadoTexto(e.estado);
  msg += "\n\n";
  agregarFloat(msg, "Temp: ",     e.tempSuavizada, " C");
  agregarFloat(msg, "Humedad: ",  m.humedad, " %");
  agregarFloat(msg, "Presion: ",  m.presionValida ? m.presion : NAN, " hPa");
  agregarFloat(msg, "P.rocio: ",  e.puntoRocio, " C");
  agregarFloat(msg, "Min.estimada: ", e.tempMinEstimada, " C");
  msg += "Score riesgo: ";
  msg += String(e.score);
  msg += "\n\n";
  msg += agro_planAccion(e.estado);
  msg += "\n\n(";
  msg += tsISO;
  msg += ")";
  return telegram_enviar(msg);
}
