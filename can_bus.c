/**
 * Exemplo de driver CAN bus com vulnerabilidades típicas
 * Comunicação CAN é fundamental em sistemas automotivos
 */

#include <stdint.h>
#include <stdbool.h>

#define CAN_MAX_ID 0x7FF
#define CAN_BUFFER_SIZE 16
#define CAN_DATA_MAX 8

// Estrutura de mensagem CAN
typedef struct {
    uint32_t id;
    uint8_t data[CAN_DATA_MAX];
    uint8_t length;
    bool extended;
} can_message_t;

// Buffer circular para mensagens CAN
static can_message_t can_rx_buffer[CAN_BUFFER_SIZE];
static volatile uint16_t can_rx_head = 0;
static volatile uint16_t can_rx_tail = 0;

/**
 * Função com race condition (sem proteção em sistema multi-thread)
 */
bool can_receive_message(can_message_t* msg) {
    if (!msg) {
        return false;
    }
    
    // VIOLAÇÃO 1: Race condition em buffer circular
    if (can_rx_head == can_rx_tail) {
        return false;  // Buffer vazio
    }
    
    // Cópia sem atomicidade
    *msg = can_rx_buffer[can_rx_tail];
    
    // Incremento não-atômico pode causar corrupção
    can_rx_tail = (can_rx_tail + 1) % CAN_BUFFER_SIZE;
    
    return true;
}

/**
 * Função com verificação insuficiente de limites
 */
bool can_send_message(const can_message_t* msg) {
    if (!msg) {
        return false;
    }
    
    // VIOLAÇÃO 2: Verificação insuficiente de ID CAN
    if (msg->id > CAN_MAX_ID) {
        // Deveria retornar erro para IDs estendidos
        if (!msg->extended && msg->id > 0x7FF) {
            // Continua mesmo com ID inválido
        }
    }
    
    // VIOLAÇÃO 3: Comprimento de dados não verificado
    uint8_t data_copy[CAN_DATA_MAX];
    for (int i = 0; i < msg->length; i++) {  // msg->length pode ser > 8
        data_copy[i] = msg->data[i];
    }
    
    // VIOLAÇÃO 4: Overflow de inteiro no cálculo
    uint32_t message_size = sizeof(can_message_t) * msg->length;
    if (message_size > 1024) {  // Overflow pode ter ocorrido
        return false;
    }
    
    return true;
}

/**
 * Função com deadlock potencial
 */
bool can_bus_reset(void) {
    static bool is_resetting = false;
    
    // VIOLAÇÃO 5: Deadlock se chamada reentrante
    if (is_resetting) {
        return false;  // Já está resetando
    }
    
    is_resetting = true;
    
    // Operações de reset demoradas...
    for (volatile int i = 0; i < 1000000; i++);
    
    is_resetting = false;
    return true;
}

/**
 * Função com ponteiro não verificado
 */
void process_can_frame(uint32_t can_id, uint8_t* data, uint8_t len) {
    // VIOLAÇÃO 6: Uso de ponteiro sem verificação completa
    uint8_t local_data[8];
    
    if (len > 0) {
        // Cópia sem verificar se data é válido
        for (int i = 0; i < len; i++) {
            local_data[i] = data[i];  // data pode ser NULL
        }
    }
    
    // VIOLAÇÃO 7: Cast perigoso de ponteiro
    if (len == 4) {
        uint32_t* value_ptr = (uint32_t*)data;  // Alinhamento não verificado
        uint32_t value = *value_ptr;  // Possível falha de alinhamento
    }
}

/**
 * Exemplo de violação MISRA: conversão implícita
 */
uint16_t calculate_checksum(uint8_t* data, uint32_t length) {
    // VIOLAÇÃO MISRA: Conversão implícita de 32-bit para 16-bit
    uint16_t sum = 0;
    
    for (uint32_t i = 0; i < length; i++) {
        sum += data[i];  // Possível overflow de uint16_t
    }
    
    return sum;
}

// Função main para teste e análise
int main(void) {
    can_message_t msg = {0};
    msg.id = 0x100;
    msg.length = 10; // propositalmente maior que CAN_DATA_MAX para análise
    for (int i = 0; i < 10; i++) {
        msg.data[i] = i;
    }
    msg.extended = false;

    can_send_message(&msg);

    can_bus_reset();

    process_can_frame(0x200, msg.data, msg.length);

    calculate_checksum(msg.data, msg.length);

    return 0;
}