# 03 · Configuración de la nube (Supabase + Telegram + GitHub Pages)

## A. Supabase (base de datos + tiempo real)

1. Creá una cuenta en **supabase.com** y un **New project** (free tier).
   Elegí una región cercana y guardá la contraseña de la base.
2. Abrí **SQL Editor → New query**, pegá y ejecutá:
   - `supabase/schema.sql`  (crea tablas, índice, vista y habilita Realtime)
   - `supabase/policies.sql` (habilita el acceso con RLS)
3. **Settings → API**, copiá:
   - **Project URL** → `SUPABASE_URL` (en `secrets.h` y en `web/js/config.js`)
   - **anon public key** → `SUPABASE_ANON_KEY` (en ambos)

> La **anon key es pública por diseño** (va en la web y en el firmware); las políticas RLS de
> `policies.sql` limitan qué se puede hacer con ella. **Nunca** uses la *service_role* key en el
> dashboard. Para endurecer el modelo, ver el pie de `policies.sql`.

### Verificar
En **Table Editor → readings** deberían aparecer filas cuando la ESP32 empiece a publicar.

## B. Telegram (alertas push)

1. En Telegram, abrí **@BotFather** → `/newbot` → elegí nombre y usuario del bot.
   BotFather te da el **token** → `TELEGRAM_BOT_TOKEN`.
2. Obtené tu **chat id**:
   - Enviale cualquier mensaje a tu bot (o agregalo a un grupo y escribí algo).
   - Abrí en el navegador: `https://api.telegram.org/bot<TOKEN>/getUpdates`
   - Buscá `"chat":{"id":123456789,...}` → ese número es `TELEGRAM_CHAT_ID`.
     (En grupos el id suele ser negativo, p. ej. `-100...`.)
3. Poné ambos valores en `secrets.h`.

Al iniciar, la ESP32 manda un mensaje "🟢 Estación iniciada y monitoreando." Si no llega, revisá
token/chat id y que el bot haya recibido al menos un mensaje tuyo.

## C. Dashboard en GitHub Pages

1. Editá `web/js/config.js` con tu `SUPABASE_URL`, `SUPABASE_ANON_KEY`, `DEVICE_ID` y `TEMPORADA_ANIO`.
2. Subí el repo a GitHub.
3. En el repo: **Settings → Pages** → *Source: Deploy from a branch* → rama `main`, carpeta `/`
   (o `/web` si servís solo esa carpeta). Guardá.
   - Alternativa simple: publicá **solo el contenido de `web/`** en una rama `gh-pages`.
4. A los minutos tendrás una URL tipo `https://usuario.github.io/repo/web/`. Abrila en el celular.

> Como el dashboard es 100% estático, también podés abrir `web/index.html` localmente para probar
> (funciona igual porque habla directo con Supabase por HTTPS).

## Resumen de dónde va cada credencial

| Credencial | `firmware/secrets.h` | `web/js/config.js` |
|------------|:---:|:---:|
| WiFi SSID/pass | ✅ | — |
| Supabase URL | ✅ | ✅ |
| Supabase anon key | ✅ | ✅ |
| Telegram token/chat | ✅ | — |
