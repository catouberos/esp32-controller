<script setup lang="ts">
import { useWebSocket } from "@vueuse/core";
import CameraScreen from "./components/CameraScreen.vue";
import { ref } from "vue";
import StatusBadge from "./components/StatusBadge.vue";

const url = ref(`ws://${import.meta.env.VITE_CAMERA_IP}/websocket`);

const { open, status, data } = useWebSocket<Blob>(url, {
  autoReconnect: true,
  autoClose: false,
});
</script>

<template>
  <div class="relative">
    <StatusBadge :status class="absolute right-0 top-0 m-3" />
    <CameraScreen
      v-if="status == 'OPEN' && data"
      class="w-full h-auto aspect-[4/3] rounded"
      :data
    />
    <div
      v-else
      class="w-full h-auto aspect-[4/3] bg-neutral-500 dark:bg-neutral-400 rounded"
    >
      <input class="w-full" v-model="url" />
      <button @click="open">Open</button>
    </div>
  </div>
</template>
