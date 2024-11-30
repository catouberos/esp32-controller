<script setup lang="ts">
import Joystick, { JoystickComponent } from "vue-joystick-component";
import { useWebSocket } from "@vueuse/core";
import StatusBadge from "./components/StatusBadge.vue";

const { status, data, send } = useWebSocket(
  `ws://${window.location.hostname}/websocket`,
  {
    autoReconnect: true,
    autoClose: false,
  },
);

function reset() {
  send("0,0,REST");
}

function move({ x, y, direction }: JoystickComponent.UpdateEvent) {
  console.log("move", { x, y, direction });
  send(`${x},${y},${direction}`);
}
</script>

<template>
  <main class="container mx-auto p-6 space-y-6">
    <div class="flex items-center justify-center">
      <StatusBadge :status />
    </div>

    <div>{{ data }}</div>
    <Joystick
      :size="100"
      base-color="pink"
      stick-color="purple"
      :throttle="100"
      @start="reset"
      @stop="reset"
      @move="move"
    />
  </main>
</template>
