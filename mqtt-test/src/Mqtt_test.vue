<template>
  <div class="mqtt-container">
    <h1>🔌 Vue 3 + MQTT - Seguindo Passos</h1>

    <!-- Painel de Status -->
    <div class="status-panel">
      <div class="step-indicator">
        <div :class="['step', connectionStep >= 1 ? 'completed' : '']">
          1. Importar Biblioteca
        </div>
        <div :class="['step', connectionStep >= 2 ? 'completed' : '']">
          2. Parâmetros Conexão
        </div>
        <div :class="['step', connectionStep >= 3 ? 'completed' : '']">
          3. Definir Callback
        </div>
        <div :class="['step', connectionStep >= 4 ? 'completed' : '']">
          4. Criar Cliente
        </div>
        <div :class="['step', connectionStep >= 5 ? 'completed' : '']">
          5. Associar Callback
        </div>
        <div :class="['step', connectionStep >= 6 ? 'completed' : '']">
          6. Conectar Broker
        </div>
        <div :class="['step', connectionStep >= 7 ? 'completed' : '']">
          7. Loop Conexão
        </div>
      </div>

      <div class="connection-status">
        <span :class="['status', isConnected ? 'connected' : 'disconnected']">
          {{ isConnected ? "✅ CONECTADO" : "❌ DESCONECTADO" }}
        </span>
        <button @click="toggleConnection" class="btn">
          {{ isConnected ? "Parar Conexão" : "Iniciar Conexão" }}
        </button>
      </div>
    </div>

    <!-- Controles -->
    <div class="control-panel" v-if="isConnected">
      <div class="input-group">
        <input v-model="publishTopic" placeholder="Tópico para publicar" />
        <input v-model="publishMessage" placeholder="Mensagem" />
        <button @click="publish" class="btn btn-publish">Publicar</button>
      </div>

      <div class="input-group">
        <input v-model="subscribeTopic" placeholder="Tópico para inscrever" />
        <button @click="subscribe" class="btn btn-subscribe">Inscrever</button>
      </div>
    </div>

    <!-- Mensagens -->
    <div class="messages-panel">
      <h3>📨 Mensagens Recebidas:</h3>
      <div class="messages">
        <div v-for="(msg, index) in messages" :key="index" class="message">
          <strong>[{{ msg.timestamp }}] {{ msg.topic }}:</strong>
          {{ msg.message }}
        </div>
      </div>
    </div>

    <!-- Log de Passos -->
    <div class="steps-log">
      <h3>📋 Log dos Passos Executados:</h3>
      <div class="log-entries">
        <div v-for="(log, index) in stepLogs" :key="index" class="log-entry">
          {{ log }}
        </div>
      </div>
    </div>
  </div>
</template>

<script>
// PASSO 1: Importar biblioteca
import mqtt from "mqtt";
import { ref, reactive, onMounted, onUnmounted } from "vue";

