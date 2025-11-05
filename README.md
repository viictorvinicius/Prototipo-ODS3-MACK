🤖 Sistema IoT de Lembrete e Controle de Medicamentos

Projeto de um Sistema IoT para lembrete de medicamentos, focado no ODS 3 (Saúde e Bem-Estar). O protótipo detecta a presença do usuário (via sensor PIR) e, no horário correto, dispara alertas locais (LED/Buzzer) e remotos (MQTT).

Este repositório contém toda a documentação, diagramas e software necessários para a reprodução do projeto.

🚀 Como Replicar (Simulação no Wokwi)

Você pode testar este projeto em 2 minutos, sem precisar de hardware, usando o Wokwi.

Acesse o Wokwi: Vá para wokwi.com e crie um novo projeto ESP32.

Arquivo Principal (sketch.ino):

Copie o conteúdo do sketch.ino (disponível neste repositório, na seção "Software") e cole no editor principal do Wokwi.

Adicione os Arquivos de Simulação:

Crie um novo arquivo no Wokwi chamado library.txt.

Crie um novo arquivo no Wokwi chamado diagram.json.

Copie os conteúdos abaixo para dentro desses arquivos:

<details>
<summary>Clique para ver o conteúdo do <code>library.txt</code></summary>

PubSubClient


</details>

<details>
<summary>Clique para ver o conteúdo do <code>diagram.json</code> (Hardware Simulado)</summary>

{
  "version": 1,
  "author": "Gemini",
  "editor": "Wokwi",
  "parts": [
    { "type": "wokwi-esp32-devkit-v1", "id": "esp", "top": -100, "left": 0 },
    {
      "type": "wokwi-pir-motion-sensor",
      "id": "pir",
      "top": -110,
      "left": 200
    },
    {
      "type": "wokwi-led",
      "id": "led",
      "top": -10,
      "left": 200,
      "attrs": { "color": "red" }
    },
    {
      "type": "wokwi-resistor",
      "id": "r1",
      "top": -10,
      "left": 250,
      "attrs": { "value": "220" }
    },
    { "type": "wokwi-buzzer", "id": "bz1", "top": -10, "left": 350 }
  ],
  "connections": [
    ["esp:TX", "$serial:RX", "", ""],
    ["esp:RX", "$serial:TX", "", ""],
    ["esp:GND", "pir:GND", "black", ["v0"]],
    ["esp:3V3", "pir:VCC", "red", ["v0"]],
    ["pir:OUT", "esp:D27", "green", ["v0"]],
    ["led:C", "esp:GND", "black", ["v0"]],
    ["led:A", "r1:1", "red", ["h0"]],
    ["r1:2", "esp:D25", "green", ["v0"]],
    ["bz1:1", "esp:D26", "green", ["v0"]],
    ["bz1:2", "esp:GND", "black", ["h0", "v0"]]
  ]
}


</details>

Execute!

Altere o HORA_MEDICAMENTO e MINUTO_MEDICAMENTO no sketch.ino para 1-2 minutos no futuro.

Clique no botão "Play" (Verde).

Quando der o horário, clique no Sensor PIR na simulação para disparar o alerta.

🎯 i) Funcionalidades Principais

Alerta Local: O sistema dispara um alerta visual (LED) e sonoro (Buzzer) no horário programado.

Detecção de Presença: O alerta só é disparado se o usuário for detectado pelo sensor PIR, garantindo que o lembrete seja interativo.

Lógica de Tempo Real: O ESP32 usa o protocolo NTP (via Wi-Fi) para obter a hora exata da internet, eliminando a necessidade de um relógio de hardware (RTC).

Notificação Remota (IoT): O sistema envia mensagens via MQTT para um broker na nuvem, permitindo que um cuidador receba notificações sobre:

Movimento Detectado

Alerta de Medicacao Ativado

⚙️ iii) Hardware e Montagem

Componentes Utilizados

Plataforma de Desenvolvimento: 1x ESP32 DevKit V1

Sensor: 1x Sensor de Presença PIR HC-SR501

Atuadores:

