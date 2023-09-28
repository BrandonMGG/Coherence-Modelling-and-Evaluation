#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 8

// Estructura para representar una instrucción
struct Instruction {
    char op[10];
    char address[4];
    int data;
};

// Función para generar un número aleatorio en punto flotante entre 0 y 1
double random_m() {
    return (double)rand() / RAND_MAX;
}

// Función para generar un número entero aleatorio en un rango [min, max]
int randint(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// Función para simular una distribución geométrica
int geometric(int n, int value) {
    int trials = 0;
    while (1) {
        if (randint(0, n) == value) {
            trials++;
            break;
        }
        trials++;
    }
    return trials;
}

// Función para obtener una dirección aleatoria
int get_random_address() {
    return randint(0, ARRAY_SIZE - 1);
}

int random_Hex() {
    // Generar un número aleatorio entre 0 y 15
    int randomNumber = rand() % 16;

    // Mostrar el número en formato hexadecimal con "0x"
    return randomNumber;
}

// Función para obtener datos aleatorios en formato hexadecimal
int get_random_data() {
    int value;
    char hexString[10]; // Ajusta el tamaño según tus necesidades

    // Genera un valor entero aleatorio (por ejemplo, entre 0 y 255)
    value = rand() % 256;

    // Convierte el valor entero a una cadena de caracteres hexadecimal
    sprintf(hexString, "%X", value);

    // Convierte la cadena hexadecimal nuevamente a un valor entero
    int result = strtol(hexString, NULL, 16);

    return result;
}

// Función para obtener una instrucción aleatoria y almacenarla en una estructura
void get_random_instruction(struct Instruction* instr) {
    int n = 1; // Número de tipos de instrucciones: "READ", "WRITE", "CALC"
    int op_index = geometric(n, n - 1);

    if (op_index == 1) {
        sprintf(instr->op, "READ");
        sprintf(instr->address, "0x%X", random_Hex());
        instr->data = 0;
    } else if (op_index == 2) {
        sprintf(instr->op, "WRITE");
        sprintf(instr->address, "0x%X", random_Hex());
        instr->data = get_random_data();
    } else {
        sprintf(instr->op, "INCR");
        sprintf(instr->address, "0x%X", random_Hex());
        instr->data= 0;
    }
}


/*
int main() {
    srand(time(NULL)); // Inicializar la semilla aleatoria

    struct Instruction instructions[10]; // Crear un arreglo de estructuras para almacenar 10 instrucciones

    // Generar y mostrar instrucciones aleatorias
    for (int i = 0; i < 10; i++) {
        get_random_instruction(&instructions[i]);
        printf("Instrucción %d: Op=%s, Address=%s, Data=%s\n", i + 1, instructions[i].op, instructions[i].address, instructions[i].data);
    }
    
    return 0;
}
*/