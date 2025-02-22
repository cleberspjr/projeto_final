#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include <stdlib.h> // Para a função rand()

// Definições dos pinos e constantes
#define I2C_INTERFACE i2c1
#define SDA_PIN 14
#define SCL_PIN 15
#define OLED_ADDR 0x3C
#define LED_PIN 16         // Pino GP16 para o LED
#define OLED_WIDTH 128
#define OLED_HEIGHT 64
#define DRY_THRESHOLD 2000 // Limite de umidade para considerar o solo seco

// Variáveis globais
ssd1306_t display; // Estrutura do display

// Protótipos das funções
void CONFIGURE_i2c();
void CONFIGURE_DISPLAY(ssd1306_t *ssd);
uint16_t simulate_soil_moisture(); // Simula a leitura da umidade do solo
void display_moisture(ssd1306_t *ssd, uint16_t moisture);

// Função principal
int main() {
    stdio_init_all(); // Inicializa a comunicação serial (para depuração)

    // Inicializa o I2C e o display
    CONFIGURE_i2c();
    CONFIGURE_DISPLAY(&display);

    // Configura o LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0); // Desliga o LED inicialmente

    while (true) {
        // Simula a leitura da umidade do solo
        uint16_t moisture = simulate_soil_moisture();

        // Exibe a umidade no display
        display_moisture(&display, moisture);

        // Verifica se o solo está seco
        if (moisture < DRY_THRESHOLD) {
            gpio_put(LED_PIN, 1); // Liga o LED
        } else {
            gpio_put(LED_PIN, 0); // Desliga o LED
        }

        // Aguarda um pouco antes da próxima leitura
        sleep_ms(1000);
    }

    return 0;
}

// Inicializa o I2C
void CONFIGURE_i2c() {
    i2c_init(I2C_INTERFACE, 400 * 1000); // Inicializa o I2C com frequência de 400 kHz
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C); // Configura o pino SDA
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C); // Configura o pino SCL
    gpio_pull_up(SDA_PIN); // Habilita pull-up no pino SDA
    gpio_pull_up(SCL_PIN); // Habilita pull-up no pino SCL
}

// Inicializa o display SSD1306
void CONFIGURE_DISPLAY(ssd1306_t *ssd) {
    ssd1306_init(ssd, OLED_WIDTH, OLED_HEIGHT, false, OLED_ADDR, I2C_INTERFACE); // Inicializa o display
    ssd1306_config(ssd); // Configura o display
}

// Simula a leitura da umidade do solo
uint16_t simulate_soil_moisture() {
    // Gera um valor aleatório entre 1500 e 3000 para simular a umidade
    return 1500 + (rand() % 1501);
}

// Exibe a umidade no display
void display_moisture(ssd1306_t *ssd, uint16_t moisture) {
    char moisture_str[20];
    snprintf(moisture_str, sizeof(moisture_str), "Umidade: %d", moisture);

    ssd1306_clear(ssd); // Limpa o display
    ssd1306_draw_string(ssd, moisture_str, 0, 0); // Exibe a string
    ssd1306_send_data(ssd); // Atualiza o display
}