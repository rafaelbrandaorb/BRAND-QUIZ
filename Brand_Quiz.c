#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" 
#include "hardware/i2c.h"
#include "hardware/pwm.h"    
#include "hardware/gpio.h"
#include "ssd1306_font.h" 

// --- CONFIGURAÇÕES DE HARDWARE ---
#define I2C_PORT i2c1
#define BUTTON_A 5
#define BUTTON_B 6
#define BUZZER_PIN 21 
#define LED_RED 13
#define LED_GREEN 11

// Notas musicais (Hz)
#define NOTE_C4 262
#define NOTE_E4 330
#define NOTE_G4 392
#define NOTE_C5 523
#define NOTE_A4 440

typedef enum { MENU, JOGANDO, GAMEOVER } Estado;
volatile Estado estado_atual = MENU;
volatile int resposta_usuario = -1;

// --- FUNÇÃO DE SOM (PWM) ---
void play_tone(uint freq, uint duration_ms) {
    if (freq == 0) { sleep_ms(duration_ms); return; }
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint32_t divider = 125000000 / (freq * 4096);
    pwm_set_clkdiv(slice_num, divider);
    pwm_set_wrap(slice_num, 4095);
    pwm_set_gpio_level(BUZZER_PIN, 2048);
    pwm_set_enabled(slice_num, true);
    sleep_ms(duration_ms);
    pwm_set_enabled(slice_num, false);
}

// Melodia temática de fundo
void tocar_musica_tema() {
    int notas[] = {NOTE_C4, NOTE_G4, NOTE_E4, NOTE_C5};
    for (int i = 0; i < 4; i++) {
        play_tone(notas[i], 150);
        sleep_ms(30);
    }
}

// --- INTERRUPÇÕES ---
void gpio_callback(uint gpio, uint32_t events) {
    if (estado_atual == MENU && gpio == BUTTON_A) {
        estado_atual = JOGANDO;
    } else if (estado_atual == JOGANDO) {
        if (gpio == BUTTON_A) resposta_usuario = 0;
        else if (gpio == BUTTON_B) resposta_usuario = 1;
    } else if (estado_atual == GAMEOVER && gpio == BUTTON_A) {
        estado_atual = MENU;
    }
}

// --- DISPLAY SSD1306 ---
static void WriteChar(uint8_t *buf, int16_t x, int16_t y, uint8_t ch) {
    if (x > 120 || y > 56) return;
    y = y / 8; ch = toupper(ch);
    int idx = 0;
    if (ch >= 'A' && ch <= 'Z') idx = ch - 'A' + 1;
    else if (ch >= '0' && ch <= '9') idx = ch - '0' + 27;
    else if (ch == '?') idx = 37;
    for (int i = 0; i < 8; i++) buf[y * 128 + x + i] = font[idx * 8 + i];
}

static void WriteString(uint8_t *buf, int16_t x, int16_t y, char *str) {
    while (*str) { WriteChar(buf, x, y, *str++); x += 8; }
}

void SSD1306_send_buf(uint8_t buf[], int buflen) {
    uint8_t *temp_buf = malloc(buflen + 1);
    if (!temp_buf) return;
    temp_buf[0] = 0x40;
    memcpy(temp_buf + 1, buf, buflen);
    i2c_write_blocking(I2C_PORT, 0x3C, temp_buf, buflen + 1, false);
    free(temp_buf);
}

void SSD1306_init() {
    uint8_t cmds[] = {0xAE, 0x20, 0x00, 0xA1, 0xC8, 0xA8, 63, 0xD3, 0x00, 0xDA, 0x12, 0x81, 0xFF, 0xA4, 0xA6, 0x8D, 0x14, 0xAF};
    for (int i = 0; i < sizeof(cmds); i++) {
        uint8_t b[2] = {0x80, cmds[i]};
        i2c_write_blocking(I2C_PORT, 0x3C, b, 2, false);
    }
}

