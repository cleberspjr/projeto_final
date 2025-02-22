#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include <stdlib.h> 

// Definições dos pinos e constantes
#define I2C_INTERFACE i2c1
#define SDA_PIN 14
#define SCL_PIN 15
#define OLED_ADDR 0x3C
#define BTN_JOYSTICK 22    // Pino GP22 para o botão do joystick
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define DRY_THRESHOLD 2000 // Limite de umidade para considerar o solo seco

// Definições dos pinos dos LEDs
#define LED_RED_PIN 13     // Pino GP13 para o LED vermelho
#define LED_GREEN_PIN 11   // Pino GP11 para o LED verde
#define LED_BLUE_PIN 12    // Pino GP12 para o LED azul

// Matriz 5x5 que mostra os números de 0 a 5
const uint32_t numbers[6][25] = {
    {1,1,1,1,1,  1,0,0,0,1,  1,0,0,0,1,  1,0,0,0,1,  1,1,1,1,1}, // 0
    {1,1,1,1,1,  0,0,1,0,0,  0,0,1,0,1,  0,1,1,0,0,  0,0,1,0,0}, // 1
    {1,1,1,1,1,  1,0,0,0,0,  1,1,1,1,1,  0,0,0,0,1,  1,1,1,1,1}, // 2
    {1,1,1,1,1,  0,0,0,0,1,  0,1,1,1,1,  0,0,0,0,1,  1,1,1,1,1}, // 3
    {1,0,0,0,0,  0,0,0,0,1,  1,1,1,1,1,  1,0,0,0,1,  1,0,0,0,1}, // 4
    {1,1,1,1,1,  0,0,0,0,1,  1,1,1,1,1,  1,0,0,0,0,  1,1,1,1,1}, // 5
};

// Variáveis globais
ssd1306_t display; // Estrutura do display
static volatile uint32_t last_time_btn_sw = 0; // Tempo do último evento do botão do joystick
static bool show_moisture = false; // Indica se deve exibir a umidade

// Protótipos das funções
void CONFIGURE_i2c();
void CONFIGURE_DISPLAY(ssd1306_t *ssd);
void CONFIGURE_BTN(uint8_t btn_pin);
void CONFIGURE_LEDS();
void HANDLER_BTN_irq(uint gpio, uint32_t events);
uint16_t simulate_soil_moisture(); // Simula a leitura da umidade do solo
void display_number(ssd1306_t *ssd, int number);
void display_moisture(ssd1306_t *ssd, uint16_t moisture);
void countdown_and_display_moisture();
void set_led_color(uint16_t moisture); // Define a cor do LED com base na umidade

// Função principal
int main() {
    stdio_init_all(); // Inicializa a comunicação serial (para depuração)

    // Configura o I2C, o display, o botão e os LEDs
    CONFIGURE_i2c();
    CONFIGURE_DISPLAY(&display);
    CONFIGURE_BTN(BTN_JOYSTICK);
    CONFIGURE_LEDS();

    // Exibe mensagem inicial no display
    ssd1306_clear(&display);
    ssd1306_draw_string(&display, "Clique para iniciar", 0, 0);
    ssd1306_send_data(&display);

    // Loop principal
    while (true) {
        if (show_moisture) {
            // Inicia a contagem regressiva e exibe a umidade
            countdown_and_display_moisture();
            show_moisture = false; // Reseta o flag
        }
        sleep_ms(100); // Aguarda um pouco antes da próxima leitura
    }
    return 0;
}

// Função para realizar a contagem regressiva e exibir a umidade
void countdown_and_display_moisture() {
    // Contagem regressiva de 5 a 0
    for (int i = 5; i >= 0; i--) {
        display_number(&display, i); // Exibe o número atual
        sleep_ms(1000); // Aguarda 1 segundo
    }
    // Simula a leitura da umidade e exibe no display
    uint16_t moisture = simulate_soil_moisture();
    display_moisture(&display, moisture);
    set_led_color(moisture); // Define a cor do LED com base na umidade
}