1x LED Vermelho 5mm

1x Buzzer Piezoelétrico Ativo 5V

Outros:

1x Resistor de 220Ω (para o LED)

1x Protoboard

Jumpers

Diagrama de Montagem (Fritzing)

O diagrama de montagem (prototipo.fzz) demonstra as conexões físicas do projeto.

(Insira aqui o print do seu Fritzing, mostrando as conexões corretas em D27, D25 e D26)

💻 ii) Software e Firmware

O firmware foi desenvolvido em C++/Arduino e é composto por 4 lógicas principais:

Gerenciador de Wi-Fi: Conecta o ESP32 à rede.

Cliente NTP: Busca a hora atual (com fuso UTC-3).

Cliente MQTT: Conecta ao broker e publica mensagens.

Lógica de Alerta: Lê o sensor (com lógica de "borda de subida" para evitar spam) e aciona os atuadores.

<details>
<summary>Clique para ver o código-fonte completo (<code>sketch.ino</code>)</summary>

/*
 * Sistema IoT de Lembrete de Medicamentos
 * Autores: Geovana Hungria, Victor Vinicius, Zenaide Silva, Leandro Carlos
 * Plataforma: ESP32 (Simulado no Wokwi)
 *
 * Este código implementa um sistema de alerta de medicação
 * que usa um sensor PIR para detecção de presença e
 * publica alertas via MQTT.
 */

#include <WiFi.h>          // Para conectar o ESP32 ao Wi-Fi
#include <PubSubClient.h>  // Para enviar mensagens MQTT
#include <time.h>          // Para verificar a hora

// --- 1. CONFIGURAÇÕES ---
// Na simulação Wokwi, use "Wokwi-GUEST" como SSID e deixe a senha em branco.
// Em um ESP32 real, coloque sua rede Wi-Fi aqui.
const char* SSID = "Wokwi-GUEST"; 
const char* PASSWORD = "";        

// Defina o horário do alarme (formato 24h)
const int HORA_MEDICAMENTO = 14;  
const int MINUTO_MEDICAMENTO = 30; 
// ----------------------------------------

// --- 2. PINOS (Hardware) ---
const int PINO_PIR = 27;     // Sensor PIR
const int PINO_LED = 25;     // LED vermelho
const int PINO_BUZZER = 26;  // Buzzer

// --- 3. MQTT (Comunicação) ---
const char* MQTT_BROKER = "broker.hivemq.com"; // Broker público HiveMQ
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC_EVENTOS = "projetoSaude/eventos"; // Tópico para publicar

// --- 4. HORA (NTP) ---
const char* NTP_SERVER = "pool.ntp.org";
const long  GMT_OFFSET_SEC = -3 * 3600; // Offset UTC-3 (Horário de Brasília)
const int   DAYLIGHT_OFFSET_SEC = 0;    // Sem horário de verão

// --- 5. Variáveis Globais ---
WiFiClient espClient;
PubSubClient client(espClient);
bool movimentoDetectadoHoje = false; // Controla se o alerta já foi disparado hoje
bool pirEstadoAnterior = LOW;        // Controla o estado do PIR para evitar spam

// --- 6. SETUP (Configuração Inicial) ---
void setup() {
  Serial.begin(115200);
  pinMode(PINO_PIR, INPUT);
  pinMode(PINO_LED, OUTPUT);
  pinMode(PINO_BUZZER, OUTPUT);
  digitalWrite(PINO_LED, LOW);
  digitalWrite(PINO_BUZZER, LOW);
  
  setup_wifi();
  client.setServer(MQTT_BROKER, MQTT_PORT);
  configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
  Serial.println("Sistema iniciado. Aguardando movimento.");
}

// --- 7. LOOP (Rotina Principal) ---
void loop() {
  if (!client.connected()) {
    reconnect_mqtt();
  }
  client.loop(); 
  verificarAlertaMedicamento();
  delay(100); 
}

