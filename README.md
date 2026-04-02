# 🎮 Projeto: Brand Quiz - Desafio Multidisciplinar Embarcado

Este projeto consiste em um sistema de quiz interativo e multidisciplinar de alta performance, desenvolvido para rodar no microcontrolador **RP2040**. Utilizando a placa **BitDogLab**, o sistema desafia o usuário com perguntas em tempo real, integrando interface visual, lógica de tempo e feedbacks sensoriais para uma experiência educativa imersiva.

## 🚀 Funcionalidades
- **Interface Visual Dinâmica:** Exibição de menus, perguntas e opções no Display OLED SSD1306.
- **Cronômetro Regressivo:** Barra visual no display que monitora o tempo de resposta do jogador de forma fluida.
- **Feedback Sonoro (PWM):** Tons distintos emitidos pelo Buzzer para indicar acertos, erros ou fim de tempo (Game Over).
- **Controle via Interrupção (IRQ):** Uso de interrupções de hardware nos botões A e B para garantir resposta imediata e sem atrasos aos comandos.
- **Pronto para IoT:** Inicialização da pilha Wi-Fi para futura integração com rankings online e telemetria remota.

## 🛠️ Hardware Utilizado (BitDogLab)
- **Microcontrolador:** [Raspberry Pi Pico W (RP2040)](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)
- **Placa de Expansão:** BitDogLab (Uso Obrigatório para integração dos periféricos)
- **Display:** OLED 128x64 (Interface I2C)
- **Entradas:** Botões A e B (Configurados com Pull-up interno e Debounce)
- **Atuador Sonoro:** Buzzer Piezoelétrico (Controlado via PWM para diferentes frequências)

## 📂 Estrutura do Repositório
- `/src`: Código-fonte principal desenvolvido em C/C++.
- `/docs`: Relatório técnico detalhado, fluxogramas e documentação de suporte.
- `/include`: Arquivos de cabeçalho (`.h`) e definições de fontes gráficas (`ssd1306_font.h`).
- `CMakeLists.txt`: Arquivo de configuração para compilação via SDK.

## 🔧 Como Instalar e Rodar
1. Configure o ambiente de desenvolvimento **Pico SDK** em sua máquina.
2. Clone este repositório:
   ```bash
   git clone [https://github.com/SEU_USUARIO/NOME_DO_REPOSITORIO.git](https://github.com/SEU_USUARIO/NOME_DO_REPOSITORIO.git)
3.Compile o projeto utilizando CMake:
mkdir build && cd build
cmake ..
make
4.Conecte a BitDogLab ao computador segurando o botão BOOTSEL.
5.Arraste o arquivo .uf2 gerado na pasta build para a unidade de disco da placa.
6.O jogo iniciará automaticamente exibindo a tela principal do Brand Quiz.
💻 Simulação jogo
Você pode visualizar a lógica de estados, a interface gráfica e o comportamento dos botões através do link abaixo:
link: 
📄 Relatório Técnico
O relatório detalhado contendo os objetivos, requisitos funcionais, arquitetura de hardware e o fluxograma de decisão está disponível na pasta /RELATÓRIO TÉCNICO_PROJETO BRAND QUIZ.PDF.
