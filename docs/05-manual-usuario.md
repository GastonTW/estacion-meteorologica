# 05 · Manual de usuario

Guía para el uso diario del sistema durante la temporada de heladas (agosto–octubre).

## Qué hace (y qué NO hace) el sistema

- **Mide** temperatura, humedad y presión en el cultivo.
- **Interpreta** el riesgo de helada con criterio agronómico (no solo mira el termómetro).
- **Avisa** por Telegram y en el dashboard con **anticipación**.
- ❌ **No prende el riego.** El encendido del motor de aspersión es **manual**: el sistema te dice
  *cuándo* y *por qué*, vos accionás.

## Los 4 estados (semáforo)

| Estado | Color | Qué significa | Qué hacer |
|--------|:-----:|---------------|-----------|
| **Sin riesgo** | 🟢 | Noche/día tranquilos | Nada. |
| **Alerta temprana** | 🟡 | El modelo proyecta que la mínima puede acercarse a valores de riesgo | **Prepararte**: revisar el motor, combustible/energía, presurizar el sistema. No accionar todavía. |
| **Riesgo crítico — PRENDER RIEGO** | 🟠 | Las condiciones para dañar el cultivo están muy cerca | **Prender YA** el riego por aspersión, caudal parejo y continuo. |
| **Helada en curso** | 🔴 | Está helando | **Mantener el riego SIN cortes** hasta que el hielo se derrita y la temperatura supere +1 °C de forma estable. Recién ahí apagar. |

> **Por qué se prende *antes* de los 0 °C:** al asperjar, parte del agua se evapora y enfría todavía
> más; además el riego protege liberando calor al congelarse sobre la planta. Por eso el sistema avisa
> con margen y conviene arrancar apenas aparece el estado naranja. (Detalle en la skill agronómica.)

## El dashboard

- **Banner grande de color**: el estado actual y el plan de acción en palabras.
- **Tiles**: temperatura, humedad, presión, punto de rocío, **mínima estimada** y **score de riesgo**.
  - *Mínima estimada*: a dónde podría bajar la temperatura al amanecer según cómo viene enfriando.
  - *Score de riesgo* (0–7): combina presión estable, humedad, velocidad de enfriamiento, rocío y
    temperatura. Cuanto más alto, más probable la helada radiativa.
- **Punto de conexión** (arriba a la izquierda): verde = la estación reporta; rojo = sin datos recientes.
- **Histórico**: botones 24 h / 7 días / Agosto / Septiembre / Octubre para ver la evolución.
- **Estado fenológico**: elegí en qué etapa está el cerezo; ajusta la temperatura crítica de referencia.
  Actualizalo a medida que avanza la temporada (yema → floración → cuajado); cuanto más avanzado,
  más sensible es el cultivo.

## Alertas por Telegram

- Llega un mensaje **cada vez que cambia el nivel** (no en cada medición), con los valores del momento
  y el plan de acción. Ideal para la madrugada.
- Tené el celular con sonido durante las noches de riesgo (estado 🟡/🟠).

## Rutina sugerida en noche de riesgo

1. Al atardecer, si ves 🟡 en el dashboard o llega alerta temprana → **preparar el equipo**.
2. Si sube a 🟠 → **prender el riego**.
3. Mientras esté 🟠/🔴 → **no cortar el agua**.
4. Cuando la temperatura se recupere sobre +1 °C estable y el estado vuelva a 🟢 → apagar.

## Mantenimiento

- Revisá que el sensor esté limpio, seco y a la altura del cultivo.
- Controlá la alimentación (fuente/solar) antes de cada temporada.
- Al inicio de temporada, corré la **simulación de alerta** (doc 04 §6) para confirmar que las
  notificaciones llegan.
