# Sistema de Irrigação Inteligente para Tomates com ESP32

## 👥 Grupo

- Gabriel Coppola - RM568044
- Marina Clara Constantino Ribeiro - RM568576
- Yasmin Kauane Silva Lima - RM566645

## 👋 Visão Geral

Este projeto implementa um sistema de irrigação inteligente específico para cultivo de tomates, utilizando o microcontrolador ESP32. Ele integra sensores e atuadores para monitorar as condições do solo (pH, umidade, temperatura e presença de nutrientes NPK), acionando a irrigação automaticamente conforme as necessidades ideais da cultura do tomateiro.


## 📝 Funcionalidades

- **Simulação de Nutrientes (N, P, K):** Cada botão físico simula a presença dos nutrientes Nitrogênio, Fósforo e Potássio no solo.
- **Simulação de pH do Solo:** Um sensor LDR capta variações de luminosidade, mapeadas para um valor de pH (0 a 14). O pH é também influenciado pela presença dos nutrientes simulados.
- **Sensor de Umidade/Temperatura:** O sensor DHT22 mede temperatura e umidade do ar, essenciais para o controle de irrigação.
- **Atuação Automática — Relé:** Um relé comanda o acionamento de uma bomba de irrigação conforme a lógica baseada em umidade e pH do solo, otimizada para tomates.
- **Exibição em Serial Monitor:** Todas as leituras e ações são reportadas com emojis e formatação clara para monitoramento em tempo real.
- **Otimizado especificamente para cultivo de tomates.**

## 🚜 Hardware Utilizado

- **ESP32**
- **3 x Botões pressionáveis** (com resistores pull-up internos)
- **Sensor LDR**
- **Sensor DHT22**
- **Módulo Relé** (controla bomba d'água)

## 🔌 Esquema de Ligações

- **Botão 1 (Nitrogênio - N):** GPIO 2
- **Botão 2 (Fósforo - P):** GPIO 15
- **Botão 3 (Potássio - K):** GPIO 4
- **LDR:** GPIO 34 (entrada analógica)
- **DHT22:** GPIO 27 (dados)
- **Relé:** GPIO 32

> Veja a imagem do circuito na pasta `/imagens` deste repositório ou acesse a simulação no Wokwi


## ⚙️ Lógica de Funcionamento

### 1. Simulação de Nutrientes com Botões

Cada botão, ao ser pressionado, simula a presença de nutrientes essenciais do solo, influenciando o pH:
- **Botão Nitrogênio (N):** Aumenta pH em +0.5 (solo mais alcalino)
- **Botão Fósforo (P):** Diminui pH em -0.3 (solo mais ácido)
- **Botão Potássio (K):** Aumenta pH em +0.2 (leve alcalinização)

### 2. Medição e Ajuste do pH

- O valor lido pelo LDR (0 a 4095) é mapeado para uma escala de pH (0 a 14).
- O valor base é ajustado em tempo real conforme os botões (nutrientes) são pressionados.
- O pH final é sempre mantido entre 0.0 e 14.0.

### 3. Leitura de Umidade & Temperatura

- Utiliza o sensor DHT22 para obter dados precisos do ambiente.
- Somente leituras válidas são consideradas (verifica se isnan()).
- Classifica a umidade em: CRÍTICO, BAIXA, IDEAL ou ALTA.

### 4. Lógica de Irrigação (Relé) - Otimizada para Tomates

O relé é ativado/desativado com base nas seguintes regras específicas para tomates:

**Liga a bomba se:**
- Umidade < 50% (crítico) **OU**
- Umidade < 60% **E** pH fora da faixa ideal (6.0 a 7.0)

**Desliga a bomba se:**
- Umidade > 80% (solo saturado) **OU**
- Umidade entre 60-80% **E** pH entre 6.0-7.0 (condições ideais)

**Parâmetros ideais para tomateiro:**
- **pH ideal:** 6.0 a 7.0
- **Umidade ideal:** 60% a 80%
- **Temperatura ideal:** 20°C a 28°C

> Ajuste facilmente no código para outra cultura, modificando os limites nas funções `controlarIrrigacao()`.


## 💻 Código-Fonte

Veja o arquivo `irrigacao_tomate.ino` para a implementação completa.


## 📋 Como Usar

1. Monte o circuito conforme descrito acima.
2. Faça upload do código para o ESP32 usando a Arduino IDE.
3. Abra o Serial Monitor (115200 baud) para acompanhar leituras e o funcionamento do sistema.
4. **No Wokwi:** Clique no DHT22 para ajustar umidade e temperatura durante a simulação.
5. **No Wokwi:** Clique no LDR e ajuste o slider para simular diferentes níveis de pH.
6. Pressione os botões para simular diferentes níveis de nutrientes e observe o impacto no pH.
7. Observe a ativação/desativação automática do relé (LED indica bomba ligada/desligada).


## 🎥 Demonstração em Vídeo

Veja a demonstração completa de funcionamento neste vídeo no YouTube.

**O vídeo mostra:**
- Explicação dos componentes
- Circuito no Wokwi funcionando
- Cenário 1: Solo seco → Bomba liga
- Cenário 2: Solo ideal → Bomba desliga
- Cenário 3: Solo saturado → Bomba desliga

## 🔗 Links Importantes

- **Simulação Wokwi:** [Acesse aqui](https://wokwi.com/projects/444825921859865601)
- **Vídeo Demonstrativo:** [Youtube](https://youtu.be/XHSX82LDZh0)
- **Repositório GitHub:** Código completo

## 📊 Referências Técnicas

- Cultivo de Tomates - Embrapa
- Documentação ESP32
- Nutrientes NPK na Agricultura

