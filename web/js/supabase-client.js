// ============================================================================
//  supabase-client.js  ─  Acceso a datos (tiempo real + histórico)
//  Requiere supabase-js v2 cargado por CDN (ver index.html).
// ============================================================================
const _cfg = window.APP_CONFIG;
const _sb = window.supabase.createClient(_cfg.SUPABASE_URL, _cfg.SUPABASE_ANON_KEY);

const DB = {
  // Última lectura del equipo
  async ultima() {
    const { data, error } = await _sb
      .from("readings")
      .select("*")
      .eq("device_id", _cfg.DEVICE_ID)
      .order("ts", { ascending: false })
      .limit(1);
    if (error) throw error;
    return data && data[0] ? data[0] : null;
  },

  // Histórico entre dos fechas ISO (hasta `limite` filas)
  async historico(desdeISO, hastaISO, limite = 5000) {
    const { data, error } = await _sb
      .from("readings")
      .select("ts,temp_c,humidity_pct,pressure_hpa,dew_point_c,est_min_c,status")
      .eq("device_id", _cfg.DEVICE_ID)
      .gte("ts", desdeISO)
      .lte("ts", hastaISO)
      .order("ts", { ascending: true })
      .limit(limite);
    if (error) throw error;
    return data || [];
  },

  // Suscripción a nuevas lecturas (Realtime)
  suscribir(cb) {
    return _sb
      .channel("readings-live")
      .on(
        "postgres_changes",
        { event: "INSERT", schema: "public", table: "readings", filter: `device_id=eq.${_cfg.DEVICE_ID}` },
        (payload) => cb(payload.new)
      )
      .subscribe();
  },

  // Estado fenológico configurado
  async getFenologia() {
    const { data, error } = await _sb
      .from("config")
      .select("estado_fenologico")
      .eq("device_id", _cfg.DEVICE_ID)
      .limit(1);
    if (error) throw error;
    return data && data[0] ? data[0].estado_fenologico : 5;
  },

  async setFenologia(id) {
    const { error } = await _sb
      .from("config")
      .update({ estado_fenologico: id, actualizado: new Date().toISOString() })
      .eq("device_id", _cfg.DEVICE_ID);
    if (error) throw error;
  },
};

window.DB = DB;
