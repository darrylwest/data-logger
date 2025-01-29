import { describe, it, expect } from 'vitest'
import { mount } from '@vue/test-utils'
import { createRouter, createWebHistory } from 'vue-router'
import { routes } from '@/router/index' // Adjust the path as needed
import HomeView from '@/views/Home.vue'
import AboutView from '@/views/About.vue'
import LoginView from '@/views/Login.vue'
import RegisterView from '@/views/Register.vue'

describe('Views', () => {
  const router = createRouter({
    history: createWebHistory(),
    routes: [{ path: '/', component: HomeView }], // Minimal route for testing
  })

  beforeEach(async () => {
    // Ensure router is ready before each test
    await router.push('/')
    await router.isReady()
  })

  it('renders the Home view', () => {
    const wrapper = mount(HomeView, {
      global: {
        plugins: [router],
      },
    })

    // NOTE: nothing actuall here because it switches between landing and home
    // expect(wrapper.text()).toContain('Landing')

    expect(wrapper.classes()).toContain('container')
    expect(wrapper.classes()).toContain('mx-auto')
  })

  it('renders the About view', () => {
    const wrapper = mount(AboutView, {
      global: {
        plugins: [router],
      },
    })

    expect(wrapper.text()).toContain('About')
    expect(wrapper.html()).toContain('>About</h2>')

    expect(wrapper.classes()).toContain('container')
    expect(wrapper.classes()).toContain('mx-auto')
  })

  it('renders the Login view', () => {
    const wrapper = mount(LoginView, {
      global: {
        plugins: [router],
      },
    })

    expect(wrapper.text()).toContain('Login') // Replace with actual text or content
    expect(wrapper.text()).toContain('Cancel')

    expect(wrapper.classes()).toContain('container')
    expect(wrapper.classes()).toContain('mx-auto')
  })

  it('renders the Register view', () => {
    const wrapper = mount(RegisterView, {
      global: {
        plugins: [router],
      },
    })

    expect(wrapper.text()).toContain('Regist') // Replace with actual text or content
    expect(wrapper.text()).toContain('Cancel')
    expect(wrapper.text()).toContain('Login')
    expect(wrapper.classes()).toContain('container')
    expect(wrapper.classes()).toContain('mx-auto')
  })
})
