# Estación Meteorológica Antiheladas — Cerezos 🍒❄️

Estación basada en **ESP32** que mide **temperatura, humedad y presión** en un campo de cerezos,
interpreta el **riesgo de helada primaveral** con criterio agronómico y **avisa con anticipación**
(por Telegram y en un dashboard web) para que el productor prenda **manualmente** el riego por
aspersión antiheladas.

> El sistema **no** acciona el motor: mide, interpreta y alerta. El encendido es manual.

## Arquitectura

```
  [DHT22]      [BMP180]
     \            /  (I2C)
      \          /
        [ ESP32 ] ──HTTPS POST──> [ Supabase / Postgres ] <──Realtime/REST── [ Dashboard (GitHub Pages) ]
           │                                                                        (celular / PC, remoto)
           └───────────────HTTPS──────────────> [ Telegram Bot ] ──push──> [ celular del productor ]
```

- **Sin servidor propio que mantener.** La ESP32 habla directo con Supabase y con Telegram.
- **Supabase** (gratis) guarda el histórico de la temporada y sirve el tiempo real.
- **Dashboard estático** (gratis en GitHub Pages) accesible en remoto.

## Estructura del repositorio

| Carpeta | Qué contiene |
|---------|--------------|
| [`firmware/`](firmware/) | Código para la ESP32 (Arduino IDE). Copiá la carpeta, poné `secrets.h` y subí. |
| [`web/`](web/) | Dashboard estático (HTML/CSS/JS) para GitHub Pages. |
| [`supabase/`](supabase/) | Scripts SQL: `schema.sql` + `policies.sql`. |
| [`docs/`](docs/) | Conexionado, instalación, nube, testeos y manual de usuario. |
| [`.claude/skills/skill-agronomica`](.claude/skills/) | **Criterio agronómico (fuente de verdad).** |

## Puesta en marcha (resumen)

1. **Hardware:** cableá según [docs/01-conexionado.md](docs/01-conexionado.md).
2. **Nube:** creá Supabase y bot de Telegram → [docs/03-configuracion-nube.md](docs/03-configuracion-nube.md).
3. **Firmware:** configurá `secrets.h` y subí el sketch → [docs/02-instalacion-firmware.md](docs/02-instalacion-firmware.md).
4. **Web:** completá `web/js/config.js` y publicá en GitHub Pages.
5. **Probá todo:** seguí [docs/04-testeos.md](docs/04-testeos.md) (incluye simulación de helada).
6. **Usalo:** [docs/05-manual-usuario.md](docs/05-manual-usuario.md).

## Componentes

- **ESP32** 38 pines (USB-C, WiFi/Bluetooth)
- **DHT22** — temperatura y humedad (GPIO 4)
- **BMP180** — presión atmosférica, I2C (GPIO 21/22)

## Lógica agronómica

Toda la inteligencia de decisión (punto de rocío, tasa de enfriamiento, proyección de mínima nocturna,
índice de riesgo compuesto y la máquina de estados) está documentada en la **skill agronómica** y se
implementa igual en el firmware (`agronomia.h/.cpp`) y se refleja en la web (`web/js/agronomia.js`).
**Si cambiás un umbral, actualizá los tres.**

## Seguridad

- `secrets.h` está en `.gitignore` (no se versiona).
- La *anon key* de Supabase es pública por diseño y está protegida por RLS; opciones para endurecer
  en `supabase/policies.sql`.
