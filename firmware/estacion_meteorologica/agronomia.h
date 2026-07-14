// ============================================================================
//  agronomia.h  ─  Lógica de dominio (agronomía + física atmosférica)
//
//  Implementa la skill  .claude/skills/skill-agronomica  (FUENTE DE VERDAD):
//  punto de rocío (Magnus), depresión de rocío, media móvil, tasa de
//  enfriamiento y proyección de mínima, índice de riesgo compuesto y la
//  máquina de estados con histéresis.
//
//  Si cambiás una constante/fórmula acá, replicala en  web/js/agronomia.js
// ============================================================================
#pragma once
#include <Arduino.h>
#include <time.h>

// ---------------------------------------------------------------------------
//  Estados (semáforo)  ─  ver skill §4
// ---------------------------------------------------------------------------
enum Estado {
  MONITOREO_NORMAL = 0,   // verde   ─ sin riesgo
  ALERTA_TEMPRANA  = 1,   // amarillo─ preparar equipos, no accionar
  ALERTA_CRITICA   = 2,   // naranja ─ PRENDER protección (riego por aspersión)
  HELADA_ACTIVA    = 3    // rojo    ─ helada en curso, mantener protección
};

// ---------------------------------------------------------------------------
//  Umbrales del índice de riesgo (skill §3) y de la máquina de estados (§4)
//  Valores de referencia: CALIBRAR con datos de la zona (ver skill §7).
// ---------------------------------------------------------------------------
#define AGRO_P_ALTA_HPA        1015.0f   // presión alta (favorece radiativa)
#define AGRO_DP_ESTABLE_6H     2.0f      // |ΔP 6h| < esto = estable
#define AGRO_HR_BAJA_PCT       60.0f     // HR baja al atardecer
#define AGRO_TASA_ENF_FUERTE   1.5f      // °C/h de enfriamiento "fuerte"
#define AGRO_TD_CRITICO        0.0f      // punto de rocío crítico
#define AGRO_T_FRIA            3.0f      // T actual considerada fría

#define AGRO_SCORE_TEMPRANA    3         // score => ALERTA_TEMPRANA
#define AGRO_SCORE_CRITICA     5         // score => ALERTA_CRITICA
#define AGRO_TMIN_TEMPRANA     3.0f      // T_min estimada < esto => TEMPRANA
#define AGRO_T_CRITICA_ACTUAL  2.0f      // T actual < esto => CRITICA
#define AGRO_T_HELADA          0.0f      // T actual <= esto => HELADA_ACTIVA
#define AGRO_T_RESET           2.0f      // T > esto sostenido => baja de nivel
#define AGRO_RESET_MIN         30        // minutos sostenidos para bajar nivel

// ---------------------------------------------------------------------------
//  Intervalos de muestreo por estado (skill §5), en milisegundos
// ---------------------------------------------------------------------------
#define AGRO_MS_NORMAL     300000UL      // 5 min
#define AGRO_MS_TEMPRANA   180000UL      // 3 min
#define AGRO_MS_CRITICA     60000UL      // 1 min
#define AGRO_MS_HELADA      60000UL      // 1 min

// ---------------------------------------------------------------------------
//  Estructuras
// ---------------------------------------------------------------------------
struct Muestra {                 // lectura cruda de sensores
  float temp;                    // °C
  float humedad;                 // %
  float presion;                 // hPa
  bool  presionValida;           // BMP180 respondió
};

struct Evaluacion {              // resultado agronómico de una muestra
  float  tempSuavizada;          // °C (media móvil)
  float  puntoRocio;             // °C
  float  depresionRocio;         // °C
  float  tasaEnfriamiento;       // °C/h (positiva = enfriándose); NAN si no hay datos
  float  tempMinEstimada;        // °C proyectada al amanecer; NAN si no hay datos
  float  deltaPresion6h;         // hPa; NAN si no hay 6h de historial
  int    score;                  // índice de riesgo compuesto
  Estado estado;
};

// ---------------------------------------------------------------------------
//  API
// ---------------------------------------------------------------------------
void        agro_init();
// Procesa una muestra: actualiza buffers internos (media móvil, historial de
// presión/temperatura) y la máquina de estados. Devuelve la evaluación completa.
// `ahora` es epoch UTC; `horaLocal` 0-23 se usa para la ventana nocturna.
Evaluacion  agro_evaluar(const Muestra& m, time_t ahora, int horaLocal);

uint32_t    agro_intervaloMuestreo(Estado e);   // ms según estado
const char* agro_estadoClave(Estado e);         // "MONITOREO_NORMAL", ...
const char* agro_estadoTexto(Estado e);         // texto humano corto
const char* agro_planAccion(Estado e);          // recomendación al productor

// Funciones puras expuestas para tests / reutilización
float agro_puntoRocio(float tempC, float humedadRel);
float agro_depresionRocio(float tempC, float td);
int   agro_indiceRiesgo(float presion, float deltaP6h, float humedad,
                        float tasaEnf, float puntoRocio, float tempActual);
