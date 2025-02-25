Aluno: Cleber Santos Pinto Júnior
U7 - Tarefa Final - Monitoramento de Umidade do Solo

 Apresentação
O projeto consiste em um sistema de monitoramento de umidade do solo utilizando um Raspberry Pi Pico W. O sistema mede a umidade do solo, exibe os valores em um display OLED e aciona LEDs indicadores de acordo com os níveis de umidade. A medição é acionada por um joystick, que também inicia uma contagem regressiva antes da exibição dos dados.
O sistema é inicializado e aguarda o acionamento do joystick. Quando pressionado, inicia-se uma contagem regressiva de 5 segundos. Após esse tempo, o sistema realiza a leitura da umidade e exibe o valor no display OLED. Com base no nível de umidade, um LED é acionado: vermelho para solo seco, azul para umidade moderada e verde para solo úmido.

    Especificação do Hardware 
Diagrama em Bloco


      Funções de cada bloco 
Raspberry Pi Pico (MCU): O microcontrolador principal que coordena todas as operações, incluindo a leitura do botão, controle dos LEDs e comunicação com o display OLED.
Display OLED SSD1306: Exibe informações visuais, como a contagem regressiva e a umidade do solo simulada.
Joystick Button (GP22): Botão que, quando pressionado, inicia a contagem regressiva e a exibição da umidade do solo.
LEDs (RGB): Indicadores visuais que mostram o estado da umidade do solo (vermelho para seco, azul para moderado e verde para úmido).
I2C Interface: Comunicação entre o Raspberry Pi Pico e o display OLED.
GPIO Interface: Comunicação entre o Raspberry Pi Pico e os LEDs, além do botão do joystick.

      Configuração de cada bloco 
Raspberry Pi Pico (MCU):
Configurado para operar com um clock de 125 MHz.
Inicializa as interfaces I2C e GPIO.
Configura interrupções para o botão do joystick.

Display OLED SSD1306:
Configurado para operar em modo I2C com endereço 0x3C.
Inicializado com resolução de 128x64 pixels.
Configurado para exibir texto e números.

Joystick Button (GP22):
Configurado como entrada com pull-up interno.
Configurado para gerar uma interrupção na borda de descida (quando o botão é pressionado).

LEDs (RGB):
Configurados como saídas digitais.
Inicialmente desligados.

I2C Interface:
Configurado com os pinos SDA (GP14) e SCL (GP15).
Frequência de operação de 400 kHz.

GPIO Interface:
Configurado para controlar os LEDs (GP11, GP12, GP13) e o botão (GP22).

      Comandos e registos utilizados
I2C:
i2c_init(I2C_INTERFACE, 400 * 1000): Inicializa a interface I2C com frequência de 400 kHz.
gpio_set_function(SDA_PIN, GPIO_FUNC_I2C): Configura o pino SDA para função I2C.
gpio_set_function(SCL_PIN, GPIO_FUNC_I2C): Configura o pino SCL para função I2C.
GPIO:
gpio_init(btn_pin): Inicializa o pino do botão.
gpio_set_dir(btn_pin, GPIO_IN): Configura o pino do botão como entrada.
gpio_pull_up(btn_pin): Habilita o pull-up interno no pino do botão.
gpio_set_irq_enabled_with_callback(btn_pin, GPIO_IRQ_EDGE_FALL, true, &HANDLER_BTN_irq): Configura a interrupção no pino do botão.
Display OLED SSD1306:
ssd1306_init(ssd, OLED_WIDTH, OLED_HEIGHT, false, OLED_ADDR, I2C_INTERFACE): Inicializa o display OLED.
ssd1306_clear(ssd): Limpa o display.
ssd1306_draw_string(ssd, "text", x, y): Desenha uma string no display.
ssd1306_send_data(ssd): Envia os dados para o display.
LEDs:
gpio_put(LED_RED_PIN, 1): Liga o LED vermelho.
gpio_put(LED_GREEN_PIN, 1): Liga o LED verde.
gpio_put(LED_BLUE_PIN, 1): Liga o LED azul.

      Descrição da pinagem usada
