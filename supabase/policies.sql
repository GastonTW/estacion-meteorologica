-- ============================================================================
--  policies.sql  ─  Row Level Security (RLS) para la estación
--
--  Ejecutar DESPUÉS de schema.sql, en Supabase Studio → SQL Editor.
--
--  Modelo de seguridad (proyecto agrícola personal):
--   - La ESP32 usa la anon key para INSERTAR lecturas.
--   - El dashboard usa la anon key para LEER (select).
--   La anon key es pública por diseño; RLS limita qué puede hacer.
--   Con estas políticas, un tercero con la anon key podría insertar lecturas
--   basura o leer los datos. Para un campo propio es aceptable; si te preocupa,
--   ver la nota al pie para endurecerlo.
-- ============================================================================

-- ---- GRANTs de tabla (capa previa a RLS) ----------------------------------
--  RLS decide QUÉ FILAS puede tocar cada rol; el GRANT decide si el rol puede
--  operar sobre la tabla. Sin estos GRANT, el rol anon recibe
--  "permission denied for table ..." (código 42501) antes de evaluar RLS.
--  Algunos proyectos Supabase no otorgan estos permisos al rol anon por
--  defecto, así que los hacemos explícitos.
grant usage on schema public to anon;
grant select, insert on public.readings to anon;
grant select on public.config to anon;   -- solo lectura: el update quedó fuera (ver config)

-- ---- readings --------------------------------------------------------------
alter table public.readings enable row level security;

-- Lectura pública (dashboard)
create policy "readings_select_anon"
  on public.readings for select
  to anon
  using (true);

-- Inserción desde la ESP32 (anon key)
create policy "readings_insert_anon"
  on public.readings for insert
  to anon
  with check (true);

-- ---- config ----------------------------------------------------------------
alter table public.config enable row level security;

create policy "config_select_anon"
  on public.config for select
  to anon
  using (true);

-- NOTA: se quitó la política de UPDATE anon sobre config (endurecimiento).
--   El repo es público y la anon key también, así que permitir UPDATE dejaba
--   que cualquiera cambiara el estado fenológico. Ahora ese valor solo se
--   modifica desde Supabase Studio. Si algún día querés volver a editarlo desde
--   el dashboard, recreá la política de update + el grant, o mejor pasá a un
--   flujo autenticado (ver "ENDURECER" al pie).

-- ============================================================================
--  ENDURECER (opcional, recomendado si el proyecto crece):
--   1) Crear una tabla de dispositivos con un token secreto por equipo.
--   2) Reemplazar el insert directo por una función RPC `security definer`
--      que valide el token antes de insertar, y quitar la política de insert anon.
--   3) O usar la service_role key SOLO en el firmware (no en la web) y quitar
--      la política de insert anon. La service_role NO debe ir nunca en el dashboard.
-- ============================================================================
