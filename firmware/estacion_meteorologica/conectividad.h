// ============================================================================
//  conectividad.h  ─  WiFi + hora NTP
// ============================================================================
#pragma once
#include <Arduino.h>
#include <time.h>

// Conecta al WiFi (bloqueante hasta WIFI_TIMEOUT_MS). Devuelve estado.
bool net_conectarWifi();

// Reconecta si se cayó. Llamar al inicio de cada ciclo. Devuelve true si hay red.
bool net_asegurarWifi();

// Sincroniza la hora por NTP (una vez tras conectar).
void net_iniciarNTP();

// Devuelve epoch UTC actual (0 si aún no hay hora válida).
time_t net_ahora();

// Hora local 0-23 según la zona horaria configurada.
int net_horaLocal();

// Timestamp ISO-8601 UTC ("2026-09-15T03:42:00Z") en `buf` (mín. 21 bytes).
void net_isoUTC(char* buf, size_t len);
