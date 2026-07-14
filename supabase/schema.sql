-- ============================================================================
--  schema.sql  ─  Estructura de datos de la estación meteorológica (Supabase)
--
--  Ejecutar en:  Supabase Studio → SQL Editor → New query → pegar → Run.
--  Luego correr  policies.sql  para habilitar el acceso (RLS).
-- ============================================================================

-- ---------------------------------------------------------------------------
--  Tabla principal: una fila por muestreo de la ESP32
--  Contrato de datos = ver skill §6 y firmware/supabase.cpp
-- ---------------------------------------------------------------------------
create table if not exists public.readings (
  id                bigint generated always as identity primary key,
  device_id         text        not null,
  ts                timestamptz not null default now(),   -- si la ESP32 no manda ts, se usa now()
  temp_c            real,          -- temperatura suavizada (media móvil)
  humidity_pct      real,
  pressure_hpa      real,
  dew_point_c       real,          -- punto de rocío (Magnus)
  cooling_rate_cph  real,          -- tasa de enfriamiento °C/h (null si no hay dato)
  est_min_c         real,          -- mínima nocturna estimada (null fuera de la noche)
  delta_p_6h        real,          -- variación de presión en 6 h (null si falta historial)
  risk_score        int,           -- índice de riesgo compuesto (skill §3)
  status            text           -- MONITOREO_NORMAL | ALERTA_TEMPRANA | ALERTA_CRITICA | HELADA_ACTIVA
);

-- Consultas típicas: últimas lecturas de un equipo y rangos por fecha.
create index if not exists idx_readings_device_ts
  on public.readings (device_id, ts desc);

-- ---------------------------------------------------------------------------
--  Configuración por equipo (estado fenológico actual, editable)
--  El dashboard la lee para mostrar la temperatura crítica de referencia.
--  (El firmware usa constantes de compilación; esta tabla es informativa/web.)
-- ---------------------------------------------------------------------------
create table if not exists public.config (
  device_id          text primary key,
  estado_fenologico  int  not null default 5,   -- 0..5 (ver skill §2); 5 = cuajado
  actualizado        timestamptz not null default now()
);

insert into public.config (device_id, estado_fenologico)
  values ('estacion-01', 5)
  on conflict (device_id) do nothing;

-- ---------------------------------------------------------------------------
--  Vista: última lectura por equipo (cómoda para el estado actual del tablero)
-- ---------------------------------------------------------------------------
create or replace view public.latest_reading as
  select distinct on (device_id) *
  from public.readings
  order by device_id, ts desc;

-- ---------------------------------------------------------------------------
--  Realtime: publicar inserts de readings para que el dashboard se actualice solo
-- ---------------------------------------------------------------------------
alter publication supabase_realtime add table public.readings;
