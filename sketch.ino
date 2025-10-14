/*
 * FarmTech Solutions - Sistema de Irrigação Inteligente
 * Projeto: Monitoramento de Solo e Controle Automático de Irrigação
 * 
 * Grupo:
 * MARINA CLARA CONSTANTINO RIBEIRO RM568576
 * YASMIN KAUANE SILVA LIMA RM566645
 * GABRIEL COPPOLA RM568044
 *
 * Descrição:
 * Este sistema monitora os níveis de nutrientes (N, P, K), pH do solo (simulado via LDR),
 * umidade e temperatura do solo (via DHT22) para controlar automaticamente uma bomba d'água.
 * 
 * Cultura Agrícola Escolhida: TOMATE
 * Requisitos do Tomate:
 * - pH ideal: 6.0 a 7.0
 * - Umidade do solo: 60% a 80%
 * - Necessita de NPK balanceado para crescimento adequado
 * 
 * Lógica de Irrigação:
 * A bomba d'água é LIGADA quando:
 * - Umidade < 60% E pH fora da faixa ideal (6.0-7.0)
 * - OU quando umidade < 50% (emergência)
 * 
 * A bomba d'água é DESLIGADA quando:
 * - Umidade > 80% (solo saturado)
 * - OU quando umidade está adequada (60-80%) E pH está ideal (6.0-7.0)
 */

#include "DHT.h"

// ==================== DEFINIÇÃO DE PINOS ====================
const int botaoN = 2;   // Botão Nitrogênio (GPIO 2)
const int botaoP = 15;  // Botão Fósforo (GPIO 15)
const int botaoK = 4;   // Botão Potássio (GPIO 4)
const int pinoLDR = 34; // Sensor de pH simulado (LDR - GPIO 34)
const int pinoDHT = 27; // Sensor DHT22 - Umidade e Temperatura (GPIO 27)
const int pinoRele = 32; // Relé - Bomba d'água (GPIO 32)

// ==================== CONFIGURAÇÕES DOS SENSORES ====================
DHT dht(pinoDHT, DHT22);

// ==================== VARIÁVEIS GLOBAIS ====================
bool nivelN = false;  // Nível de Nitrogênio
bool nivelP = false;  // Nível de Fósforo
bool nivelK = false;  // Nível de Potássio
float pH = 7.0;       // pH do solo
float umidade = 0.0;  // Umidade do solo (%)
float temperatura = 0.0; // Temperatura do solo (°C)
bool bombaLigada = false; // Estado da bomba d'água

// ==================== PARÂMETROS DA CULTURA (TOMATE) ====================
const float pH_MIN_IDEAL = 6.0;
const float pH_MAX_IDEAL = 7.0;
const float UMIDADE_MIN_IDEAL = 60.0;
const float UMIDADE_MAX_IDEAL = 80.0;
const float UMIDADE_CRITICA = 50.0;

// ==================== FUNÇÃO: SETUP ====================
void setup() {
  Serial.begin(115200);
  Serial.println("\n====================================");
  Serial.println("FarmTech Solutions - Sistema Iniciado");
  Serial.println("Cultura: TOMATE");
  Serial.println("====================================\n");
  
  // Configuração dos botões (pull-up interno)
  pinMode(botaoN, INPUT_PULLUP);
  pinMode(botaoP, INPUT_PULLUP);
  pinMode(botaoK, INPUT_PULLUP);
  
  // Configuração do LDR (entrada analógica)
  pinMode(pinoLDR, INPUT);
  
  // Configuração do Relé (saída digital)
  pinMode(pinoRele, OUTPUT);
  digitalWrite(pinoRele, LOW); // Bomba inicia desligada
  
  // Inicialização do sensor DHT22
  dht.begin();
  
  delay(2000); // Aguarda estabilização dos sensores
}

// ==================== FUNÇÃO: LER NUTRIENTES (NPK) ====================
void lerNutrientes() {
  nivelN = !digitalRead(botaoN); // Invertido por causa do pull-up
  nivelP = !digitalRead(botaoP);
  nivelK = !digitalRead(botaoK);
}

// ==================== FUNÇÃO: CALCULAR pH ====================
void calcularPH() {
  int valorLDR = analogRead(pinoLDR);
  
  // Converte LDR para escala de pH (0-14)
  // Valor ADC de 0-4095 mapeado para pH 0-14
  pH = (valorLDR / 4095.0) * 14.0;
  
  // Ajustes baseados nos nutrientes NPK
  // Nitrogênio tende a aumentar pH
  if (nivelN) {
    pH += 0.5;
  }
  
  // Fósforo tende a diminuir pH levemente
  if (nivelP) {
    pH -= 0.3;
  }
  
  // Potássio tende a aumentar pH levemente
  if (nivelK) {
    pH += 0.2;
  }
  
  // Garante que pH esteja na faixa válida (0-14)
  pH = constrain(pH, 0.0, 14.0);
}

// ==================== FUNÇÃO: LER UMIDADE E TEMPERATURA ====================
bool lerUmidadeTemperatura() {
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature();
  
  // Verifica se a leitura foi bem-sucedida
  if (isnan(umidade) || isnan(temperatura)) {
    Serial.println("❌ ERRO: Falha na leitura do DHT22!");
    return false;
  }
  return true;
}

