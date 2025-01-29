import { describe, it, expect, beforeEach } from 'vitest'
import { mount } from '@vue/test-utils'
import router from '@/router/index.js'
import App from '@/App.vue'

describe('Router navigation', () => {
  it('navigates to About page', async () => {
    const wrapper = mount(App, {
      global: {
        plugins: [router],
      },
    })

    await router.push('/about')
    await router.isReady()

    expect(wrapper.html()).toContain('About') // Check specific text or elements from AboutView
  })
})
