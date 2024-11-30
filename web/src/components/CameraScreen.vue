<script setup lang="ts">
import { computed } from "vue";

const props = defineProps<{
  data: Blob;
}>();

const src = computed<string>((previous) => {
  if (previous) URL.revokeObjectURL(previous);
  return URL.createObjectURL(props.data);
});
</script>

<template>
  <img :src />
</template>
