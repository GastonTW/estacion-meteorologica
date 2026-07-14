# 04 · Plan de testeos

Pruebas por etapas, de la más simple a la integral. Hacelas en orden: si una falla, no sigas.

## 1. Sensores (sanity por Serial)

- Subí el firmware y abrí el Monitor Serie a **115200**.
- En cada muestra debe verse una línea `[Muestra] T=.. HR=.. P=.. ...`.
- **Chequeos:**
  - `T` y `HR` con valores plausibles del ambiente (no `nan`).
  - `P` cerca de **950–1030 hPa** (según altitud). Si `P=nan` → BMP180 no responde:
    revisá SDA/SCL (GPIO 21/22), alimentación 3V3 y soldaduras.
  - `HR` constante en `nan` → falta el **pull-up** del DHT22 (ver 01-conexionado).

## 2. WiFi

- En el Serial debe aparecer `[WiFi] OK  IP: ...`.
- Si dice `FALLO de conexion`: SSID/clave en `secrets.h`, señal en el punto de montaje,
  y que la red sea **2.4 GHz** (la ESP32 no usa 5 GHz).

## 3. Hora (NTP)

- Debe decir `[NTP] Sincronizando... OK`.
- Sin hora válida, la ESP32 igual publica (la tabla completa `ts` con `now()`), pero la **mínima
  estimada** no se calcula. Verificá salida a internet.

## 4. Supabase (ingreso de datos)

- En el Serial: `[Supabase] OK (201)` en cada muestra.
- En **Supabase → Table Editor → readings**: aparecen filas nuevas con `device_id`, `temp_c`, etc.
- Error `401/403` → anon key o políticas RLS mal (re-ejecutá `policies.sql`).
- Error `404` → `SUPABASE_URL` mal escrita o falta correr `schema.sql`.

## 5. Telegram

- Al bootear llega "🟢 Estación … iniciada y monitoreando." al chat configurado.
- Si no llega: token/chat id, y que le hayas mandado al menos un mensaje al bot (paso B de doc 03).

## 6. Simulación de alerta de helada (sin esperar el frío)

Para validar la máquina de estados y el push **sin** una helada real:

1. En `config.h` poné `#define MODO_TEST 1` y `#define TEST_TEMP_C -1.0f`.
2. Recompilá y subí. El firmware **inyecta -1.0 °C** en cada lectura.
3. Esperado (en pocos muestreos):
   - Serial: el estado escala `... -> ALERTA_CRITICA` / `-> HELADA_ACTIVA`.
   - **Telegram**: llega la alerta con el estado y el plan de acción ("PRENDER YA el riego…").
   - **Dashboard**: el banner se pone naranja/rojo con el texto correspondiente.
4. Cambiá `TEST_TEMP_C` a distintos valores para ver los umbrales:
   - `4.0` → `ALERTA_TEMPRANA`  ·  `1.5` → `ALERTA_CRITICA`  ·  `-1.0` → `HELADA_ACTIVA`.
5. **Volvé a `#define MODO_TEST 0`** y recompilá para producción.

> Nota: la histéresis mantiene el nivel alto ~30 min antes de bajar (seguridad, skill §4). Es esperado.

## 7. Dashboard

- Abrí la URL de GitHub Pages en el celular.
- **Tiempo real:** al llegar una muestra nueva, los tiles y el banner se actualizan solos y el
  puntito de conexión queda verde (online).
- **Histórico:** probá los botones 24 h / 7 días / Agosto-Sep-Oct; deben dibujarse las 3 gráficas.
- **Fenología:** cambiá el selector; la "T crítica ref." cambia y (si la política lo permite) se guarda.

## Checklist final

- [ ] Sensores dan valores reales
- [ ] WiFi + NTP OK
- [ ] Filas nuevas en `readings`
- [ ] Mensaje de arranque en Telegram
- [ ] Alerta simulada dispara push + banner
- [ ] Dashboard actualiza en vivo y grafica histórico
- [ ] `MODO_TEST` de nuevo en `0`
