```mermaid
stateDiagram-v2
    [*] --> MENU : Iniciar programa

    state MENU {
        [*] --> EsperandoInicio
        EsperandoInicio --> EsperandoInicio : Renderizar UI y procesar Clics
    }

    state PLAYING {
        [*] --> BucleJuego
        BucleJuego --> BucleJuego : Actualizar posiciones
        BucleJuego --> BucleJuego : Teletransporte en laterales
    }

    state GAME_OVER {
        [*] --> EsperandoRespuesta
        EsperandoRespuesta --> EsperandoRespuesta : Renderizar UI Game Over
    }

    MENU --> PLAYING : Evento: Clic en "INICIAR JUEGO"
    PLAYING --> GAME_OVER : Condición: Colisión Pacman y Fantasma
    GAME_OVER --> PLAYING : Evento: Clic en "VOLVER A JUGAR" (Reset)
    GAME_OVER --> [*] : Evento: Clic en "CERRAR PROGRAMA" (Exit)