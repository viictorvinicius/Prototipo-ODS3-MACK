<h1>🤖 Sistema IoT de Lembrete e Controle de Medicamentos</h1>

Projeto de um Sistema IoT para lembrete de medicamentos, focado no ODS 3 (Saúde e Bem-Estar). O protótipo detecta a presença do usuário (via sensor PIR) e, no horário correto, dispara alertas locais (LED/Buzzer) e remotos (MQTT).

Este repositório contém toda a documentação, diagramas e software necessários para a reprodução do projeto.

<h1>🚀 Como Replicar (Simulação no Wokwi)</h1>

Você pode testar este projeto em 2 minutos, sem precisar de hardware, usando o Wokwi.

Acesse o Wokwi: Vá para wokwi.com e crie um novo projeto ESP32.

2. **Arquivo Principal (`sketch.ino`):**

   * Copie o conteúdo do arquivo `sketch.ino` (disponível neste repositório) e cole no editor principal do Wokwi.

3. **Adicione os Arquivos de Simulação:**

   * Crie um novo arquivo no Wokwi chamado `library.txt`. Copie o conteúdo do `library.txt` deste repositório para ele.

   * Crie um novo arquivo no Wokwi chamado `diagram.json`. Copie o conteúdo do `diagram.json` deste repositório para ele.

4. **Execute!**

   * Altere o `HORA_MEDICAMENTO` e `MINUTO_MEDICAMENTO` no `sketch.ino` para 1-2 minutos no futuro.
      * Clique no botão "Play" (Verde).
      * Quando der o horário, clique no Sensor PIR na simulação para disparar o alerta.

-----

## 🎯 I) Funcionalidades Principais

  * **Alerta Local:** O sistema dispara um alerta visual (LED) e sonoro (Buzzer) no horário programado.
  * **Detecção de Presença:** O alerta só é disparado se o usuário for detectado pelo sensor PIR, garantindo que o lembrete seja interativo.
  * **Lógica de Tempo Real:** O ESP32 usa o protocolo NTP (via Wi-Fi) para obter a hora exata da internet, eliminando a necessidade de um relógio de hardware (RTC).
  * **Notificação Remota (IoT):** O sistema envia mensagens via MQTT para um broker na nuvem, permitindo que um cuidador receba notificações sobre:
      * `Movimento Detectado`
      * `Alerta de Medicacao Ativado`

## ⚙️ II) Hardware e Montagem

### Componentes Utilizados

  * **Plataforma de Desenvolvimento:** 1x ESP32 DevKit V1
  * **Sensor:** 1x Sensor de Presença PIR HC-SR501
  * **Atuadores:**
      * 1x LED Vermelho 5mm
      * 1x Buzzer Piezoelétrico Ativo 5V
  * **Outros:**
      * 1x Resistor de 220Ω (para o LED)
      * 1x Protoboard
      * Jumpers

### Diagrama de Montagem (Fritzing)

O diagrama de montagem (`prototipo.fzz`) demonstra as conexões físicas do projeto.

*(Insira aqui o print do seu Fritzing, mostrando as conexões corretas em D27, D25 e D26)*

## 💻 III) Software e Firmware

O firmware foi desenvolvido em C++/Arduino e é composto por 4 lógicas principais:

1.  **Gerenciador de Wi-Fi:** Conecta o ESP32 à rede.
2.  **Cliente NTP:** Busca a hora atual (com fuso UTC-3).
3.  **Cliente MQTT:** Conecta ao broker e publica mensagens.
4.  **Lógica de Alerta:** Lê o sensor (com lógica de "borda de subida" para evitar spam) e aciona os atuadores.

O código-fonte completo e comentado está disponível no arquivo `sketch.ino` neste repositório.

## 📡 IV) e V) Protocolos de Comunicação (TCP/IP e MQTT)

  * **Interface de Hardware:** Os componentes são conectados aos pinos digitais (GPIO) do ESP32.
    \* `D27` (INPUT): Recebe o sinal do Sensor PIR.
    \* `D25` (OUTPUT): Envia sinal para o LED.
    \* `D26` (OUTPUT): Envia sinal para o Buzzer.
      * **Protocolo de Rede (TCP/IP):** O ESP32 usa sua pilha TCP/IP integrada para se conectar ao Wi-Fi, permitindo a comunicação com a internet.
      * **Protocolo de Aplicação (MQTT):** O projeto utiliza MQTT para a comunicação IoT.
          * **Broker (Servidor):** `broker.hivemq.com` (Público)
          * **Porta:** `1883`
          * **Tópico (Topic):** `projetoSaude/eventos`
          * **Mensagens Publicadas:**
              * `Movimento Detectado`: Enviada quando o PIR é acionado.
              * `Alerta de Medicacao Ativado`: Enviada quando o PIR é acionado no horário do alarme.

## 🌟 Melhorias Futuras

  * **Botão de Confirmação:** Adicionar um botão para o paciente pressionar, confirmando que tomou o remédio. Isso enviaria uma terceira mensagem MQTT (`Medicamento Tomado`).
  * **Lógica Offline (RTC):** Adicionar um módulo de relógio (RTC DS3231) como *backup*. Se o Wi-Fi cair, o sistema usaria o RTC para disparar o alarme local.
  * **Interface de Configuração:** Criar uma pequena página web (hospedada no próprio ESP32) para permitir que o cuidador configure os horários dos alarmes pelo celular.

## 🧑‍💻 Autores

  * Geovana Hungria
  * Victor Vinicius
  * Zenaide Silva 
  * Leandro Carlos (Professor)

## 📜 Licença

Este projeto está licenciado sob a Licença MIT.
