// ============================================================================
//  agronomia.js  ─  ESPEJO de la skill  .claude/skills/skill-agronomica
//                   y de  firmware/estacion_meteorologica/agronomia.h
//
//  El estado (status) lo calcula y envía la ESP32; acá SOLO lo presentamos:
//  colores, texto humano y plan de acción. También la tabla fenológica para
//  mostrar la temperatura crítica de referencia.
//
//  Si cambian los umbrales/estados en la skill o el firmware, actualizar acá.
// ============================================================================

// Estados (semáforo) — mismas claves que agro_estadoClave() del firmware
const ESTADOS = {
  MONITOREO_NORMAL: {
    nivel: 0, color: "#2e7d32", label: "Sin riesgo",
    plan: "Sin riesgo de helada. Ninguna acción requerida.",
  },
  ALERTA_TEMPRANA: {
    nivel: 1, color: "#f9a825", label: "Alerta temprana",
    plan: "Preparar el equipo: presurizar, revisar el motor y combustible/energía. " +
          "Personal alertado. Aún no accionar.",
  },
  ALERTA_CRITICA: {
    nivel: 2, color: "#ef6c00", label: "Riesgo crítico — PRENDER RIEGO",
    plan: "PRENDER YA el riego por aspersión. Caudal parejo y continuo. " +
          "No apagar hasta pasada la helada.",
  },
  HELADA_ACTIVA: {
    nivel: 3, color: "#c62828", label: "Helada en curso",
    plan: "Helada en curso. Mantener el riego por aspersión SIN cortes hasta que el " +
          "hielo se derrita y la temperatura supere +1 °C de forma estable.",
  },
};

function estadoInfo(clave) {
  return ESTADOS[clave] || ESTADOS.MONITOREO_NORMAL;
}

// Temperaturas críticas del cerezo por estado fenológico (skill §2)
const FENOLOGIA = [
  { id: 0, nombre: "Dormante / yema hinchada", t10: -7.0 },
  { id: 1, nombre: "Punta verde",              t10: -5.5 },
  { id: 2, nombre: "Racimo compacto",          t10: -3.9 },
  { id: 3, nombre: "Botón blanco",             t10: -2.8 },
  { id: 4, nombre: "Plena floración",          t10: -2.2 },
  { id: 5, nombre: "Cuajado / fruto chico",    t10: -1.1 },
];

function fenologiaInfo(id) {
  return FENOLOGIA.find((f) => f.id === Number(id)) || FENOLOGIA[5];
}

// Punto de rocío (Magnus) — por si se quiere recomputar en el cliente
function puntoRocio(tempC, humedadRel) {
  const a = 17.27, b = 237.7;
  const hr = Math.max(humedadRel, 1);
  const g = (a * tempC) / (b + tempC) + Math.log(hr / 100);
  return (b * g) / (a - g);
}

window.AGRO = { ESTADOS, estadoInfo, FENOLOGIA, fenologiaInfo, puntoRocio };
