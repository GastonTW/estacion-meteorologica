# 02 · Instalación del firmware (Arduino IDE)

Todo lo necesario para subir el código a la ESP32 desde la carpeta
`firmware/estacion_meteorologica/`.

## 1. Instalar Arduino IDE + soporte ESP32

1. Descargá e instalá **Arduino IDE 2.x** (arduino.cc).
2. `Archivo → Preferencias → URLs adicionales de gestor de placas`, agregá:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. `Herramientas → Placa → Gestor de placas…` → buscá **esp32** (Espressif) → **Instalar**.

## 2. Instalar librerías

`Herramientas → Administrar librerías…` e instalá:

| Librería | Autor | Para |
|----------|-------|------|
| **DHT sensor library** | Adafruit | DHT22 |
| **Adafruit Unified Sensor** | Adafruit | dependencia del DHT |
| **Adafruit BMP085 Library** | Adafruit | BMP180 (compatible) |
| **ArduinoJson** | Benoit Blanchon | payloads JSON (usar 6.x o 7.x) |

WiFi, HTTPClient, WiFiClientSecure y el reloj (NTP) vienen con el core ESP32.

## 3. Configurar credenciales

1. En `firmware/estacion_meteorologica/`, copiá `secrets.example.h` como **`secrets.h`**.
2. Completá WiFi, Supabase (URL + anon key) y Telegram (token + chat id).
   → Cómo obtener cada uno: **[03-configuracion-nube.md](03-configuracion-nube.md)**.
3. Revisá `config.h` (pines, `DEVICE_ID`, zona horaria, horas de la ventana nocturna).

> `secrets.h` está en `.gitignore` y **no se sube al repo**.

## 4. Abrir y subir el sketch

1. Abrí `firmware/estacion_meteorologica/estacion_meteorologica.ino`
   (Arduino IDE carga automáticamente los `.h/.cpp` de la carpeta).
2. `Herramientas → Placa` → **ESP32 Dev Module**.
3. Conectá la ESP32 por USB-C y elegí el **Puerto** correspondiente.
4. Config. recomendada (suele venir por defecto): Upload Speed `921600`,
   Flash Size `4MB`, Partition Scheme `Default`.
5. **Subir** (→). Si no entra en modo carga, mantené `BOOT` presionado al iniciar el upload.

## 5. Verificar por Serial

`Herramientas → Monitor Serie` a **115200 baudios**. Deberías ver:

```
=== Estacion Meteorologica Antiheladas ===
[WiFi] Conectando....
[WiFi] OK  IP: 192.168.x.x
[NTP] Sincronizando... OK
[Muestra] T=14.2C HR=61% P=1017.3hPa Td=6.8C tasa=nanC/h min~=nanC score=1 -> MONITOREO_NORMAL
[Supabase] OK (201)
```

Si algo falla, seguí **[04-testeos.md](04-testeos.md)**.

## Notas

- **`tasa` y `min~` en `nan` al principio es normal:** la tasa de enfriamiento necesita ~30 min de
  historial y la mínima estimada solo se calcula de noche (skill §2.3).
- Driver USB: si no aparece el puerto, instalá el driver **CP2102** o **CH340** según tu placa.
