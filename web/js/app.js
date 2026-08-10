// ============================================================================
//  app.js  ─  Render del dashboard (tiles, banner de estado, gráficas)
// ============================================================================
// estadoInfo, FENOLOGIA y fenologiaInfo ya son globales (definidas en agronomia.js,
// que carga antes). No re-declararlas acá: dispara "Identifier already declared".
const cfg = window.APP_CONFIG;

const $ = (id) => document.getElementById(id);
const fmt = (v, d = 1) => (v === null || v === undefined || Number.isNaN(v) ? "—" : Number(v).toFixed(d));

// ---------------------------------------------------------------------------
//  Estado actual (banner + tiles)
// ---------------------------------------------------------------------------
function renderActual(r) {
  if (!r) return;
  const info = estadoInfo(r.status);

  const banner = $("status-banner");
  banner.style.background = info.color;
  $("status-word").textContent = info.label;
  $("status-plan").textContent = info.plan;

  $("v-temp").textContent = fmt(r.temp_c);
  $("v-hum").textContent = fmt(r.humidity_pct, 0);
  $("v-pres").textContent = fmt(r.pressure_hpa, 0);
  $("v-dew").textContent = fmt(r.dew_point_c);
  $("v-min").textContent = fmt(r.est_min_c);
  $("v-score").textContent = r.risk_score ?? "—";

  const ts = new Date(r.ts);
  $("last-update").textContent = "Últ.: " + ts.toLocaleString("es-AR", { hour12: false });
  $("device-label").textContent = cfg.DEVICE_ID;

  // Conexión: online si la última lectura es reciente (< 15 min)
  const online = Date.now() - ts.getTime() < 15 * 60 * 1000;
  const dot = $("conn-dot");
  dot.classList.toggle("online", online);
  dot.classList.toggle("offline", !online);
}

// ---------------------------------------------------------------------------
//  Estado fenológico
// ---------------------------------------------------------------------------
function poblarFenologia(seleccion) {
  const sel = $("feno-select");
  sel.innerHTML = "";
  FENOLOGIA.forEach((f) => {
    const opt = document.createElement("option");
    opt.value = f.id;
    opt.textContent = `${f.nombre} (T10 ${f.t10} °C)`;
    if (f.id === seleccion) opt.selected = true;
    sel.appendChild(opt);
  });
  $("feno-crit").textContent = fenologiaInfo(seleccion).t10;
}

// El selector es editable: la policy de UPDATE anon sobre `config` está
// habilitada a propósito (proyecto privado, lo cambia el encargado del campo).
// Se llama una sola vez, después de poblarFenologia, para no apilar listeners.
function wireFenologia() {
  const sel = $("feno-select");
  const msg = $("feno-msg");
  let guardado = sel.value;   // último valor confirmado en la base

  const aviso = (texto, clase) => {
    msg.textContent = texto;
    msg.className = "feno-msg" + (clase ? " " + clase : "");
  };

  sel.addEventListener("change", async () => {
    const id = Number(sel.value);
    $("feno-crit").textContent = fenologiaInfo(id).t10;   // feedback inmediato
    sel.disabled = true;
    aviso("Guardando…");

    try {
      await DB.setFenologia(id);
      guardado = sel.value;
      aviso("✔ Guardado", "ok");
      setTimeout(() => { if (msg.textContent === "✔ Guardado") aviso(""); }, 3000);
    } catch (e) {
      console.error(e);
      // Revierte a lo que sí está en la base para no mostrar una T crítica falsa.
      sel.value = guardado;
      $("feno-crit").textContent = fenologiaInfo(Number(guardado)).t10;
      aviso("No se pudo guardar", "err");
    } finally {
      sel.disabled = false;
    }
  });
}

// ---------------------------------------------------------------------------
//  Gráficas
// ---------------------------------------------------------------------------
let charts = {};
const COL = { temp: "#ef6c00", dew: "#42a5f5", min: "#c62828", hum: "#26a69a", pres: "#ab47bc" };

function baseOpts(titulo) {
  return {
    responsive: true, maintainAspectRatio: false,
    interaction: { mode: "index", intersect: false },
    plugins: {
      legend: { labels: { color: "#9fb0c0", boxWidth: 12 } },
      title: { display: true, text: titulo, color: "#e8edf2" },
    },
    scales: {
      x: { ticks: { color: "#9fb0c0", maxTicksLimit: 8, autoSkip: true }, grid: { color: "rgba(255,255,255,0.05)" } },
      y: { ticks: { color: "#9fb0c0" }, grid: { color: "rgba(255,255,255,0.06)" } },
    },
  };
}