GP14 (SDA): Pino de dados para comunicação I2C com o display OLED.
GP15 (SCL): Pino de clock para comunicação I2C com o display OLED.
GP22: Pino de entrada para o botão do joystick.
GP11: Pino de saída para o LED verde.
GP12: Pino de saída para o LED azul.
GP13: Pino de saída para o LED vermelho.

3.    Especificação do Hardware 
3.1.   Descrição das funcionalidades

Aplicação:
Controle principal do sistema.
Gerencia a contagem regressiva e a exibição da umidade do solo.
Define a cor dos LEDs com base na umidade simulada.
Responde às interrupções do botão do joystick.
Drivers:
I2C Driver: Gerencia a comunicação I2C com o display OLED.
GPIO Driver: Controla os LEDs e o botão do joystick.
SSD1306 Driver: Controla o display OLED (limpar tela, exibir texto, números, etc.).
Hardware Abstraction:
Inicializa e configura o hardware (I2C, GPIO, display OLED).
Fornece funções de alto nível para interagir com o hardware.
Hardware (MCU):
Executa o firmware no Raspberry Pi Pico.
Fornece acesso aos periféricos (GPIO, I2C, Timers).
3.2.    Definição das Variáveis
Variáveis Globais:
ssd1306_t display: Estrutura que armazena as configurações do display OLED.
static volatile uint32_t last_time_btn_sw: Armazena o tempo do último evento do botão (para debouncing).
static bool show_moisture: Flag que indica se a umidade deve ser exibida.
Variáveis Locais:
uint16_t moisture: Armazena o valor simulado da umidade do solo.
char moisture_str[20]: String formatada para exibir a umidade no display.
char num_str[2]: String para exibir números durante a contagem regressiva.
3.3.   Fluxograma
3.4.     INICIALIZAÇÃO

