<script setup>
import { ref, computed, onUnmounted } from "vue";
import mqtt from "mqtt"; // Certifique-se de instalar: npm install mqtt

// Estados
const isConnected = ref(false);
const connecting = ref(false);
const client = ref(null);
const topic = ref("vue3/test/topic");
const message = ref("");
const messages = ref([]);
const logs = ref([]);

// Lista de brokers públicos
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

// Broker selecionado
const selectedBroker = ref(brokers.value[1]); // começa com o "wss" do Mosquitto

// Computed: texto e estilo do status
const statusText = computed(() => {
  if (connecting.value) return "🔄 Conectando...";
  return isConnected.value ? "✅ Conectado" : "❌ Desconectado";
});

const buttonText = computed(() => {
  if (connecting.value) return "Conectando...";
  return isConnected.value ? "Desconectar" : "Conectar";
});

// Função para adicionar logs
const addLog = (msg) => {
  logs.value.unshift(`[${new Date().toLocaleTimeString()}] ${msg}`);
  if (logs.value.length > 15) logs.value.pop();
};

// Conectar ao broker
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
      client.value.subscribe(topic.value, (err) => {
        if (!err) addLog(`📌 Inscrito no tópico: ${topic.value}`);
      });
    });

    client.value.on("message", (receivedTopic, receivedMessage) => {
      const newMsg = {
        topic: receivedTopic,
        message: receivedMessage.toString(),
        timestamp: new Date().toLocaleTimeString(),
      };
      messages.value.unshift(newMsg);
      addLog(`📥 Mensagem recebida de ${receivedTopic}`);
    });

    client.value.on("error", (error) => {
      connecting.value = false;
      addLog(`❌ Erro: ${error.message}`);
      console.error("MQTT Error:", error);
    });

    client.value.on("close", () => {
      isConnected.value = false;
      connecting.value = false;
      addLog("🔌 Conexão encerrada");
    });
  } catch (error) {
    connecting.value = false;
    addLog(`❌ Erro na conexão: ${error.message}`);
  }
};

// Desconectar do broker
const disconnect = () => {
  if (client.value) {
    client.value.end();
    client.value = null;
    isConnected.value = false;
    messages.value = [];
    addLog("🔌 Desconectado");
  }
};

// Alternar entre conectar e desconectar
const toggleConnection = () => {
  if (isConnected.value) disconnect();
  else connect();
};

// Publicar mensagem
const publish = () => {
  if (client.value && isConnected.value && topic.value && message.value) {
    client.value.publish(topic.value, message.value);
    addLog(`📤 Publicado em ${topic.value}: ${message.value}`);
    message.value = "";
  }
};

// Trocar broker
const changeBroker = () => {
  if (isConnected.value) disconnect();
  addLog(`Broker alterado para: ${selectedBroker.value.name}`);
};

// Limpeza automática ao sair
onUnmounted(() => {
  disconnect();
});
</script>

<template>
  <v-container class="pa-4" style="max-width: 600px">
    <!-- Título -->
    <h1 class="text-h5 mb-4">Painel MQTT - Vue 3 + Vuetify</h1>

    <!-- Status da conexão -->
    <v-card class="pa-4 mb-4">
      <v-alert
        :type="isConnected ? 'success' : connecting ? 'warning' : 'error'"
        dense
        class="mb-4"
      >
        {{ statusText }}
      </v-alert>

      <!-- Seleção de Broker -->
      <v-select
        label="Broker"
        :items="brokers"
        item-title="name"
        item-value="url"
        v-model="selectedBroker"
        @change="changeBroker"
        outlined
        dense
      ></v-select>

      <!-- Botão conectar/desconectar -->
      <v-btn
        color="primary"
        class="mt-3"
        :loading="connecting"
        @click="toggleConnection"
      >
        {{ buttonText }}
      </v-btn>
    </v-card>

    <!-- Publicação de mensagem -->
    <v-card class="pa-4 mb-4" v-if="isConnected">
      <h3 class="text-h6 mb-3">Publicar mensagem</h3>

      <v-text-field
        label="Tópico"
        v-model="topic"
        outlined
        dense
      ></v-text-field>

      <v-text-field
        label="Mensagem"
        v-model="message"
        outlined
        dense
      ></v-text-field>

      <v-btn color="success" class="mt-2" @click="publish"> 📤 Publicar </v-btn>
    </v-card>

    <!-- Mensagens recebidas -->
    <v-card class="pa-4 mb-4" v-if="messages.length">
      <h3 class="text-h6 mb-2">Mensagens Recebidas</h3>
      <v-list max-height="200" class="overflow-y-auto">
        <v-list-item v-for="(msg, i) in messages" :key="i">
          <v-list-item-content>
            <v-list-item-title>{{ msg.message }}</v-list-item-title>
            <v-list-item-subtitle>
              {{ msg.topic }} • {{ msg.timestamp }}
            </v-list-item-subtitle>
          </v-list-item-content>
        </v-list-item>
      </v-list>
    </v-card>

    <!-- Logs -->
    <v-card class="pa-4">
      <h3 class="text-h6 mb-2">Logs</h3>
      <v-list max-height="200" class="overflow-y-auto">
        <v-list-item v-for="(log, i) in logs" :key="i">
          <v-list-item-content>
            <v-list-item-subtitle class="font-mono text-caption">{{
              log
            }}</v-list-item-subtitle>
          </v-list-item-content>
        </v-list-item>
      </v-list>
    </v-card>
  </v-container>
</template>