// Configura o I2C
void CONFIGURE_i2c() {
    i2c_init(I2C_INTERFACE, 400 * 1000); // Inicializa o I2C com frequência de 400 kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C); // Configura o pino SDA
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C); // Configura o pino SCL
    gpio_pull_up(SDA_PIN); // Habilita pull-up no pino SDA
    gpio_pull_up(SCL_PIN); // Habilita pull-up no pino SCL
}

// Configura o display SSD1306
void CONFIGURE_DISPLAY(ssd1306_t *ssd) {
    ssd1306_init(ssd, OLED_WIDTH, OLED_HEIGHT, false, OLED_ADDR, I2C_INTERFACE); // Inicializa o display
    ssd1306_config(ssd); // Configura o display
}

// Configura o botão do joystick
void CONFIGURE_BTN(uint8_t btn_pin) {
    gpio_init(btn_pin); // Inicializa o pino do botão
    gpio_set_dir(btn_pin, GPIO_IN); // Define como entrada
    gpio_pull_up(btn_pin); // Habilita pull-up interno
    gpio_set_irq_enabled_with_callback(btn_pin, GPIO_IRQ_EDGE_FALL, true, &HANDLER_BTN_irq); // Configura interrupção
}

// Configura os pinos dos LEDs
void CONFIGURE_LEDS() {
    gpio_init(LED_RED_PIN); // Inicializa o pino do LED vermelho
    gpio_init(LED_GREEN_PIN); // Inicializa o pino do LED verde
    gpio_init(LED_BLUE_PIN); // Inicializa o pino do LED azul
    gpio_set_dir(LED_RED_PIN, GPIO_OUT); // Define como saída
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT); // Define como saída
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT); // Define como saída
    gpio_put(LED_RED_PIN, 0);   // Inicialmente desligado
    gpio_put(LED_GREEN_PIN, 0); // Inicialmente desligado
    gpio_put(LED_BLUE_PIN, 0);  // Inicialmente desligado
}

// Trata as interrupções dos botões
void HANDLER_BTN_irq(uint gpio, uint32_t events) {
    uint32_t current_time = to_us_since_boot(get_absolute_time()); // Obtém o tempo atual
    // Debouncing para o botão do joystick
    if (gpio == BTN_JOYSTICK && (current_time - last_time_btn_sw > 200000)) {
        show_moisture = true; // Ativa o flag para exibir a umidade
        last_time_btn_sw = current_time; // Atualiza o tempo do último evento
    }
}

// Simula a leitura da umidade do solo
uint16_t simulate_soil_moisture() {
    // Gera um valor aleatório entre 1500 e 3000 para simular a umidade
    return 1500 + (rand() % 1501);
}

// Exibe um número no display
void display_number(ssd1306_t *ssd, int number) {
    ssd1306_clear(ssd); // Limpa o display
    char num_str[2];
    snprintf(num_str, sizeof(num_str), "%d", number); // Converte o número para string
    ssd1306_draw_string(ssd, num_str, OLED_WIDTH / 2 - 5, OLED_HEIGHT / 2 - 5); // Exibe o número centralizado
    ssd1306_send_data(ssd); // Atualiza o display
}

// Exibe a umidade no display
void display_moisture(ssd1306_t *ssd, uint16_t moisture) {
    char moisture_str[20];
    snprintf(moisture_str, sizeof(moisture_str), "Umidade: %d", moisture); // Formata a string da umidade
    ssd1306_clear(ssd); // Limpa o display
    ssd1306_draw_string(ssd, moisture_str, 0, 0); // Exibe a string da umidade
    ssd1306_send_data(ssd); // Atualiza o display
}

// Define a cor do LED com base na umidade
void set_led_color(uint16_t moisture) {
    // Desliga todos os LEDs antes de acender o correto
    gpio_put(LED_RED_PIN, 0);
    gpio_put(LED_GREEN_PIN, 0);
    gpio_put(LED_BLUE_PIN, 0);

    // Define a cor do LED com base na umidade
    if (moisture < 1800) {
        gpio_put(LED_RED_PIN, 1);    // Seco: LED vermelho
    } else if (moisture >= 1800 && moisture < 2200) {
        gpio_put(LED_BLUE_PIN, 1);   // Moderado: LED azul
    } else {
        gpio_put(LED_GREEN_PIN, 1);  // Úmido: LED verde
    }
}