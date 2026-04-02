#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h" 
#include "hardware/i2c.h"
#include "hardware/pwm.h"    
#include "ssd1306_font.h" 

// --- CONFIGURAÇÕES DO PROJETO ---
#define I2C_PORT i2c1
#define BUTTON_A 5
#define BUTTON_B 6
#define BUZZER_PIN 21 

typedef enum { MENU, JOGANDO, GAMEOVER } Estado;
volatile Estado estado_atual = MENU;
volatile int resposta_usuario = -1;

// --- PROTÓTIPOS ---
void play_tone(uint freq, uint duration_ms);
void gpio_callback(uint gpio, uint32_t events);

// --- FUNÇÃO DE SOM (REQUISITO: ATUADORES/BUZZER) ---
void play_tone(uint freq, uint duration_ms) {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint32_t divider = 125000000 / (freq * 4096);
    pwm_set_clkdiv(slice_num, divider);
    pwm_set_wrap(slice_num, 4095);
    pwm_set_gpio_level(BUZZER_PIN, 2048);
    pwm_set_enabled(slice_num, true);
    sleep_ms(duration_ms);
    pwm_set_enabled(slice_num, false);
}

// --- INTERRUPÇÕES (REQUISITO: INTERRUPÇÃO/BOTÕES) ---
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

// --- FUNÇÕES DO DISPLAY SSD1306 ---
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

// --- BANCO DE QUESTÕES (AUTORIA PRÓPRIA) ---
typedef struct { char pergunta[16]; char op1[12]; char op2[12]; int correta; } Questao;
Questao quiz[] = {
    {"7 MAIS 8", "A:15", "B:13", 0}, {"9 VEZES 3", "A:24", "B:27", 1},
    {"H2O EH", "A:AGUA", "B:SAL", 0}, {"PLANETA VERM.", "A:MARTE", "B:VENUS", 0}
};

int main() {
    stdio_init_all();
    srand(to_ms_since_boot(get_absolute_time()));

    // Inicialização IoT (Requisito: Comunicação sem fio) [cite: 18, 20]
    cyw43_arch_init(); 
    
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(14, GPIO_FUNC_I2C); gpio_set_function(15, GPIO_FUNC_I2C);
    gpio_pull_up(14); gpio_pull_up(15);
    SSD1306_init();
    
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    
    gpio_init(BUTTON_A); gpio_set_dir(BUTTON_A, GPIO_IN); gpio_pull_up(BUTTON_A);
    gpio_init(BUTTON_B); gpio_set_dir(BUTTON_B, GPIO_IN); gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);

    uint8_t canvas[1024];
    int q_atual = 0, timer = 128, acertos = 0;

    while (true) {
        memset(canvas, 0, 1024);

        if (estado_atual == MENU) {
            WriteString(canvas, 25, 16, "BRAND QUIZ"); // Nome do Jogo atualizado
            WriteString(canvas, 10, 40, "APERTE A: START");
            acertos = 0;
        } 
        else if (estado_atual == JOGANDO) {
            WriteString(canvas, 5, 8, quiz[q_atual].pergunta);
            WriteString(canvas, 5, 32, quiz[q_atual].op1);
            WriteString(canvas, 5, 48, quiz[q_atual].op2);

            // Barra de tempo (Representação visual)
            for(int i=0; i<timer; i++) canvas[i] = 0x01; 
            timer--;

            if (timer <= 0) { 
                estado_atual = GAMEOVER; 
                play_tone(200, 500); 
            }

            if (resposta_usuario != -1) {
                if (resposta_usuario == quiz[q_atual].correta) {
                    play_tone(1000, 100); 
                    acertos++;
                    timer = 128; 
                    q_atual = (q_atual + 1) % 4;
                } else {
                    estado_atual = GAMEOVER;
                    play_tone(150, 600); 
                }
                resposta_usuario = -1;
                sleep_ms(300);
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
        sleep_ms(40);
    }
}