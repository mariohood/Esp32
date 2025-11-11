<template>
  <div class="container">
    <h1>🔌 Vue 3 + MQTT</h1>

    <!-- Status -->
    <div class="status">
      <p>
        Status:
        <span :class="statusClass">{{ statusText }}</span>
      </p>
      <button @click="toggleConnection" :disabled="connecting" class="btn">
        {{ buttonText }}
      </button>
    </div>

    <!-- Broker Selection -->vue3/demo/topic
    <div class="broker-select">
      <label>Selecionar Broker:</label>
      <select v-model="selectedBroker" @change="changeBroker">
        <option v-for="broker in brokers" :key="broker.url" :value="broker">
          {{ broker.name }}
        </option>
      </select>
    </div>

    <!-- Publicar -->
    <div v-if="isConnected" class="publish">
      <h3>📤 Publicar Mensagem</h3>
      <input v-model="topic" placeholder="Tópico" class="input" />
      <input v-model="message" placeholder="Mensagem" class="input" />
      <button @click="publish" class="btn publish-btn">Publicar</button>
    </div>

    <!-- Mensagens -->
    <div v-if="isConnected" class="messages">
      <h3>📨 Mensagens ({{ messages.length }}):</h3>
      <div v-for="(msg, index) in messages" :key="index" class="message">
        <strong>[{{ msg.timestamp }}] {{ msg.topic }}:</strong>
        {{ msg.message }}
      </div>
    </div>

    <!-- Log -->
    <div class="log">
      <h3>📋 Log:</h3>
      <div v-for="(log, index) in logs" :key="index" class="log-entry">
        {{ log }}
      </div>
    </div>
  </div>
</template>

<script>
import { ref, computed, onUnmounted } from "vue";
import mqtt from "mqtt";

export default {
  name: "App",
  setup() {
    // Estados
    const isConnected = ref(false);
    const connecting = ref(false);
    const client = ref(null);
    const topic = ref("vue3/test/topic");
    const message = ref("Hello from Vue 3!");
    const messages = ref([]);
    const logs = ref([]);

    // Brokers alternativos
    const brokers = ref([
      {
        name: "Mosquitto WebSocket (8080)",
        url: "ws://test.mosquitto.org:8080",
      },
      {
        name: "Mosquitto WebSocket Secure (8081)",
        url: "wss://test.mosquitto.org:8081",
      },
      {
        name: "HiveMQ Public",
        url: "wss://broker.hivemq.com:8884",
      },
      {
        name: "EMQX Public",
        url: "wss://broker.emqx.io:8084/mqtt",
      },
    ]);

    const selectedBroker = ref(brokers.value[1]); // Começa com wss

    // Computed
    const statusText = computed(() => {
      if (connecting.value) return "🔄 Conectando...";
      return isConnected.value ? "✅ Conectado" : "❌ Desconectado";
    });

    const statusClass = computed(() => {
      if (connecting.value) return "connecting";
      return isConnected.value ? "connected" : "disconnected";
    });

    const buttonText = computed(() => {
      if (connecting.value) return "Conectando...";
      return isConnected.value ? "Desconectar" : "Conectar";
    });

    // Funções
    const addLog = (message) => {
      logs.value.unshift(`[${new Date().toLocaleTimeString()}] ${message}`);
      if (logs.value.length > 10) logs.value.pop();
    };

    const connect = () => {
      if (client.value) return;

      connecting.value = true;
      addLog(`Tentando conectar em: ${selectedBroker.value.url}`);

      try {
        client.value = mqtt.connect(selectedBroker.value.url, {
          clientId: "vue3_client_" + Math.random().toString(16).substr(2, 8),
          keepalive: 60,
          clean: true,
          reconnectPeriod: 4000,
          connectTimeout: 8000,
        });

        client.value.on("connect", () => {
          isConnected.value = true;
          connecting.value = false;
          addLog("✅ Conectado com sucesso!");

          // Inscrever no tópico
          client.value.subscribe(topic.value, (err) => {
            if (!err) {
              addLog(`📌 Inscrito no tópico: ${topic.value}`);
            }
          });
        });

        client.value.on("message", (receivedTopic, receivedMessage) => {
          const newMessage = {
            topic: receivedTopic,
            message: receivedMessage.toString(),
            timestamp: new Date().toLocaleTimeString(),
          };
          messages.value.unshift(newMessage);
          addLog(`📥 Mensagem recebida: ${receivedTopic}`);
        });

        client.value.on("error", (error) => {
          connecting.value = false;
          addLog(`❌ Erro: ${error.message}`);
          console.error("MQTT Error:", error);
        });

        client.value.on("close", () => {
          isConnected.value = false;
          connecting.value = false;
          addLog("🔌 Conexão fechada");
        });
      } catch (error) {
        connecting.value = false;
        addLog(`❌ Erro na conexão: ${error.message}`);
      }
    };

    const disconnect = () => {
      if (client.value) {
        client.value.end();
        client.value = null;
        isConnected.value = false;
        messages.value = [];
        addLog("🔌 Desconectado");
      }
    };

    const toggleConnection = () => {
      if (isConnected.value) {
        disconnect();
      } else {
        connect();
      }
    };

    const publish = () => {
      if (client.value && isConnected.value && topic.value && message.value) {
        client.value.publish(topic.value, message.value);
        addLog(`📤 Publicado em ${topic.value}: ${message.value}`);
        message.value = "";
      }
    };

    const changeBroker = () => {
      if (isConnected.value) {
        disconnect();
      }
      addLog(`Broker alterado para: ${selectedBroker.value.name}`);
    };

    // Cleanup
    onUnmounted(() => {
      disconnect();
    });

    return {
      isConnected,
      connecting,
      topic,
      message,
      messages,
      logs,
      brokers,
      selectedBroker,
      statusText,
      statusClass,
      buttonText,
      toggleConnection,
      publish,
      changeBroker,
    };
  },
};
</script>

<style>
.container {
  max-width: 600px;
  margin: 0 auto;
  padding: 20px;
  font-family: Arial, sans-serif;
}

.status {
  background: #f5f5f5;
  padding: 15px;
  border-radius: 8px;
  margin-bottom: 20px;
}

.connected {
  color: green;
  font-weight: bold;
}

.disconnected {
  color: red;
  font-weight: bold;
}

.connecting {
  color: orange;
  font-weight: bold;
}

.btn {
  padding: 10px 20px;
  border: none;
  border-radius: 5px;
  cursor: pointer;
  font-size: 16px;
  margin: 5px;
}

.btn:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}

.publish-btn {
  background: #4caf50;
  color: white;
}

.input {
  width: 100%;
  padding: 10px;
  margin: 5px 0;
  border: 1px solid #ddd;
  border-radius: 5px;
  box-sizing: border-box;
}

.broker-select {
  margin: 15px 0;
}

.broker-select select {
  padding: 8px;
  border: 1px solid #ddd;
  border-radius: 5px;
  margin-left: 10px;
}

.messages,
.log {
  margin-top: 20px;
  max-height: 300px;
  overflow-y: auto;
}

.message,
.log-entry {
  background: white;
  padding: 10px;
  margin: 5px 0;
  border: 1px solid #ddd;
  border-radius: 5px;
  font-size: 0.9em;
}

.log-entry {
  color: #666;
  font-family: monospace;
}

h1,
h3 {
  color: #333;
}
</style>