// --- QUESTÕES ---
typedef struct { char pergunta[16]; char op1[12]; char op2[12]; int correta; } Questao;
Questao quiz[] = {
    {"7 MAIS 8", "A:15", "B:13", 0}, {"9 VEZES 3", "A:24", "B:27", 1},
    {"RAIZ DE 64", "A:8", "B:6", 0}, {"2 ELEVADO A 3", "A:6", "B:8", 1},
    {"METADE DE 50", "A:25", "B:20", 0}, {"H2O EH", "A:AGUA", "B:SAL", 0},
    {"PLANETA VERM.", "A:MARTE", "B:VENUS", 0}, {"RP2040 EH UM", "A:MICROCONT.", "B:SENSOR", 0},
    {"MEMORIA RAM EH", "A:VOLATIL", "B:FIXA", 0}, {"I2C USA QUANTOS", "A:2 FIOS", "B:4 FIOS", 0},
    {"SOL EH UMA", "A:ESTRELA", "B:PLANETA", 0}, {"PARA RESPIRAR", "A:PULMAO", "B:RINS", 0},
    {"CAPITAL BR", "A:BSB", "B:RIO", 0}, {"ONDE FICA SP", "A:SUL", "B:SUDESTE", 1},
    {"MAIOR PAIS", "A:RUSSIA", "B:CHINA", 0}, {"BRASIL FICA NA", "A:AM.SUL", "B:EUROPA", 0},
    {"DESCOB. BRASIL", "A:1500", "B:1822", 0}, {"SINON. BELO", "A:LINDO", "B:FEIO", 0},
    {"PLURAL DE PAO", "A:PAES", "B:PAONS", 0}, {"COR DO CEU", "A:AZUL", "B:VERDE", 0}
};

int main() {
    stdio_init_all();
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C); gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14); gpio_pull_up(15);
    SSD1306_init();
    
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    
    // Configura LEDs RGB
    gpio_init(LED_RED); gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_init(LED_GREEN); gpio_set_dir(LED_GREEN, GPIO_OUT);
    
    gpio_init(BUTTON_A); gpio_set_dir(BUTTON_A, GPIO_IN); gpio_pull_up(BUTTON_A);
    gpio_init(BUTTON_B); gpio_set_dir(BUTTON_B, GPIO_IN); gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);

    uint8_t canvas[1024];
    int q_atual = 0, timer = 128, acertos = 0;

    while (true) {
        memset(canvas, 0, 1024);

        if (estado_atual == MENU) {
            WriteString(canvas, 25, 16, "BRAND QUIZ");
            WriteString(canvas, 10, 40, "APERTE A: START");
            gpio_put(LED_RED, 0); gpio_put(LED_GREEN, 0);
            acertos = 0;
            static uint32_t last_m = 0;
            if (to_ms_since_boot(get_absolute_time()) - last_m > 6000) {
                tocar_musica_tema();
                last_m = to_ms_since_boot(get_absolute_time());
            }
        } 
        else if (estado_atual == JOGANDO) {
            WriteString(canvas, 5, 8, quiz[q_atual].pergunta);
            WriteString(canvas, 5, 32, quiz[q_atual].op1);
            WriteString(canvas, 5, 48, quiz[q_atual].op2);

            for(int i=0; i<timer; i++) canvas[i] = 0x01; 
            timer--;

            if (timer <= 0) { 
                estado_atual = GAMEOVER; 
                gpio_put(LED_RED, 1);
                play_tone(150, 500); 
            }

            if (resposta_usuario != -1) {
                if (resposta_usuario == quiz[q_atual].correta) {
                    gpio_put(LED_GREEN, 1);
                    play_tone(1000, 150); 
                    acertos++;
                    timer = 128; 
                    q_atual = (q_atual + 1) % 20;
                    sleep_ms(200);
                    gpio_put(LED_GREEN, 0);
                } else {
                    estado_atual = GAMEOVER;
                    gpio_put(LED_RED, 1);
                    play_tone(100, 600); 
                }
                resposta_usuario = -1;
            }
        } 
        else if (estado_atual == GAMEOVER) {
            char score_str[16];
            sprintf(score_str, "PONTOS: %d", acertos);
            WriteString(canvas, 30, 8, "GAME OVER");
            WriteString(canvas, 30, 32, score_str);
            WriteString(canvas, 10, 48, "RECOMECAR: A");
        }

        SSD1306_send_buf(canvas, 1024);
        sleep_ms(50);
    }
}