// ==================== FUNÇÃO: CONTROLAR IRRIGAÇÃO ====================
void controlarIrrigacao() {
  bool deveIrrigar = false;
  String motivo = "";
  
  // REGRA 1: Umidade crítica (< 50%) - EMERGÊNCIA
  if (umidade < UMIDADE_CRITICA) {
    deveIrrigar = true;
    motivo = "Umidade crítica (< 50%)";
  }
  
  // REGRA 2: Umidade baixa E pH fora do ideal
  else if (umidade < UMIDADE_MIN_IDEAL && (pH < pH_MIN_IDEAL || pH > pH_MAX_IDEAL)) {
    deveIrrigar = true;
    motivo = "Umidade baixa E pH inadequado";
  }
  
  // REGRA 3: Solo saturado (> 80%) - DESLIGAR
  else if (umidade > UMIDADE_MAX_IDEAL) {
    deveIrrigar = false;
    motivo = "Solo saturado (> 80%)";
  }
  
  // REGRA 4: Condições ideais - DESLIGAR
  else if (umidade >= UMIDADE_MIN_IDEAL && umidade <= UMIDADE_MAX_IDEAL && 
           pH >= pH_MIN_IDEAL && pH <= pH_MAX_IDEAL) {
    deveIrrigar = false;
    motivo = "Condições ideais para tomate";
  }
  
  // Atualiza estado da bomba
  if (deveIrrigar) {
    digitalWrite(pinoRele, HIGH);
    if (!bombaLigada) {
      Serial.println("\n🚰 ========== BOMBA LIGADA ==========");
      Serial.println("Motivo: " + motivo);
      bombaLigada = true;
    }
  } else {
    digitalWrite(pinoRele, LOW);
    if (bombaLigada) {
      Serial.println("\n🛑 ========== BOMBA DESLIGADA ==========");
      Serial.println("Motivo: " + motivo);
      bombaLigada = false;
    }
  }
}

// ==================== FUNÇÃO: EXIBIR DADOS NO MONITOR SERIAL ====================
void exibirDados() {
  Serial.println("\n┌────────────────────────────────────────┐");
  Serial.println("│    DADOS DO MONITORAMENTO DE SOLO     │");
  Serial.println("└────────────────────────────────────────┘");
  
  // Nutrientes NPK
  Serial.println("\n📊 NUTRIENTES (NPK):");
  Serial.print("  Nitrogênio (N): ");
  Serial.println(nivelN ? "✅ PRESENTE" : "❌ AUSENTE");
  Serial.print("  Fósforo (P):    ");
  Serial.println(nivelP ? "✅ PRESENTE" : "❌ AUSENTE");
  Serial.print("  Potássio (K):   ");
  Serial.println(nivelK ? "✅ PRESENTE" : "❌ AUSENTE");
  
  // pH do Solo
  Serial.println("\n🧪 pH DO SOLO:");
  Serial.print("  Valor: ");
  Serial.print(pH, 2);
  if (pH >= pH_MIN_IDEAL && pH <= pH_MAX_IDEAL) {
    Serial.println(" ✅ IDEAL para tomate");
  } else if (pH < pH_MIN_IDEAL) {
    Serial.println(" ⚠️ ÁCIDO demais (adicionar calcário)");
  } else {
    Serial.println(" ⚠️ ALCALINO demais (adicionar enxofre)");
  }
  
  // Umidade e Temperatura
  Serial.println("\n💧 UMIDADE E TEMPERATURA:");
  Serial.print("  Umidade: ");
  Serial.print(umidade, 1);
  Serial.print("%");
  if (umidade < UMIDADE_CRITICA) {
    Serial.println(" 🔴 CRÍTICO");
  } else if (umidade < UMIDADE_MIN_IDEAL) {
    Serial.println(" 🟡 BAIXA");
  } else if (umidade > UMIDADE_MAX_IDEAL) {
    Serial.println(" 🔵 ALTA");
  } else {
    Serial.println(" ✅ IDEAL");
  }
  
  Serial.print("  Temperatura: ");
  Serial.print(temperatura, 1);
  Serial.println("°C");
  
  // Estado da Bomba
  Serial.println("\n🚰 SISTEMA DE IRRIGAÇÃO:");
  Serial.print("  Status: ");
  Serial.println(bombaLigada ? "🟢 BOMBA LIGADA" : "⚪ BOMBA DESLIGADA");
  
  Serial.println("\n════════════════════════════════════════\n");
}

// ==================== FUNÇÃO: LOOP PRINCIPAL ====================
void loop() {
  // 1. Ler estado dos botões de nutrientes
  lerNutrientes();
  
  // 2. Calcular pH com base no LDR e nutrientes
  calcularPH();
  
  // 3. Ler umidade e temperatura do DHT22
  if (!lerUmidadeTemperatura()) {
    delay(2000);
    return; // Pula o ciclo se houver erro na leitura
  }
  
  // 4. Controlar sistema de irrigação
  controlarIrrigacao();
  
  // 5. Exibir dados no monitor serial
  exibirDados();
  
  // 6. Aguardar antes da próxima leitura
  delay(3000); // 3 segundos entre leituras
}
