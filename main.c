#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REFERENCIAS 100

// Estructura tabla hash
typedef struct {
    int pagina;
    int bit_acceso; // Para RELOJ
} Marco;

//Encuentra el largo de la secuencia para usarlo en los remplazos
void leer_referencias(const char *filename, int referencias[], int *num_referencias) {
    //Abre y verifica si se abre bien el archivo
    FILE *archivo = fopen(filename, "r");
    if (!archivo) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }
    //Setea el numero de referencia en 0, y mientras siga leyendo numeros sumara 1
    *num_referencias = 0;
    while (fscanf(archivo, "%d", &referencias[(*num_referencias)]) != EOF) {
        (*num_referencias)++;
    }
    fclose(archivo);
}
//Busca si la pagina esta en uno de los marcos y retorna su posición
int buscar_pagina(Marco marcos[], int num_marcos, int pagina) {
    for (int i = 0; i < num_marcos; i++) {
        if (marcos[i].pagina == pagina) return i;
    }
    //Si no esta retorna -1
    return -1;
}

// ALGORITMO FIFO
void fifo(int referencias[], int num_referencias, int num_marcos) {
    Marco marcos[num_marcos];
    int puntero = 0, fallos = 0;

    for (int i = 0; i < num_marcos; i++) marcos[i].pagina = -1;

    for (int i = 0; i < num_referencias; i++) {
        if (buscar_pagina(marcos, num_marcos, referencias[i]) == -1) {
            marcos[puntero].pagina = referencias[i];
            puntero = (puntero + 1) % num_marcos;
            fallos++;
        }
    }
    printf("FIFO - Fallos de página: %d\n", fallos);
}
// ALGORITMO LRU
void lru(int referencias[], int num_referencias, int num_marcos) {
    Marco marcos[num_marcos];
    int fallos = 0, tiempo[num_marcos];
    int tiempo_actual = 0;

    for (int i = 0; i < num_marcos; i++) {
        marcos[i].pagina = -1;
        tiempo[i] = 0;
    }

    for (int i = 0; i < num_referencias; i++) {
        int pagina_actual = referencias[i];
        int indice = buscar_pagina(marcos, num_marcos, pagina_actual);

        if (indice == -1) { 
            fallos++;
            int reemplazo = 0;
            for (int j = 1; j < num_marcos; j++) {
                if (marcos[j].pagina == -1) { 
                    reemplazo = j;
                    break;
                } else if (tiempo[j] < tiempo[reemplazo]) { 
                    reemplazo = j;
                }
            }
            marcos[reemplazo].pagina = pagina_actual;
            tiempo[reemplazo] = tiempo_actual;
        } else { 
            tiempo[indice] = tiempo_actual;
        }

        tiempo_actual++;
    }

    printf("LRU - Fallos de página: %d\n", fallos);
}
//ALGORITMO OPTIMO
void optimo(int referencias[], int num_referencias, int num_marcos) {
    Marco marcos[num_marcos];
    int fallos = 0;

    for (int i = 0; i < num_marcos; i++) marcos[i].pagina = -1;

    for (int i = 0; i < num_referencias; i++) {
        if (buscar_pagina(marcos, num_marcos, referencias[i]) == -1) {
            int reemplazo = -1, max_distancia = -1;
            for (int j = 0; j < num_marcos; j++) {
                if (marcos[j].pagina == -1) {
                    reemplazo = j;
                    break;
                }
                int distancia = MAX_REFERENCIAS;
                for (int k = i + 1; k < num_referencias; k++) {
                    if (marcos[j].pagina == referencias[k]) {
                        distancia = k;
                        break;
                    }
                }
                if (distancia > max_distancia) {
                    max_distancia = distancia;
                    reemplazo = j;
                }
            }
            marcos[reemplazo].pagina = referencias[i];
            fallos++;
        }
    }
    printf("OPTIMO - Fallos de página: %d\n", fallos);
}
//ALGORITMO LRU
void lru_reloj(int referencias[], int num_referencias, int num_marcos) {
    Marco marcos[num_marcos];
    int puntero = 0, fallos = 0;

    for (int i = 0; i < num_marcos; i++) {
        marcos[i].pagina = -1;
        marcos[i].bit_acceso = 0;
    }

    for (int i = 0; i < num_referencias; i++) {
        int indice = buscar_pagina(marcos, num_marcos, referencias[i]);
        if (indice == -1) {
            while (marcos[puntero].bit_acceso == 1) {
                marcos[puntero].bit_acceso = 0;
                puntero = (puntero + 1) % num_marcos;
            }
            marcos[puntero].pagina = referencias[i];
            marcos[puntero].bit_acceso = 1;
            puntero = (puntero + 1) % num_marcos;
            fallos++;
        } else {
            marcos[indice].bit_acceso = 1;
        }
    }
    printf("RELOJ - Fallos de página: %d\n", fallos);
}

int main(int argc, char *argv[]) {
    //Verifica que tenga la estructura correcta
    if (argc != 7) {
        fprintf(stderr, "Uso: %s -m <num_marcos> -a <algoritmo> -f <archivo>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_marcos = 0;
    char algoritmo[10];
    char archivo[50];

    //Extrae los parametros
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-m") == 0) {
            num_marcos = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-a") == 0) {
            strcpy(algoritmo, argv[i + 1]);
        } else if (strcmp(argv[i], "-f") == 0) {
            strcpy(archivo, argv[i + 1]);
        }
    }

    int referencias[MAX_REFERENCIAS], num_referencias;
    // Lee las referencias para guardar el numero de ellas
    leer_referencias(archivo, referencias, &num_referencias);

    // Dependiendo del algoritmo escrito se ejecuta uno u otro
    if (strcmp(algoritmo, "FIFO") == 0) {
        fifo(referencias, num_referencias, num_marcos);
    } else if (strcmp(algoritmo, "LRU") == 0) {
        lru(referencias, num_referencias, num_marcos);
    } else if (strcmp(algoritmo, "OPTIMO") == 0) {
        optimo(referencias, num_referencias, num_marcos);
    } else if (strcmp(algoritmo, "RELOJ") == 0) {
        lru_reloj(referencias, num_referencias, num_marcos);
    } else {
        //Si lee un algoritmo que no está
        fprintf(stderr, "Algoritmo incorrecto: %s\n", algoritmo);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
