// ============================================================================
//  telegram.h  ─  Alertas push por Telegram (Bot API)
// ============================================================================
#pragma once
#include "agronomia.h"

// Envía un mensaje de texto al chat configurado. Devuelve true si 2xx.
bool telegram_enviar(const String& texto);

// Envía la alerta de cambio de estado (formatea T, HR, rocío, presión, plan).
// Llamar SOLO cuando el estado cambió (ver skill §4).
bool telegram_alertaEstado(const Muestra& m, const Evaluacion& e,
                           Estado anterior, const char* tsISO);
