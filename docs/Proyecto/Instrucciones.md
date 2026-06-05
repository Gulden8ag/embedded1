# Capstone Micromouse

El proyecto **Micromouse** consiste en diseñar y programar un robot móvil autónomo capaz de **explorar** un laberinto, **construir un mapa** y ejecutar una **carrera rápida (fast run)** desde el inicio hasta el objetivo en el centro. 

<iframe width="560" height="315" src="https://www.youtube.com/embed/IngelKjmecg?si=9EK3V4aegkhExXMP" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>


<iframe width="560" height="315" src="https://www.youtube.com/embed/kMOssi5IcP0?si=zZLUoDwHL0lH90OW" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

## Alcance y entregables

**Entregables obligatorios:**
1. **Robot funcional** con binario **MODO_ARENA**.
2. **Bitácora técnica final** (Pagina): arquitectura, pruebas clave, métricas, decisiones y lecciones.
3. **Código fuente** etiquetado (`tag` de release), **binario** final y **mapa de pines/clocks**.
4. **Acta de resultados** del evento.

## Especificaciones del laberinto

- **Tamaño:** 16×16 celdas, objetivo en un **cuadro 2×2** al centro.
- **Celdas:** ~15×15 cm (típico), paredes ~5 cm de alto (pueden variar según arena disponible).
- **Superficie:** Nivelada, con suficiente fricción para tracción.
- **Meta:** Se considera alcanzada cuando el centro geométrico del robot entra a la zona objetivo.

## Reglas del robot

- **Autonomía:** No se permite control humano una vez iniciado el intento.
- **Dimensiones:** Máx. **12×12×12 cm**. Peso libre.
- **Seguridad:** Sin elementos punzantes, sin proyecciones, sin líquidos o humo.
- **Kill switch:** Obligatorio, accesible sin levantar el robot.
- **Integridad:** Cableado firme; batería protegida; sin piezas sueltas.

## Formato de competencia

- **Intentos:** Hasta **3 intentos** por equipo (tiempo mediante, puede ajustarse en briefing).
- **Estructura de intento:**  
  1) **Exploración** (el robot construye o actualiza el mapa)  
  2) **Fast run** (carrera óptima hacia el objetivo)  

**Acciones que invalidan un intento:**
- Levantar el robot, o **asistencia manual** durante la carrera (excepto por seguridad).
- Salida deliberada de pista o daño al laberinto.

## Cronograma

Tiempo en minutos

- **00:00–00:10** Registro e **inspección de seguridad**.
- **00:10–00:15** Briefing (reglas, orden, señales).
- **00:15–01:30** Clasificatorios.
- **01:30–01:50** Final.
- **01:50–02:00** Premiación y firma de actas.


## Inspección técnica (checklist)
- [ ] Dimensiones dentro de 12×12×12 cm.  
- [ ] **Kill switch** accesible y probado.  
- [ ] Batería y cables asegurados; sin bordes peligrosos.  
- [ ] **MODO_ARENA** cargado (sin logs verbosos).  
- [ ] CLI y telemetría **solo** para setup.   
- [ ] Sin control remoto durante la carrera.

## Puntuación, bonos y penalizaciones
**Tiempo base (Tc):** mejor **fast run** del equipo (en segundos).

**Bonos:**
- **Exploración única** (sin reset entre explorar y fast run): **−5 s**
- **Cero colisiones** (validado por jueces): **−3 s**

**Penalizaciones:**
- **Colisión** clara (detiene o altera trayectoria): **+2 s** c/u (máx. **+10 s**)
- **Reinicio manual** durante fast run: **+5 s**
- **Levantamiento del robot / salida de pista**: intento inválido

**Puntuación final:**

P = Tc + Bonos - Penalizaciones

## Criterios de desempate

1. Menor **fast run** (sin bonos/penalizaciones).  
2. Menor tiempo de **exploración**.  
3. Persistiendo empate: sorteo supervisado (coin toss).

## Documentación y entrega final
- **Bitácora final (Pagina)** con:  
  - Arquitectura (diagrama), pinout, clocks.  
  - Pruebas clave (PIO/DMA/UART/WDT/energia/EMC/multicore).  
  - Métricas: tiempos, jitter, %CPU, pérdidas FIFO/DMA, consumo.  
  - Decisiones y **rationale** (por qué cada prueba).  
  - Lecciones aprendidas.
  - Diseño de PCB.
- **Código y binario** .

## Calificacion

- **Desempeño en competencia (ranking, P)** — 40%  
    - **Primer lugar** — 40%
    - **Segundo lugar** — 36%
    - **Tercer lugar** — 32%
    - **Cuarto lugar** — 28%
    - **Quinto lugar** — 24%
    - **Sexto lugar** — 20%
    - **Septimo lugar** — 16%
    - **Octavo lugar** — 12%
- **Ingeniería y robustez del mouse** — 30%  
    - PCB
    - Programación
    - Pruebas
- **Requisitos técnicos** — 20%
- **Bitácora técnica** — 10%