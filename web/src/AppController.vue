<script setup lang="ts">
import { useWebSocket } from "@vueuse/core";
import StatusBadge from "./components/StatusBadge.vue";
import { Joystick, JoystickComponent } from "vue-joystick-component";

const { status, send } = useWebSocket(
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
  <div class="flex items-center justify-center flex-col gap-6">
    <StatusBadge :status />
    <Joystick
      :size="100"
      base-color="pink"
      stick-color="purple"
      :throttle="100"
      @start="reset"
      @stop="reset"
      @move="move"
    />
  </div>
</template>
