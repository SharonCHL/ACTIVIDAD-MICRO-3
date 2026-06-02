/*
 * main.c
 * Lógica de control escalonado para 8 sensores
 */ 
#include "leds.h"
#include "motors.h"
#include "adc.h"
#include "irsensors.h"
#include <util/delay.h>
#include <stdint.h>
#include <avr/io.h> // Necesario para leer los puertos directamente

// Definición de velocidades (ajustables de 0 a 255)
#define VEL_MAX  120  // Sensor más cercano al centro
#define VEL_ALTA 100  // Siguiente sensor
#define VEL_MED  95  // Siguiente sensor
#define VEL_BAJA 70   // Sensor más alejado

// --- AQUÍ ESTÁ LA FUNCIÓN QUE FALTABA ---
uint8_t read_sensors_raw(void) {
    /* * Retorna el valor directo de los pines. 
     * Asumimos que los 8 sensores están en el Puerto D.
     * Si en tu circuito físico están en otro puerto, cambia PIND por PINB o PINC.
     */
    return PIND; 
}
// ----------------------------------------

int main(void)
{
    // Inicialización de periféricos
    leds_init();
    test_leds();
    motors_init();
    
    while (1) 
    {
        // 1. Leer el estado de los 8 sensores como un byte (0 a 255)
        uint8_t sensores = read_sensors_raw(); 

        // 2. Condición de paro: Todos detectan blanco (Nivel Bajo = 0)
        if (sensores == 0x00) 
        {
            motor_left(MOTOR_FORWARD, 0);
            motor_right(MOTOR_FORWARD, 0);
            continue; // Vuelve al inicio del while
        }

        uint8_t vel_izq = 0;
        uint8_t vel_der = 0;

        // 3. Activar MOTOR IZQUIERDO (Sensores de la extrema izquierda: bits 7, 6, 5, 4)
        if      (sensores & (1 << 4)) vel_izq = VEL_MAX;  // Más cercano al centro
        else if (sensores & (1 << 5)) vel_izq = VEL_ALTA;
        else if (sensores & (1 << 6)) vel_izq = VEL_MED;
        else if (sensores & (1 << 7)) vel_izq = VEL_BAJA; // Más alejado

        // 4. Activar MOTOR DERECHO (Sensores de la extrema derecha: bits 3, 2, 1, 0)
        if      (sensores & (1 << 3)) vel_der = VEL_MAX;  // Más cercano al centro
        else if (sensores & (1 << 2)) vel_der = VEL_ALTA;
        else if (sensores & (1 << 1)) vel_der = VEL_MED;
        else if (sensores & (1 << 0)) vel_der = VEL_BAJA; // Más alejado

        // 5. Aplicar la potencia a los motores
        motor_left(MOTOR_FORWARD, vel_izq);
        motor_right(MOTOR_FORWARD, vel_der);
    }
}