Inicialização do Hardware:
stdio_init_all(): Inicializa a comunicação serial (para depuração).
CONFIGURE_i2c(): Configura a interface I2C.
CONFIGURE_DISPLAY(&display): Configura o display OLED.
CONFIGURE_BTN(BTN_JOYSTICK): Configura o botão do joystick.
CONFIGURE_LEDS(): Configura os pinos dos LEDs.
Exibição da Mensagem Inicial:
O display OLED exibe a mensagem "Clique para iniciar".
Entrada no Loop Principal:
O sistema aguarda o pressionamento do botão para iniciar a contagem regressiva.
3.5.       Configurações dos Registros
I2C:
i2c_init(I2C_INTERFACE, 400 * 1000): Configura a frequência do I2C para 400 kHz.
gpio_set_function(SDA_PIN, GPIO_FUNC_I2C): Configura o pino SDA para função I2C.
gpio_set_function(SCL_PIN, GPIO_FUNC_I2C): Configura o pino SCL para função I2C.
GPIO:
gpio_init(btn_pin): Inicializa o pino do botão.
gpio_set_dir(btn_pin, GPIO_IN): Configura o pino do botão como entrada.
gpio_pull_up(btn_pin): Habilita o pull-up interno no pino do botão.
gpio_set_irq_enabled_with_callback(btn_pin, GPIO_IRQ_EDGE_FALL, true, &HANDLER_BTN_irq): Configura a interrupção no pino do botão.
Display OLED:
ssd1306_init(ssd, OLED_WIDTH, OLED_HEIGHT, false, OLED_ADDR, I2C_INTERFACE): Inicializa o display OLED.
ssd1306_clear(ssd): Limpa o display.
ssd1306_draw_string(ssd, "text", x, y): Desenha uma string no display.
3.6.       Estrutura e Formato dos Dados
Dados do Display OLED:
O display OLED usa uma matriz de pixels de 128x64.
Dados de Umidade:
A umidade é simulada como um valor inteiro entre 1500 e 3000.
O valor é exibido no display como uma string formatada (ex: "Umidade: 2000").
Dados dos LEDs:
Cada LED (vermelho, verde, azul) é controlado por um pino GPIO.
O estado do LED é definido como 0 (desligado) ou 1 (ligado).
4.    METODOLOGIA
Escolha do Hardware:
Microcontrolador: Raspberry Pi Pico.
Display: OLED SSD1306 (128x64 pixels, interface I2C).
LEDs: RGB (vermelho, verde, azul).
Botão: Botão do joystick (GP22).
Definição das Funcionalidades do Software:
O software foi projetado para:
Inicializar o hardware (I2C, GPIO, display OLED).
Responder ao pressionamento do botão.
Realizar uma contagem regressiva de 5 a 0.
Simular a leitura da umidade do solo.
Exibir a umidade no display OLED.
Controlar a cor dos LEDs com base na umidade.
Inicialização da IDE:
A IDE utilizada foi o Visual Studio Code com a extensão Pico SDK.
O ambiente foi configurado para compilar e depurar código para o Raspberry Pi Pico.
Programação na IDE:
O código foi desenvolvido em C, utilizando as bibliotecas do Pico SDK.
Foram implementadas funções para:
Configuração do I2C e GPIO.
Controle do display OLED.
Leitura do botão e controle dos LEDs.
Simulação da umidade do solo.
Depuração:
A depuração foi realizada usando:
printf para exibir mensagens de depuração via serial.
Testes incrementais de cada funcionalidade (ex: verificar se o botão está sendo lido corretamente, se o display exibe os números corretamente...).
Ajustes no código para corrigir erros e melhorar a eficiência.
5.     Testes de Validação
Teste de Inicialização:
Verificou-se se o hardware (I2C, GPIO, display OLED) era inicializado corretamente.
Resultado: O display exibiu a mensagem inicial "Clique para iniciar".
Teste do Botão:
Verificou-se se o botão do joystick era detectado corretamente e se a interrupção era acionada.
Resultado: O sistema respondeu ao pressionamento do botão, iniciando a contagem regressiva.
Teste da Contagem Regressiva:
Verificou-se se os números de 5 a 0 eram exibidos corretamente no display OLED.
Resultado: A contagem regressiva funcionou conforme o esperado.
Teste de Simulação de Umidade:
Verificou-se se o valor simulado da umidade era gerado corretamente e exibido no display.
Resultado: O valor da umidade foi exibido corretamente no formato "Umidade: XXXX".
Teste dos LEDs:
Verificou-se se a cor dos LEDs mudava conforme o valor da umidade:
Vermelho para umidade < 1800.
Azul para umidade entre 1800 e 2200.
Verde para umidade >= 2200.
Resultado: Os LEDs mudaram de cor conforme o esperado.
Teste de Estresse:
O sistema foi testado repetidamente para garantir que não houvesse falhas após múltiplos ciclos de operação.
Resultado: O sistema funcionou de forma consistente sem falhas.

Referências 
Raspberry Pi Pico Documentation:
Documentação oficial do Raspberry Pi Pico, incluindo guias de hardware e software.
Disponível em: https://www.raspberrypi.com/documentation/microcontrollers/
SSD1306 OLED Display Datasheet:
Especificações técnicas do display OLED SSD1306.
Disponível em: https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
Pico SDK Examples:
Exemplos de código fornecidos pelo Pico SDK para configuração de I2C, GPIO e interrupções.
Disponível em: https://github.com/raspberrypi/pico-examples
Fulton, J., et al. (2021). Precision agriculture technologies positively contributing to GHG emissions mitigation, farm productivity and economics. Sustainability, 13(4), 2125. doi:10.3390/su13042125. Disponível em: https://www.mdpi.com/2071-1050/13/4/2125
Jones, S. B., et al. (2018). Application of soil moisture sensors in agriculture: A review. Vadose Zone Journal, 17(1), 1-15. doi:10.2136/vzj2017.10.0178. Disponível em: https://acsess.onlinelibrary.wiley.com/doi/full/10.2136/vzj2017.10.0178
