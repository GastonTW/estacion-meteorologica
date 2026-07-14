// ============================================================================
//  conectividad.cpp  ─  WiFi + NTP
// ============================================================================
#include "conectividad.h"
#include "config.h"
#include "secrets.h"
#include <WiFi.h>

static bool s_ntpOk = false;

bool net_conectarWifi() {
  if (WiFi.status() == WL_CONNECTED) return true;
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("[WiFi] Conectando");
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - t0) < WIFI_TIMEOUT_MS) {
    delay(400);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("[WiFi] OK  IP: "); Serial.println(WiFi.localIP());
    return true;
  }
  Serial.println("[WiFi] FALLO de conexion");
  return false;
}

bool net_asegurarWifi() {
  if (WiFi.status() == WL_CONNECTED) return true;
  Serial.println("[WiFi] Reconectando...");
  return net_conectarWifi();
}

void net_iniciarNTP() {
  configTzTime(TZ_POSIX, NTP_SERVER_1, NTP_SERVER_2);
  Serial.print("[NTP] Sincronizando");
  struct tm tm;
  for (int i = 0; i < 20; i++) {           // hasta ~10 s
    if (getLocalTime(&tm, 500)) { s_ntpOk = true; break; }
    Serial.print(".");
  }
  Serial.println(s_ntpOk ? " OK" : " sin hora (se reintenta luego)");
}

time_t net_ahora() {
  time_t now = time(nullptr);
  return (now > 1700000000) ? now : 0;      // válido si supera ~2023
}

int net_horaLocal() {
  struct tm tm;
  if (!getLocalTime(&tm, 200)) return -1;
  return tm.tm_hour;
}

void net_isoUTC(char* buf, size_t len) {
  time_t now = time(nullptr);
  struct tm tmUTC;
  gmtime_r(&now, &tmUTC);
  strftime(buf, len, "%Y-%m-%dT%H:%M:%SZ", &tmUTC);
}
