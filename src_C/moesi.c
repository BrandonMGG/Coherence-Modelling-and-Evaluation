#include <stdio.h>

// Estados del protocolo MOESI
typedef enum {
    MODIFIED,
    OWNED,
    EXCLUSIVE,
    SHARED,
    INVALID
} MOESIState;

// Eventos del protocolo MOESI
typedef enum {
    READ,
    WRITE,
    BUSREAD,
    BUSREADX,
    BUSWRITE
} MOESIEvent;

// Función para realizar una transición de estado
MOESIState transition(MOESIState current_state, MOESIEvent event) {
    MOESIState new_state = current_state;

    switch (current_state) {
        case MODIFIED:
            if (event == BUSWRITE) {
                new_state = INVALID;
            } else if (event == BUSREAD) {
                new_state = OWNED;
            } else if (event == READ) {
                // Acción: No hace nada en este estado
            } else if (event == BUSREADX) {
                // Acción: No hace nada en este estado
            } else if (event == WRITE) {
                // Acción: No hace nada en este estado
            }
            break;

        case OWNED:
            if (event == WRITE) {
                new_state = MODIFIED;
            } else if (event == BUSWRITE) {
                new_state = INVALID;
            } else if (event == BUSREAD) {
                // Acción: No hace nada en este estado
            } else if (event == BUSREADX) {
                // Acción: No hace nada en este estado
            } else if (event == READ) {
                // Acción: No hace nada en este estado
            }
            break;

        case EXCLUSIVE:
            if (event == WRITE) {
                new_state = MODIFIED;
            } else if (event == BUSREAD) {
                new_state = SHARED;
            } else if (event == BUSWRITE) {
                new_state = INVALID;
            } else if (event == READ) {
                // Acción: No hace nada en este estado
            } else if (event == BUSREADX) {
                // Acción: No hace nada en este estado
            }
            break;

        case SHARED:
            if (event == WRITE) {
                new_state = MODIFIED;
            } else if (event == BUSWRITE) {
                new_state = INVALID;
            } else if (event == BUSREAD) {
                // Acción: No hace nada en este estado
            } else if (event == BUSREADX) {
                // Acción: No hace nada en este estado
            } else if (event == READ) {
                // Acción: No hace nada en este estado
            }
            break;

        case INVALID:
            if (event == WRITE) {
                new_state = MODIFIED;
            } else if (event == READ) {
                new_state = EXCLUSIVE;
            } else if (event == BUSREAD) {
                // Acción: No hace nada en este estado
            } else if (event == BUSREADX) {
                // Acción: No hace nada en este estado
            } else if (event == BUSWRITE) {
                // Acción: No hace nada en este estado
            }
            break;
    }

    //Bus Snoopy
    if ((current_state == EXCLUSIVE && new_state == SHARED) ||
        (current_state == INVALID && new_state == EXCLUSIVE) ||
        (current_state == INVALID && new_state == MODIFIED)) {
        // Generar un mensaje para invalidar la dirección de memoria
        
        printf("Nothing_Nothing_Nothing\n");
        
    }
    else if (current_state == MODIFIED && new_state == INVALID)
    {
        printf("Invalida_Address_BusWrite\n");
    }
    else if (current_state == OWNED && new_state == INVALID)
    {
        printf("Transmitir_Address_BusWrite\n");
    }
    else if (current_state == SHARED && new_state == INVALID)
    {
        printf("Invalida_Address_BusRead\n");
    }
    else {
        printf("Nothing_Nothing_Nothing\n");
    }

    return new_state;
}

int main() {
    // Ejemplo de uso de la máquina de estados
    MOESIState current_state = MODIFIED;

    printf("Estado actual: MODIFIED\n");

    // Simulemos eventos
    MOESIEvent events[] = {READ, WRITE, BUSREAD, BUSWRITE, READ};
    int num_events = sizeof(events) / sizeof(events[0]);

    for (int i = 0; i < num_events; i++) {
        current_state = transition(current_state, events[i]);
        printf("Evento: %d -> Nuevo estado: ", events[i]);

        switch (current_state) {
            case MODIFIED:
                printf("MODIFIED\n");
                break;
            case OWNED:
                printf("OWNED\n");
                break;
            case EXCLUSIVE:
                printf("EXCLUSIVE\n");
                break;
            case SHARED:
                printf("SHARED\n");
                break;
            case INVALID:
                printf("INVALID\n");
                break;
        }
    }

    return 0;
}
