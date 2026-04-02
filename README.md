RELATÓRIO TÉCNICO
PROJETO BRAND QUIZ
a. Apresentação
O projeto Brand Quiz é um sistema embarcado educativo desenvolvido para a plataforma BitDogLab (RP2040). O desafio abordado é a criação de uma ferramenta de aprendizado interativa que utilize recursos de hardware modernos para tornar o estudo multidisciplinar mais dinâmico. A motivação reside em demonstrar como microcontroladores podem ser aplicados em gamificação, unindo processamento de dados, interface visual e conectividade IoT.
b. Objetivos
Geral: Desenvolver um jogo de perguntas e respostas funcional e original utilizando a linguagem C e a placa BitDogLab.
Específicos:
Implementar uma interface gráfica estável no display SSD1306.
Utilizar interrupções de hardware (IRQ) para controle preciso dos botões.
Integrar feedback sonoro via PWM no Buzzer.
Configurar a pilha de rede Wi-Fi para futuras expansões de ranking online (IoT).
c. Requisitos Funcionais
Entrada: O sistema deve captar comandos do usuário através dos botões A e B via interrupção.
Processamento: O software deve gerenciar uma máquina de estados (Menu, Jogo, Fim) e validar as respostas em tempo real.
Saída Visual: Exibição de perguntas, opções e barra de tempo no display OLED.
Saída Sonora: Emissão de tons distintos para acertos e erros através do Buzzer.
Restrição: O tempo para resposta é limitado, indicado por uma barra visual que decresce.
d. Arquitetura de Hardware
O projeto utiliza a placa BitDogLab baseada no microcontrolador RP2040. Os componentes principais são:
Processador: Dual-core ARM Cortex-M0+ (RP2040).
Display OLED (I2C): Interface gráfica para interação com o usuário.
Botões A e B (GPIO): Atuam como dispositivos de entrada configurados com resistores de pull-up internos.
Buzzer (PWM): Atuador para feedback sonoro.
Módulo Wi-Fi (CYW43): Responsável pela conectividade sem fio.
e. Arquitetura do Firmware
O firmware foi desenvolvido em Linguagem C utilizando o Pico SDK. A lógica é estruturada em:
Núcleo Principal: Um laço while(true) que processa a atualização do display e a lógica da máquina de estados.
Sistema de Interrupções: As entradas dos botões são tratadas fora do fluxo principal (IRQ), garantindo que nenhum clique seja perdido.
Módulo de Som: Utiliza o periférico PWM para gerar frequências específicas sem bloquear o processador.
Gerenciamento de Memória: Uso de buffers para renderização de texto no display.
f. Fluxograma
O fluxo inicia na tela de Menu. Ao detectar a interrupção do Botão A, o estado muda para Jogando. O sistema sorteia uma pergunta e inicia o decremento do Timer. Se o usuário pressionar um botão, a interrupção valida a resposta: se correta, reinicia o timer e avança; se incorreta ou o tempo acabar, muda para Game Over.
g. Indicação do uso de IA
Para a elaboração deste trabalho, a Inteligência Artificial Gemini foi utilizada como ferramenta de suporte para:
Estruturação da máquina de estados em C.
Auxílio na integração da biblioteca hardware/pwm.h para o Buzzer.
Refinamento do texto deste relatório técnico e sugestão de lógica para o cronômetro visual.
h. Conclusão
O projeto Brand Quiz cumpre todos os requisitos técnicos exigidos pelo programa EmbarcaTech. A implementação demonstrou que o RP2040 é altamente capaz de gerenciar múltiplas interfaces (I2C, PWM, GPIO) simultaneamente. Como melhoria futura, planeja-se a implementação completa de um protocolo MQTT para registrar as pontuações em um banco de dados na nuvem.

