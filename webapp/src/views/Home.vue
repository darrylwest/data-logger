<script setup>
import { useLoginStore } from '@/stores/login'
import Home from '@/components/Home.vue'
import Landing from '@/components/Landing.vue'

const loginStore = useLoginStore()
</script>

<template>
  <main class="container mx-auto pt-10">
    <!-- if not logged in then show the landing/splash page -->
    <!-- else show the home page -->
    <div v-if="loginStore.isLoggedIn"><Home /></div>
    <div v-else><Landing /></div>
  </main>
</template>
