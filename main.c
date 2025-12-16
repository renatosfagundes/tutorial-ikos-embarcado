/**
 * Programa principal para demonstração
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
// Declarações das funções dos exemplos
void sensor_main_task(void);
void safe_sensor_main(void);

// Função vulnerável para demonstrar análise
int problematic_function(int x, int y) {
    // Exemplo de overflow de inteiro
    int result = x * y;  // Possível overflow
    
    // Divisão por zero potencial
    int divisor = y - x;
    return result / divisor;
}

// Exemplo de loop infinito potencial
void process_data(uint8_t* data, size_t size) {
    size_t i = 0;
    
    // Loop pode ser infinito se size for 0
    while (i < size) {
        data[i] = 0;
        i++;  // Mas i nunca incrementa se size for 0?
    }
}

int main(void) {
    printf("Iniciando demonstração IKOS para ESP32\n");
    
    // Executar exemplos
    printf("\n1. Executando exemplo vulnerável...\n");
    sensor_main_task();
    
    printf("\n2. Executando exemplo seguro...\n");
    safe_sensor_main();
    
    printf("\n3. Testando função problemática...\n");
    int x = problematic_function(1000000, 1000000);
    printf("Resultado: %d\n", x);
    
    printf("\nDemonstração concluída!\n");
    
    return 0;
}

// Simulação de hardware do ESP32
#define ADC_MAX_CHANNELS 8
#define BUFFER_SIZE 64

// Estrutura para dados do sensor
typedef struct {
    uint8_t channel;
    int16_t raw_value;
    float calibrated_value;
    uint32_t timestamp;
} sensor_data_t;

// Variável global problemática
sensor_data_t* global_sensor_data = NULL;

/**
 * Função com múltiplas vulnerabilidades
 */
int read_sensor_data(uint8_t sensor_id, int* output_buffer, size_t buffer_size) {
    // VIOLAÇÃO 1: Uso de variável não inicializada
    int local_uninitialized;
    if (local_uninitialized > 0) {  // Comportamento indefinido
        *output_buffer = 0;
    }
    
    // VIOLAÇÃO 2: Divisão por zero potencial
    int calibration_factor = get_calibration_factor(sensor_id);
    int scaled_value = 100 / calibration_factor;  // Possível divisão por zero
    
    // VIOLAÇÃO 3: Buffer overflow potencial
    int temp_buffer[10];
    for (size_t i = 0; i < buffer_size; i++) {  // Se buffer_size > 10
        temp_buffer[i] = i * scaled_value;
        output_buffer[i] = temp_buffer[i];
    }
    
    // VIOLAÇÃO 4: Uso de memória após liberação
    if (global_sensor_data) {
        free(global_sensor_data);
        global_sensor_data = NULL;
    }
    
    // Acesso após free em certos caminhos
    if (sensor_id == 0xFF) {
        global_sensor_data->channel = sensor_id;  // Use-after-free
    }
    
    return scaled_value;
}

/**
 * Função com estouro de buffer garantido
 */
void process_sensor_packet(uint8_t* packet, size_t packet_size) {
    // VIOLAÇÃO 5: Buffer overflow garantido
    char small_buffer[32];
    memcpy(small_buffer, packet, packet_size);  // Sem verificação de tamanho
    
    // VIOLAÇÃO 6: Acesso fora dos limites do array
    int sensor_array[5];
    for (int i = 0; i <= 5; i++) {  // Índice 5 está fora dos limites
        sensor_array[i] = i * 10;
    }
}

/**
 * Função com null pointer dereference
 */
int* get_sensor_config(uint8_t sensor_id) {
    // VIOLAÇÃO 7: Retorno de ponteiro para variável local
    int local_config[3] = {1, 2, 3};
    
    if (sensor_id > ADC_MAX_CHANNELS) {
        return NULL;  // Pode retornar NULL
    }
    
    return local_config;  // Retorna ponteiro para stack
}

/**
 * Função auxiliar simulada
 */
int get_calibration_factor(uint8_t sensor_id) {
    // Simula retorno ocasional de zero
    return (sensor_id % 5 == 0) ? 0 : 10;
}

/**
 * Função principal para demonstração
 */
void sensor_main_task(void) {
    int output_data[20];
    uint8_t large_packet[100];
    
    // Chamadas problemáticas
    read_sensor_data(5, output_data, 20);  // buffer_size maior que array interno
    process_sensor_packet(large_packet, 100);  // packet_size maior que buffer
    
    // Uso de função que pode retornar NULL
    int* config = get_sensor_config(10);
    if (config) {
        int value = config[0];  // Possível acesso inválido (stack liberada)
    }
}

