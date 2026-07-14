// ============================================================================
//  supabase.h  ─  Envío de lecturas a la REST de Supabase (tabla readings)
// ============================================================================
#pragma once
#include "agronomia.h"

// POST de una lectura. `tsISO` = timestamp ISO-8601 UTC. Devuelve true si 2xx.
bool supabase_enviarLectura(const Muestra& m, const Evaluacion& e, const char* tsISO);
