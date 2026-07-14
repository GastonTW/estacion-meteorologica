// ============================================================================
//  agronomia.cpp  ─  Implementación de la skill agronómica
// ============================================================================
#include "agronomia.h"
#include "config.h"
#include <math.h>

// ---------------------------------------------------------------------------
//  Media móvil de temperatura (skill §2.4)
// ---------------------------------------------------------------------------
#define VENTANA_MEDIA 6
static float s_bufMedia[VENTANA_MEDIA];
static int   s_idxMedia = 0;
static int   s_nMedia   = 0;

static float mediaMovil(float nueva) {
  s_bufMedia[s_idxMedia] = nueva;
  s_idxMedia = (s_idxMedia + 1) % VENTANA_MEDIA;
  if (s_nMedia < VENTANA_MEDIA) s_nMedia++;
  float suma = 0;
  for (int i = 0; i < s_nMedia; i++) suma += s_bufMedia[i];
  return suma / s_nMedia;
}

// ---------------------------------------------------------------------------
//  Serie temporal genérica (buffer circular con muestreo espaciado)
// ---------------------------------------------------------------------------
template <int N>
struct Serie {
  time_t t[N];
  float  v[N];
  int    n = 0;          // cantidad de muestras válidas
  time_t ultimo = 0;     // epoch de la última muestra guardada

  void push(time_t ahora, float valor, long minSepSeg) {
    if (n > 0 && (ahora - ultimo) < minSepSeg) return;   // muy seguido: ignorar
    if (n == N) {                                        // lleno: descartar la más vieja
      for (int i = 1; i < N; i++) { t[i-1] = t[i]; v[i-1] = v[i]; }
      n = N - 1;
    }
    t[n] = ahora; v[n] = valor; n++;
    ultimo = ahora;
  }
};

static Serie<30> s_temp;    // ~2.5 h a 5 min → tasa de enfriamiento
static Serie<40> s_pres;    // ~6.6 h a 10 min → ΔP 6 h

// Tasa de enfriamiento (°C/h): usa la muestra más vieja dentro de las últimas 2 h.
// Positiva = enfriándose. NAN si no hay al menos 30 min de historial.
static float tasaEnfriamiento(time_t ahora) {
  float mejorEdad = -1; float tViejo = NAN;
  for (int i = 0; i < s_temp.n; i++) {
    long edad = (long)(ahora - s_temp.t[i]);
    if (edad > 0 && edad <= 7200 && edad > mejorEdad) { mejorEdad = edad; tViejo = s_temp.v[i]; }
  }
  if (isnan(tViejo) || mejorEdad < 1800) return NAN;      // <30 min: sin dato fiable
  float horas = mejorEdad / 3600.0f;
  float tNow  = s_temp.v[s_temp.n - 1];
  return (tViejo - tNow) / horas;                          // (T_inicio - T_actual)/h
}

// ΔP en 6 h (hPa): busca la muestra de ~6 h atrás (ventana 5–7 h). NAN si falta.
static float deltaPresion6h(time_t ahora, float presionActual) {
  float mejorDif = 1e9; int idx = -1;
  for (int i = 0; i < s_pres.n; i++) {
    long edad = (long)(ahora - s_pres.t[i]);
    if (edad >= 18000 && edad <= 25200) {                  // 5 h a 7 h
      float dif = fabsf(edad - 21600.0f);
      if (dif < mejorDif) { mejorDif = dif; idx = i; }
    }
  }
  if (idx < 0) return NAN;
  return presionActual - s_pres.v[idx];
}

// ---------------------------------------------------------------------------
//  Funciones puras (skill §2 y §3)
// ---------------------------------------------------------------------------
float agro_puntoRocio(float tempC, float humedadRel) {
  const float a = 17.27f, b = 237.7f;
  if (humedadRel < 1.0f) humedadRel = 1.0f;                // evita log(0)
  float gamma = (a * tempC) / (b + tempC) + logf(humedadRel / 100.0f);
  return (b * gamma) / (a - gamma);
}

float agro_depresionRocio(float tempC, float td) { return tempC - td; }

int agro_indiceRiesgo(float presion, float deltaP6h, float humedad,
                      float tasaEnf, float puntoRocio, float tempActual) {
  int score = 0;
  if (!isnan(deltaP6h) && presion > AGRO_P_ALTA_HPA && fabsf(deltaP6h) < AGRO_DP_ESTABLE_6H) score += 1;
  if (humedad < AGRO_HR_BAJA_PCT) score += 1;
  if (!isnan(tasaEnf) && tasaEnf > AGRO_TASA_ENF_FUERTE) score += 2;
  if (puntoRocio <= AGRO_TD_CRITICO) score += 2;
  if (tempActual < AGRO_T_FRIA) score += 1;
  return score;
}

// ---------------------------------------------------------------------------
//  Máquina de estados con histéresis (skill §4)
// ---------------------------------------------------------------------------
static Estado s_estado = MONITOREO_NORMAL;
static time_t s_tInicioBaja = 0;      // desde cuándo se cumple la condición de bajar
static bool   s_bajando = false;

