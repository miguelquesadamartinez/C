#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int suma(int a, int b);
int factorial(int n);
void crearArchivo(const char *nombreArchivo, const char *texto);
void leerArchivo(const char *nombreArchivo);
void saludarUsuario();

int main() {

    int *ptr = (int*)malloc(sizeof(int));
    if (ptr == NULL) {
        printf("Error al asignar memoria\n");
        return 1;
    }
    
    *ptr = 42;
    printf("Valor almacenado en el puntero: %d\n", *ptr);
    printf("Dirección de memoria: %p\n", (void*)ptr);
    
    // Crear archivo de texto
    crearArchivo("salida.txt", "Hola, este es un archivo generado desde C!\nSegunda linea de ejemplo.\n");
    printf("Archivo creado exitosamente\n");

    // Leer y mostrar contenido del archivo
    printf("\n--- Contenido del archivo ---\n");
    leerArchivo("salida.txt");
    printf("--- Fin del archivo ---\n\n");

    free(ptr);

    int x = 10;
    int *pp = &x;

    /*
    x → valor
    &x → dirección
    pp → guarda esa dirección
    */

    //Strings (no son objetos, son trampas)
    char s[] = "hola";        // array
    char *p = "hola";        // literal (NO modificar)

    return 0;
}

void crearArchivo(const char *nombreArchivo, const char *texto) {
    FILE *archivo = fopen(nombreArchivo, "a+");

    if (archivo == NULL) {
        printf("Error: No se pudo crear el archivo %s\n", nombreArchivo);
        return;
    }

    printf("Archivo: %s\n", nombreArchivo);
    printf("texto a escribir: %s\n", texto);

    fprintf(archivo, "%s\n", texto);
    fclose(archivo);
}

void leerArchivo(const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");

    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo %s\n", nombreArchivo);
        return;
    }

    char linea[256];
    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        printf("%s", linea);
    }

    fclose(archivo);
}
