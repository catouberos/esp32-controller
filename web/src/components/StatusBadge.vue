<script setup lang="ts">
import type { WebSocketStatus } from "@vueuse/core";

defineProps<{
  status: WebSocketStatus;
}>();
</script>

<template>
  <span
    class="inline-flex items-center font-medium rounded-md text-xs px-2 py-1 gap-1 bg-primary-500 dark:bg-primary-400 text-white dark:text-gray-900"
    :class="{
      'bg-green-500 dark:bg-green-400': status === 'OPEN',
      'bg-red-500 dark:bg-red-400': status === 'CLOSED',
      'bg-amber-500 dark:bg-amber-400': status === 'CONNECTING',
    }"
  >
    {{ status }}
  </span>
</template>
