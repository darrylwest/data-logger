<script setup>
import { useRouter } from 'vue-router'
import { useLoginStore } from '@/stores/login'

const router = useRouter()
const loginStore = useLoginStore()

const handleLoginClick = () => {
  console.log('login button clicked')
  loginStore.setLoggedIn(true)
  router.push({ name: 'home' })
}

const handleCancelClick = () => {
  router.push({ name: 'home' })
}
</script>

<template>
  <div>
    <!-- this is where the phone# and email inputs are -->
    <div class="text-center py-4">
      <span
        @click="handleCancelClick"
        class="bg-gray-700 hover:bg-green-500 text-white font-semibold py-2 px-4 mx-2 rounded"
        >Cancel</span
      >
      <span
        @click="handleLoginClick"
        class="bg-gray-700 hover:bg-green-500 text-white font-semibold py-2 px-4 mx-2 rounded"
        >Login</span
      >
    </div>
  </div>
</template>
