//
// global temperature data store
//
import { defineStore } from 'pinia'
import { ref } from 'vue'
import TemperatureService from '@/services/TemperatureService'

export const useTemperatureStore = defineStore('temperature', () => {
  const temperatures = ref([])
  const labels = ref([])

  const fetchData = () => {
    temperatures.value = TemperatureService.fetchReadings()
    labels.value = TemperatureService.fetchLabels()
  }

  return { temperatures, labels, fetchData }
})