export default {
  name: "MqttStepByStep",
  setup() {
    // Estado da aplicação
    const isConnected = ref(false);
    const connectionStep = ref(0);
    const stepLogs = ref([]);
    const messages = ref([]);

    // Dados para publicação/inscrição
    const publishTopic = ref("vue3/demo/topic");
    const publishMessage = ref("Hello MQTT from Vue3!");
    const subscribeTopic = ref("vue3/demo/#");

    // PASSO 2: Definir os parâmetros de conexão
    const connectionParams = reactive({
      brokerUrl: "wss://test.mosquitto.org:8081", // Broker público
      options: {
        clientId: "vue3_mqtt_client_" + Math.random().toString(16).substr(2, 8),
        keepalive: 60,
        clean: true,
        reconnectPeriod: 4000,
        connectTimeout: 8000,
      },
    });

    // Referência para o cliente MQTT
    let mqttClient = null;

    // PASSO 3: Definir o callback
    const mqttCallback = (topic, message) => {
      const newMessage = {
        topic: topic,
        message: message.toString(),
        timestamp: new Date().toLocaleTimeString(),
      };

      messages.value.unshift(newMessage);
      addLog(`📥 Callback executado: Mensagem recebida no tópico "${topic}"`);

      // Manter apenas as últimas 20 mensagens
      if (messages.value.length > 20) {
        messages.value.pop();
      }
    };

    // Função para adicionar logs
    const addLog = (message) => {
      stepLogs.value.unshift(`[${new Date().toLocaleTimeString()}] ${message}`);
      if (stepLogs.value.length > 15) {
        stepLogs.value.pop();
      }
    };

    // Função principal de conexão
    const connectToBroker = () => {
      addLog("🚀 INICIANDO PROCESSO DE CONEXÃO MQTT...");
      connectionStep.value = 1;

      try {
        addLog("✅ PASSO 1: Biblioteca MQTT importada com sucesso");

        // PASSO 2: Parâmetros já definidos
        connectionStep.value = 2;
        addLog(
          `✅ PASSO 2: Parâmetros de conexão definidos - Broker: ${connectionParams.brokerUrl}`
        );

        // PASSO 3: Callback definido
        connectionStep.value = 3;
        addLog("✅ PASSO 3: Função callback para mensagens definida");

        // PASSO 4: Criar um cliente MQTT
        connectionStep.value = 4;
        addLog("🔄 PASSO 4: Criando cliente MQTT...");

        mqttClient = mqtt.connect(
          connectionParams.brokerUrl,
          connectionParams.options
        );
        addLog("✅ PASSO 4: Cliente MQTT criado com sucesso");

        // PASSO 5: Associar a função callback para a conexão
        connectionStep.value = 5;
        addLog("🔄 PASSO 5: Associando callbacks aos eventos...");

        // Callback para mensagens recebidas
        mqttClient.on("message", mqttCallback);

        // Callbacks para eventos de conexão
        mqttClient.on("connect", () => {
          connectionStep.value = 6;
          isConnected.value = true;
          addLog("✅ PASSO 6: Conectado ao broker MQTT com sucesso!");
          startConnectionLoop();
        });

        mqttClient.on("error", (error) => {
          addLog(`❌ Erro na conexão: ${error.message}`);
          resetConnection();
        });

        mqttClient.on("close", () => {
          addLog("🔌 Conexão fechada");
          resetConnection();
        });

        mqttClient.on("reconnect", () => {
          addLog("🔄 Tentando reconectar...");
        });

        addLog("✅ PASSO 5: Todos os callbacks associados aos eventos");

        // PASSO 6: Conectar ao broker
        connectionStep.value = 6;
        addLog("🔄 PASSO 6: Conectando ao broker...");
      } catch (error) {
        addLog(`❌ Erro no processo de conexão: ${error.message}`);
        resetConnection();
      }
    };

    // PASSO 7: Deixar a conexão em loop
    const startConnectionLoop = () => {
      connectionStep.value = 7;
      addLog(
        "✅ PASSO 7: Conexão em loop - Pronto para receber/ enviar mensagens"
      );

      // O próprio cliente MQTT gerencia o loop de conexão automaticamente
      // com keepalive e reconexão automática
    };

    // Função para publicar mensagens
    const publish = () => {
      if (mqttClient && isConnected.value) {
        mqttClient.publish(publishTopic.value, publishMessage.value);
        addLog(
          `📤 Mensagem publicada no tópico "${publishTopic.value}": ${publishMessage.value}`
        );
      }
    };

    // Função para inscrever em tópicos
    const subscribe = () => {
      if (mqttClient && isConnected.value) {
        mqttClient.subscribe(subscribeTopic.value, (err) => {
          if (!err) {
            addLog(`✅ Inscrito no tópico: ${subscribeTopic.value}`);
          } else {
            addLog(`❌ Erro na inscrição: ${err.message}`);
          }
        });
      }
    };

    // Função para desconectar
    const disconnect = () => {
      if (mqttClient) {
        mqttClient.end();
        addLog("🔌 Cliente MQTT desconectado");
      }
      resetConnection();
    };

    // Resetar estado da conexão
    const resetConnection = () => {
      mqttClient = null;
      isConnected.value = false;
      connectionStep.value = 0;
    };

    // Toggle conexão
    const toggleConnection = () => {
      if (isConnected.value) {
        disconnect();
      } else {
        connectToBroker();
      }
    };

    // Limpeza quando o componente for destruído
    onUnmounted(() => {
      if (mqttClient) {
        disconnect();
      }
    });

    // Iniciar automaticamente (opcional)
    onMounted(() => {
      addLog(
        '🔧 Aplicação Vue 3 + MQTT carregada. Clique em "Iniciar Conexão" para começar.'
      );
    });

    return {
      // Estado
      isConnected,
      connectionStep,
      stepLogs,
      messages,

      // Dados
      publishTopic,
      publishMessage,
      subscribeTopic,

      // Métodos
      toggleConnection,
      publish,
      subscribe,
    };
  },
};
</script>

<style scoped>
.mqtt-container {
  max-width: 1000px;
  margin: 0 auto;
  padding: 20px;
  font-family: "Segoe UI", Tahoma, Geneva, Verdana, sans-serif;
}

.step-indicator {
  display: flex;
  justify-content: space-between;
  margin: 20px 0;
  flex-wrap: wrap;
  gap: 10px;
}

.step {
  padding: 10px 15px;
  background: #f0f0f0;
  border-radius: 20px;
  font-size: 0.9em;
  text-align: center;
  flex: 1;
  min-width: 120px;
}

.step.completed {
  background: #4caf50;
  color: white;
}

.connection-status {
  display: flex;
  align-items: center;
  gap: 15px;
  margin: 20px 0;
  padding: 15px;
  background: #f8f9fa;
  border-radius: 8px;
}

.status {
  padding: 8px 16px;
  border-radius: 20px;
  font-weight: bold;
}

.status.connected {
  background: #4caf50;
  color: white;
}

.status.disconnected {
  background: #f44336;
  color: white;
}

.btn {
  padding: 10px 20px;
  border: none;
  border-radius: 5px;
  cursor: pointer;
  font-weight: bold;
}

.btn:hover {
  opacity: 0.9;
}

.btn-publish {
  background: #2196f3;
  color: white;
}

.btn-subscribe {
  background: #ff9800;
  color: white;
}

.control-panel {
  margin: 20px 0;
  padding: 20px;
  background: #f8f9fa;
  border-radius: 8px;
}

.input-group {
  display: flex;
  gap: 10px;
  margin: 10px 0;
  align-items: center;
}

.input-group input {
  flex: 1;
  padding: 10px;
  border: 1px solid #ddd;
  border-radius: 5px;
}

.messages-panel,
.steps-log {
  margin: 20px 0;
  padding: 20px;
  background: #f8f9fa;
  border-radius: 8px;
}

.messages,
.log-entries {
  max-height: 300px;
  overflow-y: auto;
  border: 1px solid #ddd;
  border-radius: 5px;
  padding: 10px;
  background: white;
}

.message,
.log-entry {
  padding: 8px;
  border-bottom: 1px solid #eee;
  font-family: monospace;
  font-size: 0.9em;
}

.message:last-child,
.log-entry:last-child {
  border-bottom: none;
}

.log-entry {
  color: #666;
}
</style>
