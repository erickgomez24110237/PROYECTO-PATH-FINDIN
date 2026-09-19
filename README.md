ESTE ES UN PROYECTO ORIGINAL DE ERICK RAFAEL GOMEZ MACIEL 24110237  6E


ESCRIBIR LO SIGUIENTE EN LA TERMINAL: "make runPathFinding" PARA PODER EJECUTAR EL CODIGO.

Imagínate que el fantasma es un perrito buscando su hueso (que es Pac-Man). El perrito no ve todo el camino completo, pero es muy inteligente y hace lo siguiente para no perderse:

Mira las pistas de los pasos (Valor G): Cuenta cuántos pasos ha caminado desde su casita.

Mide la distancia en línea recta (Valor H): Mira a Pac-Man a lo lejos y calcula cuántos pasos le faltan para llegar.

Elige el camino más corto (Valor F = G + H): Suma los pasos que ya dio más los que le faltan. Siempre decide dar el siguiente paso por el caminito donde esa suma sea más pequeña.

Evita las paredes: Si se topa con un muro azul, el perrito lo ignora y busca otra casilla libre hasta atrapar a Pac-Man.