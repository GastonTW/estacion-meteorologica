# CLAUDE.md — Guía para trabajar en este repo

Estación meteorológica antiheladas para cerezos: ESP32 + DHT22 + BMP180 → Supabase → dashboard web
+ alertas Telegram. El riego por aspersión se prende **manualmente**; el sistema solo mide, interpreta
y alerta.

## Regla de oro: una sola fuente de verdad agronómica

La lógica de dominio (umbrales, fórmulas, estados, tasa de muestreo) vive en:

- **`.claude/skills/skill-agronomica`** ← FUENTE DE VERDAD

y está replicada en:

- `firmware/estacion_meteorologica/agronomia.h` y `agronomia.cpp`
- `web/js/agronomia.js`

**Si cambiás un umbral, fórmula o estado, actualizá los tres lugares** y validá con la simulación de
`docs/04-testeos.md §6`.

## Mapa del repo

- `firmware/estacion_meteorologica/` — sketch Arduino IDE modular:
  - `estacion_meteorologica.ino` (orquesta), `agronomia.*` (decisión), `sensores.*`,
    `conectividad.*` (WiFi/NTP), `supabase.*`, `telegram.*`
  - `config.h` (editable), `secrets.h` (gitignored; copiar de `secrets.example.h`)
- `web/` — dashboard estático (Supabase JS + Chart.js por CDN) para GitHub Pages
- `supabase/` — `schema.sql` (tablas/vista/realtime) + `policies.sql` (RLS)
- `docs/` — 01 conexionado · 02 firmware · 03 nube · 04 testeos · 05 manual

## Contrato de datos (tabla `readings`)

`device_id, ts, temp_c, humidity_pct, pressure_hpa, dew_point_c, cooling_rate_cph, est_min_c,
delta_p_6h, risk_score, status`. El `status` es una de las 4 claves de estado
(`MONITOREO_NORMAL | ALERTA_TEMPRANA | ALERTA_CRITICA | HELADA_ACTIVA`).

## Convenciones

- Pines en `config.h` (DHT22 → GPIO 4; BMP180 I2C → GPIO 21/22; sensores a 3V3).
- No compilar en este entorno (no hay toolchain Arduino); la verificación es la de `docs/04-testeos.md`.
- No subir `secrets.h`. La anon key de Supabase sí es pública (protegida por RLS).
