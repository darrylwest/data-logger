<script setup>
import { useRouter } from 'vue-router'
import { VERSION } from '@/config'

const appVersion = VERSION
const router = useRouter()

const termsOfServicePage = () => {
  router.push({ name: 'terms' })
}
const privacyPolicyPage = () => {
  router.push({ name: 'privacy' })
}
const contactPage = () => {
  router.push({ name: 'contact' })
}
const faqPage = () => {
  router.push({ name: 'faq' })
}
</script>

<template>
  <footer class="bg-gray-800 text-white shadow-md py-3 fixed bottom-0 left-0 w-full">
    <div class="flex justify-between">
      <div class="mx-4">
        <div @click="termsOfServicePage" class="flex-row">
          <a href="#" class="hover:text-gray-400 mx-2">Terms of Service</a>
        </div>
        <div @click="privacyPolicyPage" class="flex-row">
          <a href="#" class="hover:text-gray-400 mx-2">Privacy Policy</a>
        </div>
      </div>

      <div class="text-center">
        <div @click="contactPage" class="flex-row">
          <a href="#" class="hover:text-gray-400 mx-2">Contact</a>
        </div>
        <div @click="faqPage" class="flex-row">
          <a href="#" class="hover:text-gray-400 mx-2">FAQ</a>
        </div>
      </div>

      <div class="mx-6 text-right">
        <div class="flex-row">Rain City Software &copy; 2025</div>
        <div class="flex-row">Version {{ appVersion }}</div>
      </div>
    </div>
  </footer>
</template>
