// src/stores/login.js
import { defineStore } from 'pinia'

export const useLoginStore = defineStore('login', {
  state: () => ({
    isLoggedIn: true,
  }),
  actions: {
    setLoggedIn(value) {
      // console.log('store: set the login state to value: ', value)
      this.isLoggedIn = value
      // console.log('store: new state value: ', this.isLoggedIn)
    },
  },
})