function thin(rows, max = 800) {
  if (rows.length <= max) return rows;
  const step = Math.ceil(rows.length / max);
  return rows.filter((_, i) => i % step === 0);
}

function etiqueta(ts, largo) {
  const d = new Date(ts);
  return largo
    ? d.toLocaleString("es-AR", { day: "2-digit", month: "2-digit", hour: "2-digit", minute: "2-digit", hour12: false })
    : d.toLocaleTimeString("es-AR", { hour: "2-digit", minute: "2-digit", hour12: false });
}

function dibujar(id, titulo, labels, datasets) {
  if (charts[id]) charts[id].destroy();
  charts[id] = new Chart($(id), {
    type: "line",
    data: { labels, datasets },
    options: baseOpts(titulo),
  });
}

function renderHistorico(rows, spanLargo) {
  const empty = $("hist-empty");
  if (!rows.length) {
    empty.hidden = false;
    ["chart-temp", "chart-hum", "chart-pres"].forEach((id) => charts[id] && charts[id].destroy());
    charts = {};
    return;
  }
  empty.hidden = true;
  rows = thin(rows);
  const labels = rows.map((r) => etiqueta(r.ts, spanLargo));
  const linea = (data, color, label) => ({
    label, data, borderColor: color, backgroundColor: color,
    borderWidth: 1.6, pointRadius: 0, tension: 0.25, spanGaps: true,
  });

  dibujar("chart-temp", "Temperatura / rocío / mínima estimada (°C)", labels, [
    linea(rows.map((r) => r.temp_c), COL.temp, "Temp"),
    linea(rows.map((r) => r.dew_point_c), COL.dew, "Rocío"),
    linea(rows.map((r) => r.est_min_c), COL.min, "Mín. estimada"),
  ]);
  dibujar("chart-hum", "Humedad relativa (%)", labels, [
    linea(rows.map((r) => r.humidity_pct), COL.hum, "Humedad"),
  ]);
  dibujar("chart-pres", "Presión atmosférica (hPa)", labels, [
    linea(rows.map((r) => r.pressure_hpa), COL.pres, "Presión"),
  ]);
}

// ---------------------------------------------------------------------------
//  Rangos de fecha
// ---------------------------------------------------------------------------
function rango(clave) {
  const anio = cfg.TEMPORADA_ANIO;
  const ahora = new Date();
  const mes = (m) => ({
    desde: new Date(anio, m, 1).toISOString(),
    hasta: new Date(anio, m + 1, 1).toISOString(),
    largo: true,
  });
  switch (clave) {
    case "7d":  return { desde: new Date(ahora - 7 * 864e5).toISOString(), hasta: ahora.toISOString(), largo: true };
    case "ago": return mes(7);   // agosto (0-index)
    case "sep": return mes(8);
    case "oct": return mes(9);
    case "24h":
    default:    return { desde: new Date(ahora - 864e5).toISOString(), hasta: ahora.toISOString(), largo: false };
  }
}

async function cargarHistorico(clave) {
  const r = rango(clave);
  try {
    const rows = await DB.historico(r.desde, r.hasta);
    renderHistorico(rows, r.largo);
  } catch (e) {
    console.error("Error histórico:", e.message);
    $("hist-empty").textContent = "Error al cargar el histórico.";
    $("hist-empty").hidden = false;
  }
}

function wireRangos() {
  const cont = $("range-btns");
  cont.addEventListener("click", (ev) => {
    const btn = ev.target.closest("button");
    if (!btn) return;
    cont.querySelectorAll("button").forEach((b) => b.classList.remove("active"));
    btn.classList.add("active");
    cargarHistorico(btn.dataset.range);
  });
}

// ---------------------------------------------------------------------------
//  Init
// ---------------------------------------------------------------------------
async function init() {
  wireRangos();

  // Fenología (default 5 si falla la config)
  let feno = 5;
  try { feno = await DB.getFenologia(); } catch (e) { console.warn("Config no disponible:", e.message); }
  poblarFenologia(feno);
  wireFenologia();

  // Estado actual
  try { renderActual(await DB.ultima()); }
  catch (e) {
    console.error(e);
    $("status-word").textContent = "Sin conexión";
    $("status-plan").textContent = "No se pudo leer la estación. Revisá config.js y la conexión.";
  }

  // Histórico inicial (24 h)
  cargarHistorico("24h");

  // Tiempo real
  DB.suscribir((nueva) => {
    renderActual(nueva);
    const activo = document.querySelector("#range-btns button.active");
    if (activo && activo.dataset.range === "24h") cargarHistorico("24h");
  });
}

window.addEventListener("DOMContentLoaded", init);