static Estado nivelDeseado(int score, float tActual, float tMinEst) {
  if (tActual <= AGRO_T_HELADA) return HELADA_ACTIVA;
  if (score >= AGRO_SCORE_CRITICA || tActual < AGRO_T_CRITICA_ACTUAL) return ALERTA_CRITICA;
  if (score >= AGRO_SCORE_TEMPRANA || (!isnan(tMinEst) && tMinEst < AGRO_TMIN_TEMPRANA)) return ALERTA_TEMPRANA;
  return MONITOREO_NORMAL;
}

// Sube de nivel inmediato (seguridad); baja solo tras AGRO_RESET_MIN sostenido.
static void actualizarEstado(Estado deseado, time_t ahora) {
  if (deseado >= s_estado) {
    s_estado = deseado;
    s_bajando = false;
  } else {
    if (!s_bajando) { s_bajando = true; s_tInicioBaja = ahora; }
    if ((long)(ahora - s_tInicioBaja) >= (long)AGRO_RESET_MIN * 60) {
      s_estado = deseado;
      s_bajando = false;
    }
  }
}

// ---------------------------------------------------------------------------
//  API principal
// ---------------------------------------------------------------------------
void agro_init() {
  s_idxMedia = 0; s_nMedia = 0;
  s_temp.n = 0; s_temp.ultimo = 0;
  s_pres.n = 0; s_pres.ultimo = 0;
  s_estado = MONITOREO_NORMAL;
  s_bajando = false;
}

Evaluacion agro_evaluar(const Muestra& m, time_t ahora, int horaLocal) {
  Evaluacion e;

  e.tempSuavizada = mediaMovil(m.temp);
  e.puntoRocio    = agro_puntoRocio(e.tempSuavizada, m.humedad);
  e.depresionRocio = agro_depresionRocio(e.tempSuavizada, e.puntoRocio);

  // Historial temporal
  s_temp.push(ahora, e.tempSuavizada, 300);                 // ≥5 min
  if (m.presionValida) s_pres.push(ahora, m.presion, 600);  // ≥10 min

  e.tasaEnfriamiento = tasaEnfriamiento(ahora);
  e.deltaPresion6h   = m.presionValida ? deltaPresion6h(ahora, m.presion) : NAN;

  // Proyección de mínima solo en ventana nocturna (skill §2.3)
  bool esNoche = (horaLocal >= 0) &&
                 ((horaLocal >= HORA_ATARDECER) || (horaLocal < HORA_AMANECER));
  if (esNoche && !isnan(e.tasaEnfriamiento)) {
    int horasHastaAmanecer = (horaLocal < HORA_AMANECER)
                              ? (HORA_AMANECER - horaLocal)
                              : (24 - horaLocal + HORA_AMANECER);
    float tasaPos = e.tasaEnfriamiento > 0 ? e.tasaEnfriamiento : 0;
    e.tempMinEstimada = e.tempSuavizada - tasaPos * horasHastaAmanecer;
  } else {
    e.tempMinEstimada = NAN;
  }

  e.score = agro_indiceRiesgo(m.presion, e.deltaPresion6h, m.humedad,
                              e.tasaEnfriamiento, e.puntoRocio, e.tempSuavizada);

  Estado deseado = nivelDeseado(e.score, e.tempSuavizada, e.tempMinEstimada);
  actualizarEstado(deseado, ahora);
  e.estado = s_estado;
  return e;
}

uint32_t agro_intervaloMuestreo(Estado e) {
  switch (e) {
    case HELADA_ACTIVA:   return AGRO_MS_HELADA;
    case ALERTA_CRITICA:  return AGRO_MS_CRITICA;
    case ALERTA_TEMPRANA: return AGRO_MS_TEMPRANA;
    default:              return AGRO_MS_NORMAL;
  }
}

const char* agro_estadoClave(Estado e) {
  switch (e) {
    case HELADA_ACTIVA:   return "HELADA_ACTIVA";
    case ALERTA_CRITICA:  return "ALERTA_CRITICA";
    case ALERTA_TEMPRANA: return "ALERTA_TEMPRANA";
    default:              return "MONITOREO_NORMAL";
  }
}

const char* agro_estadoTexto(Estado e) {
  switch (e) {
    case HELADA_ACTIVA:   return "HELADA EN CURSO";
    case ALERTA_CRITICA:  return "RIESGO CRITICO - PRENDER RIEGO";
    case ALERTA_TEMPRANA: return "ALERTA TEMPRANA";
    default:              return "Sin riesgo";
  }
}

const char* agro_planAccion(Estado e) {
  switch (e) {
    case HELADA_ACTIVA:
      return "Helada en curso. Mantener el riego por aspersion SIN cortes hasta que "
             "el hielo se derrita y la temperatura supere +1 C de forma estable.";
    case ALERTA_CRITICA:
      return "PRENDER YA el riego por aspersion. Caudal parejo y continuo. No apagar "
             "hasta pasada la helada.";
    case ALERTA_TEMPRANA:
      return "Preparar el equipo: presurizar, revisar el motor y combustible/energia. "
             "Personal alertado. Aun no accionar.";
    default:
      return "Sin riesgo de helada. Ninguna accion requerida.";
  }
}
