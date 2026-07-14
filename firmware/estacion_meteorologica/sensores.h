// ============================================================================
//  sensores.h  ─  Lectura de DHT22 (temp/hum) y BMP180 (presión)
// ============================================================================
#pragma once
#include "agronomia.h"   // struct Muestra

// Inicializa DHT22 y BMP180. Devuelve false si el BMP180 no responde en el bus.
bool sensores_init();

// Lee ambos sensores y llena `m`. Reintenta el DHT22 (a veces devuelve NAN).
// Devuelve false si el DHT22 no dio una lectura válida (temp/hum obligatorias).
bool sensores_leer(Muestra& m);
