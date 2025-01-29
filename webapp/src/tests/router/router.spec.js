import { describe, it, expect } from 'vitest'
import { createRouter, createWebHistory } from 'vue-router'
import routes from '@/router/index.js'
// import { useRouter } from 'vue-router'

describe('Router configuration', () => {
  it('should have correct routes', () => {
    const router = createRouter({
      history: createWebHistory(),
      routes: routes.options.routes, // Access routes from the `router` options
    })

    const routeNames = router.getRoutes().map((route) => route.name)
    expect(routeNames).toContain('home')
    expect(routeNames).toContain('about')
    expect(routeNames).toContain('login')
    expect(routeNames).toContain('register')
    expect(routeNames).toContain('docs')
    expect(routeNames).toContain('help')
    expect(routeNames).toContain('terms')
    expect(routeNames).toContain('privacy')
    expect(routeNames).toContain('contact')
    expect(routeNames).toContain('faq')
    // Add assertions for other routes
  })
})
