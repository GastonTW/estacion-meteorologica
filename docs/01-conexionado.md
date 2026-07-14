# 01 · Conexionado (hardware)

Estación con **ESP32 (38 pines)** + **DHT22** (temp/humedad) + **BMP180** (presión, I2C).

## Pinout usado

| Señal            | Sensor  | Pin ESP32 | Notas |
|------------------|---------|-----------|-------|
| Alimentación 3V3 | DHT22 + BMP180 | `3V3` | Ambos a **3.3 V** |
| GND              | DHT22 + BMP180 | `GND` | Masa común |
| Dato DHT22       | DHT22   | **GPIO 4** | Requiere pull-up (ver abajo) |
| SDA (I2C)        | BMP180  | **GPIO 21** | I2C por defecto |
| SCL (I2C)        | BMP180  | **GPIO 22** | I2C por defecto |

> ⚠️ **El BMP180 es de 3.3 V.** No lo alimentes ni le mandes lógica de 5 V al bus I2C.
> El DHT22 funciona 3.3–5.5 V; lo dejamos en 3.3 V para compartir niveles con el BMP180.

Si cambiás algún pin, actualizá `firmware/estacion_meteorologica/config.h`.

## Diagrama

```
                ESP32 (38 pines)
              ┌─────────────────┐
   DHT22      │                 │
  ┌──────┐    │                 │      BMP180 (GY-68)
  │ VCC  ├────┤ 3V3        3V3 ├────┤ VCC  │
  │ DATA ├──┬─┤ GPIO4      GND ├────┤ GND  │
  │ GND  ├──│─┤ GND       GPIO21├───┤ SDA  │
  └──────┘  │ │           GPIO22├───┤ SCL  │
            │ │                 │   └──────┘
         [10kΩ]│                │
            └─┤ 3V3  (pull-up DATA→3V3)
              └─────────────────┘
```

## Pull-up del DHT22

- Muchos **módulos DHT22 "con cables/placa"** ya traen el resistor de pull-up incorporado
  (suelen tener 3 pines rotulados VCC/DATA/GND o +/out/−). En ese caso **no agregues** nada.
- Si usás el **sensor pelado de 4 patas**, colocá un **resistor de 10 kΩ** entre `DATA` y `3V3`.
- ¿Duda? Si las lecturas salen `NAN` de forma constante, casi siempre falta el pull-up.

## Alimentación y montaje en campo

- Alimentar la ESP32 por **USB-C** desde una fuente 5 V estable o power bank/panel solar con regulador.
- **Ubicá el DHT22 a la altura de brotes/fruta**, no a 2 m: la helada se define en el microclima del
  cultivo (skill §5). Protegido de lluvia y sol directo (abrigo meteorológico / pantalla ventilada).
- Cables de sensor cortos (< 1 m) para I2C confiable; si necesitás más largo, bajá la velocidad I2C
  o usá cable apantallado.
- Verificá **buena señal WiFi** en el punto de montaje (requisito del proyecto).

## Próximo sensor recomendado

Un **anemómetro**: el viento es la variable que más cambia el riesgo de helada radiativa (skill §5).
Es la mejor ampliación futura del sistema.