/**
 * Exemplo de implementação segura seguindo boas práticas
 */

#define SENSOR_MAX_CHANNELS 8
#define BUFFER_CAPACITY 64
#define CALIBRATION_MIN 1  // Evitar divisão por zero

// Estrutura com inicialização clara
typedef struct {
    uint8_t channel;
    int16_t raw_value;
    float calibrated_value;
    uint32_t timestamp;
    bool initialized;
} safe_sensor_data_t;

// Buffer protegido com informação de tamanho
typedef struct {
    int data[BUFFER_CAPACITY];
    size_t size;
} safe_buffer_t;

/**
 * Função segura com todas as verificações
 */
bool safe_read_sensor_data(uint8_t sensor_id, safe_buffer_t* output) {
    // VALIDAÇÃO 1: Verificação de parâmetros de entrada
    if (!output || sensor_id >= SENSOR_MAX_CHANNELS) {
        return false;
    }
    
    // VALIDAÇÃO 2: Buffer deve ter capacidade
    if (output->size > BUFFER_CAPACITY) {
        return false;
    }
    
    // VALIDAÇÃO 3: Evitar divisão por zero
    int calibration_factor = safe_get_calibration(sensor_id);
    if (calibration_factor < CALIBRATION_MIN) {
        calibration_factor = CALIBRATION_MIN;
    }
    
    // VALIDAÇÃO 4: Loop com limites verificados
    size_t elements_to_process = output->size;
    if (elements_to_process > BUFFER_CAPACITY) {
        elements_to_process = BUFFER_CAPACITY;
    }
    
    int temp_buffer[BUFFER_CAPACITY];
    for (size_t i = 0; i < elements_to_process; i++) {
        temp_buffer[i] = (int)i * (100 / calibration_factor);
        output->data[i] = temp_buffer[i];
    }
    
    return true;
}

/**
 * Função segura com cópia protegida
 */
bool safe_copy_sensor_data(const uint8_t* source, size_t source_size,
                          uint8_t* dest, size_t dest_capacity) {
    // VALIDAÇÃO: Verificar todos os ponteiros e tamanhos
    if (!source || !dest) {
        return false;
    }
    
    // Garantir que não há overflow
    if (source_size > dest_capacity) {
        return false;
    }
    
    // Cópia segura
    memcpy(dest, source, source_size);
    return true;
}

/**
 * Função com retorno seguro
 */
const char* safe_get_sensor_name(uint8_t sensor_id) {
    // Array estático - vida útil durante todo o programa
    static const char* sensor_names[] = {
        "Temperatura",
        "Pressão",
        "Rotação",
        "Combustível",
        "Óleo",
        "Voltagem",
        "Corrente",
        "Posição"
    };
    
    if (sensor_id >= SENSOR_MAX_CHANNELS) {
        return "Desconhecido";
    }
    
    return sensor_names[sensor_id];
}

/**
 * Função de calibração segura
 */
int safe_get_calibration(uint8_t sensor_id) {
    static const int calibration_values[SENSOR_MAX_CHANNELS] = {
        10, 12, 8, 15, 20, 5, 7, 9
    };
    
    if (sensor_id >= SENSOR_MAX_CHANNELS) {
        return CALIBRATION_MIN;
    }
    
    int value = calibration_values[sensor_id];
    
    // Garantir valor mínimo seguro
    return (value < CALIBRATION_MIN) ? CALIBRATION_MIN : value;
}

/**
 * Inicialização segura de estrutura
 */
bool safe_init_sensor_data(safe_sensor_data_t* sensor) {
    if (!sensor) {
        return false;
    }
    
    // Inicialização explícita de todos os campos
    sensor->channel = 0;
    sensor->raw_value = 0;
    sensor->calibrated_value = 0.0f;
    sensor->timestamp = 0;
    sensor->initialized = true;
    
    return true;
}

/**
 * Exemplo de uso correto
 */
void safe_sensor_main(void) {
    safe_buffer_t buffer = { .size = 10 };
    safe_sensor_data_t sensor;
    
    // Inicialização segura
    if (!safe_init_sensor_data(&sensor)) {
        // Tratamento de erro
        return;
    }
    
    // Leitura segura
    if (!safe_read_sensor_data(3, &buffer)) {
        // Tratamento de erro
        return;
    }
    
    // Acesso seguro
    const char* name = safe_get_sensor_name(3);
    // Uso seguro...
}