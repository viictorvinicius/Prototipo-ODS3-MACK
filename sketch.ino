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