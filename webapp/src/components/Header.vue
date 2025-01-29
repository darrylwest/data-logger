<script setup>
import { computed } from 'vue'
import { useRouter } from 'vue-router'
import { useLoginStore } from '@/stores/login'
import applogo from '@/assets/logo.svg'

const appName = 'Data Logger'

const loginStore = useLoginStore()

const router = useRouter()

const handleLoginEvent = () => {
  loginStore.setLoggedIn(true)
  router.push({ name: 'home' })
}

const handleLogoutEvent = () => {
  loginStore.setLoggedIn(false)
  router.push({ name: 'home' })
}

const homePage = () => {
  router.push({ name: 'home' })
}

// Other pages that require login should use (loginStore.isLoggedIn) ? page : login...

const docsPage = () => {
  router.push({ name: 'docs' })
}

const aboutPage = () => {
  router.push({ name: 'about' })
}

const helpPage = () => {
  router.push({ name: 'help' })
}

const loginPage = () => {
  router.push({ name: 'login' })
}

const registerPage = () => {
  router.push({ name: 'register' })
}

// use computed here to retain the reactivity
const isLoggedIn = computed(() => loginStore.isLoggedIn)
const navHover = 'hover:text-lime-400'
const loginHover = 'hover:text-lime-200 hover:underline'
</script>

<template>
  <header
    class="bg-gray-800 text-white shadow-md bg-gradient-to-r from-gray-950 to-gray-300 fixed top-0 left-0 w-full"
  >
    <div class="flex justify-between items-center">
      <div class="flex items-center mx-2">
        <img :src="applogo" class="h-12 w-12" />
        <p class="text-xl font-bold">{{ appName }}</p>
      </div>

      <nav class="px-2">
        <ul class="flex space-x-4">
          <li><button @click="homePage" :class="navHover">Home</button></li>
          <li><button @click="docsPage" :class="navHover">Docs</button></li>
          <li><button @click="aboutPage" :class="navHover">About</button></li>
          <li><button @click="helpPage" :class="navHover">Help</button></li>
        </ul>
      </nav>

      <div class="mx-4 my-4">
        <div v-if="isLoggedIn">
          <button @click="handleLogoutEvent" :class="loginHover" class="mx-2">Logout</button>
        </div>
        <div v-else>
          <button @click="loginPage" :class="loginHover" class="mx-2">Login</button>
          <button @click="registerPage" :class="loginHover" class="mx-2">Register</button>
        </div>
      </div>
    </div>
  </header>
</template>