// --- 8. LÓGICA DO ALERTA ---
void verificarAlertaMedicamento() {
  int estadoPIR = digitalRead(PINO_PIR);

  // Lógica de "Borda de Subida" (só dispara na transição de LOW para HIGH)
  if (estadoPIR == HIGH && pirEstadoAnterior == LOW) {
    Serial.println("Movimento Detectado!");
    pirEstadoAnterior = HIGH; 
    client.publish(MQTT_TOPIC_EVENTOS, "Movimento Detectado");
    
    if (eHoraDoMedicamento()) {
      Serial.println("HORA DO MEDICAMENTO!");
      dispararAlertaVisualSonoro();
      client.publish(MQTT_TOPIC_EVENTOS, "Alerta de Medicacao Ativado");
      movimentoDetectadoHoje = true; 
    }
  } 
  
  // Rearma o sensor quando o movimento para
  else if (estadoPIR == LOW && pirEstadoAnterior == HIGH) {
    Serial.println("Movimento Parou.");
    pirEstadoAnterior = LOW; 
  }
}

// Função auxiliar para disparar os atuadores
void dispararAlertaVisualSonoro() {
  Serial.println("Ativando Alerta Sonoro e Visual por 5 segundos...");
  digitalWrite(PINO_LED, HIGH);
  for (int i = 0; i < 10; i++) {
    digitalWrite(PINO_BUZZER, HIGH);
    delay(250);
    digitalWrite(PINO_BUZZER, LOW);
    delay(250);
  }
  digitalWrite(PINO_LED, LOW);
  digitalWrite(PINO_BUZZER, LOW);
  Serial.println("Alertas desativados.");
}

// Função auxiliar para verificar a hora
bool eHoraDoMedicamento() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Falha ao obter hora");
    return false;
  }
  
  // Reseta o flag à meia-noite
  if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0) {
      movimentoDetectadoHoje = false;
  }

  // Retorna verdadeiro apenas se a hora bate E o alarme não foi disparado
  return (timeinfo.tm_hour == HORA_MEDICAMENTO && 
          timeinfo.tm_min == MINUTO_MEDICAMENTO && 
          !movimentoDetectadoHoje);
}

// --- Funções Auxiliares de Rede ---
void setup_wifi() {
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
}

void reconnect_mqtt() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT (HiveMQ)...");
    String clientId = "ESP32-ProjetoSaude-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
      client.publish(MQTT_TOPIC_EVENTOS, "ESP32 Conectado ao Broker");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}


</details>

📡 iv) e v) Protocolos de Comunicação (TCP/IP e MQTT)

Interface de Hardware: Os componentes são conectados aos pinos digitais (GPIO) do ESP32.

D27 (INPUT): Recebe o sinal do Sensor PIR.

D25 (OUTPUT): Envia sinal para o LED.

D26 (OUTPUT): Envia sinal para o Buzzer.

Protocolo de Rede (TCP/IP): O ESP32 usa sua pilha TCP/IP integrada para se conectar ao Wi-Fi, permitindo a comunicação com a internet.

Protocolo de Aplicação (MQTT): O projeto utiliza MQTT para a comunicação IoT.

Broker (Servidor): broker.hivemq.com (Público)

Porta: 1883

Tópico (Topic): projetoSaude/eventos

Mensagens Publicadas:

Movimento Detectado: Enviada quando o PIR é acionado.

Alerta de Medicacao Ativado: Enviada quando o PIR é acionado no horário do alarme.

🌟 Melhorias Futuras

Botão de Confirmação: Adicionar um botão para o paciente pressionar, confirmando que tomou o remédio. Isso enviaria uma terceira mensagem MQTT (Medicamento Tomado).

Lógica Offline (RTC): Adicionar um módulo de relógio (RTC DS3231) como backup. Se o Wi-Fi cair, o sistema usaria o RTC para disparar o alarme local.

Interface de Configuração: Criar uma pequena página web (hospedada no próprio ESP32) para permitir que o cuidador configure os horários dos alarmes pelo celular.

🧑‍💻 Autores

Geovana Hungria

Victor Vinicius

Zenaide Silva

Leandro Carlos

📜 Licença

Este projeto está licenciado sob a Licença MIT.
