#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Estructura para pasar datos a los threads
typedef struct {
    int id;
    int cantidad;
} ThreadData;

// Sección crítica para sincronizar la salida (evitar que se mezcle el texto)
CRITICAL_SECTION cs;

// Función que ejecutará cada thread
DWORD WINAPI generarNumerosAleatorios(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;
    
    // Inicializar generador aleatorio (diferente para cada thread)
    srand(time(NULL) + data->id);
    
    for (int i = 0; i < data->cantidad; i++) {
        // Generar número aleatorio entre 1 y 100
        int numeroAleatorio = (rand() % 100) + 1;
        
        // Entrar en sección crítica para imprimir sin que se mezcle
        EnterCriticalSection(&cs);
        printf("[Thread %d] Número %d: %d\n", data->id, i + 1, numeroAleatorio);
        LeaveCriticalSection(&cs);
        
        // Pequeña pausa para ver mejor el efecto del multithreading
        Sleep(rand() % 500 + 100);  // Entre 100 y 600 ms
    }
    
    // Mensaje final
    EnterCriticalSection(&cs);
    printf("[Thread %d] ✓ Finalizado!\n", data->id);
    LeaveCriticalSection(&cs);
    
    return 0;
}

int main() {
    printf("=================================================\n");
    printf("  EJEMPLO DE MULTITHREADING EN C (Windows API)\n");
    printf("=================================================\n\n");
    
    // Inicializar la sección crítica
    InitializeCriticalSection(&cs);
    
    // Configuración
    const int NUM_THREADS = 4;
    const int NUMEROS_POR_THREAD = 5;
    
    HANDLE threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];
    
    printf("Creando %d threads...\n", NUM_THREADS);
    printf("Cada thread generará %d números aleatorios\n\n", NUMEROS_POR_THREAD);
    
    // Crear los threads
    for (int i = 0; i < NUM_THREADS; i++) {
        threadData[i].id = i + 1;
        threadData[i].cantidad = NUMEROS_POR_THREAD;
        
        threads[i] = CreateThread(
            NULL,                           // Seguridad por defecto
            0,                              // Tamaño de stack por defecto
            generarNumerosAleatorios,       // Función a ejecutar
            &threadData[i],                 // Parámetros
            0,                              // Flags
            NULL                            // ID del thread (no necesario)
        );
        
        if (threads[i] == NULL) {
            printf("Error creando thread %d\n", i + 1);
            return 1;
        }
        
        printf("✓ Thread %d creado\n", i + 1);
    }
    
    printf("\n--- Generando números aleatorios... ---\n\n");
    
    // Esperar a que todos los threads terminen
    WaitForMultipleObjects(NUM_THREADS, threads, TRUE, INFINITE);
    
    printf("\n=================================================\n");
    printf("  Todos los threads han finalizado\n");
    printf("=================================================\n");
    
    // Limpiar recursos
    for (int i = 0; i < NUM_THREADS; i++) {
        CloseHandle(threads[i]);
    }
    
    DeleteCriticalSection(&cs);
    
    printf("\n✓ Recursos liberados correctamente\n");
    
    return 0;
}
