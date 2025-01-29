import { describe, it, expect, beforeEach } from 'vitest'
import { mount } from '@vue/test-utils'
import router from '@/router/index.js'
import HomeView from '@/views/Home.vue'

describe('Home route', () => {
  it('renders HomeView on / route', async () => {
    const wrapper = mount(HomeView, {
      global: {
        plugins: [router],
      },
    })

    await router.push('/')
    await router.isReady()

    expect(wrapper.html()).toContain('landing/splash') // Check specific text or elements
  })
})
