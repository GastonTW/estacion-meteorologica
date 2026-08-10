-- ============================================================================
--  policies.sql  ─  Row Level Security (RLS) para la estación
--
--  Ejecutar DESPUÉS de schema.sql, en Supabase Studio → SQL Editor.
--
--  Modelo de seguridad (proyecto agrícola personal):
--   - La ESP32 usa la anon key para INSERTAR lecturas.
--   - El dashboard usa la anon key para LEER (select) y para EDITAR el estado
--     fenológico en `config` (update).
--   La anon key es pública por diseño; RLS limita qué puede hacer.
--   Con estas políticas, un tercero con la anon key podría insertar lecturas
--   basura, leer los datos o cambiar el estado fenológico. Para un campo propio
--   es aceptable; si te preocupa, ver la nota al pie para endurecerlo.
-- ============================================================================

-- ---- GRANTs de tabla (capa previa a RLS) ----------------------------------
--  RLS decide QUÉ FILAS puede tocar cada rol; el GRANT decide si el rol puede
--  operar sobre la tabla. Sin estos GRANT, el rol anon recibe
--  "permission denied for table ..." (código 42501) antes de evaluar RLS.
--  Algunos proyectos Supabase no otorgan estos permisos al rol anon por
--  defecto, así que los hacemos explícitos.
grant usage on schema public to anon;
grant select, insert on public.readings to anon;
grant select, update on public.config to anon;   -- el dashboard edita el estado fenológico

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

-- Edición del estado fenológico desde el dashboard.
--   Decisión tomada a conciencia: el proyecto es privado y el único que lo
--   cambia es el encargado del campo, así que no se pide login. Cualquiera que
--   conozca la URL del dashboard puede editar este valor. Si el día de mañana
--   la URL se comparte, pasá a un flujo autenticado (ver "ENDURECER" al pie).
--   El `with check` acota el daño de un valor inválido: la escala fenológica
--   es 0..5 (skill §2) y fuera de ese rango la T crítica no tendría sentido.
create policy "config_update_anon"
  on public.config for update
  to anon
  using (true)
  with check (estado_fenologico between 0 and 5);

-- ============================================================================
--  ENDURECER (opcional, recomendado si el proyecto crece):
--   1) Crear una tabla de dispositivos con un token secreto por equipo.
--   2) Reemplazar el insert directo por una función RPC `security definer`
--      que valide el token antes de insertar, y quitar la política de insert anon.
--   3) O usar la service_role key SOLO en el firmware (no en la web) y quitar
--      la política de insert anon. La service_role NO debe ir nunca en el dashboard.
-- ============================================================================
