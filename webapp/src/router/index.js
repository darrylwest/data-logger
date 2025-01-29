import { createRouter, createWebHistory } from 'vue-router'
import HomeView from '@/views/Home.vue'
import AboutView from '@/views/About.vue'
import LoginView from '@/views/Login.vue'
import RegisterView from '@/views/Register.vue'
import Docs from '../views/Docs.vue'
import TermsOfService from '../views/TermsOfService.vue'
import PrivacyPolicy from '@/views/PrivacyPolicy.vue'
import Faq from '@/views/Faq.vue'
import Contact from '@/views/Contact.vue'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'home',
      component: HomeView,
    },
    {
      path: '/docs',
      name: 'docs',
      component: Docs,
    },
    {
      path: '/help',
      name: 'help',
      component: () => import('../views/Help.vue'),
    },
    {
      path: '/about',
      name: 'about',
      component: AboutView,
    },
    {
      path: '/terms',
      name: 'terms',
      component: TermsOfService,
    },
    {
      path: '/privacy',
      name: 'privacy',
      component: PrivacyPolicy,
    },
    {
      path: '/contact',
      name: 'contact',
      component: Contact,
    },
    {
      path: '/faq',
      name: 'faq',
      component: Faq,
    },
    {
      path: '/login',
      name: 'login',
      component: LoginView,
    },
    {
      path: '/register',
      name: 'register',
      component: RegisterView,
    },
  ],
})

export default router
