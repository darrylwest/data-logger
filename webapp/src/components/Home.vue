<template>
  <div class="container mx-auto pt-10">
    <h2 class="text-3xl font-bold text-center">Temperature Readings</h2>

    <!-- Display Selected Date Range and Interval -->
    <h3 class="font-bold text-center">
      {{ dataParams.end_date }} (Interval: {{ dataParams.interval }} minutes)
    </h3>

    <!-- Auto-Refresh Toggle -->
    <div class="flex justify-center items-center my-4">
      <label class="flex items-center space-x-2">
        <input type="checkbox" v-model="isAutoRefreshEnabled" class="w-5 h-5" />
        <span class="text-sm font-semibold">Auto-refresh every {{ autoRefreshInterval }} min</span>
      </label>
    </div>

    <!-- Loading Indicator -->
    <div v-if="isLoading" class="flex justify-center items-center mt-4">
      <div class="animate-spin rounded-full h-8 w-8 border-t-2 border-blue-500"></div>
      <span class="ml-2 text-blue-500">Loading data...</span>
    </div>

    <!-- Error Message & Retry Button -->
    <div v-if="errorMessage" class="text-center text-red-500 font-semibold mt-4">
      {{ errorMessage }}
      <button
        @click="fetchData"
        class="mt-2 px-4 py-2 bg-red-500 text-white rounded hover:bg-red-600"
      >
        Retry
      </button>
    </div>

    <!-- Chart -->
    <div class="mt-4 mb-10">
      <canvas v-show="!isLoading && !errorMessage" id="lineChart"></canvas>
      <p class="text-sm text-gray-500 text-center mt-2">
        Last updated: {{ lastUpdated || 'Fetching data...' }}
      </p>
    </div>

    <!-- Editable End Date Inputs -->
    <div class="flex justify-center gap-4 my-4">
      <label class="flex flex-col">
        Temperature Date
        <input v-model="dataParams.end_date" type="datetime-local" class="border p-2 rounded" />
      </label>
      <label class="flex flex-col">
        Interval (minutes):
        <select v-model="dataParams.interval" class="border p-2 rounded">
          <option :value="5">5</option>
          <option :value="10">10</option>
          <option :value="15">15</option>
          <option :value="20">20</option>
          <option :value="30">30</option>
          <option :value="60">60</option>
        </select>
      </label>
    </div>
  </div>
</template>

<script>
import { defineComponent, onMounted, ref, watch, onUnmounted, nextTick } from 'vue'
import { Chart, registerables } from 'chart.js'
import TemperatureService from '@/services/TemperatureService'

Chart.register(...registerables)

export default defineComponent({
  name: 'HomePage',
  setup() {
    const roundToPreviousHourLocal = () => {
      const now = new Date()
      now.setMinutes(0, 0, 0) // ✅ Set minutes, seconds, and milliseconds to 0 (round down)
      return now
    }

    const formatLocalDateTime = (date) => {
      const year = date.getFullYear()
      const month = String(date.getMonth() + 1).padStart(2, '0') // Ensure two digits
      const day = String(date.getDate()).padStart(2, '0')
      const hours = String(date.getHours()).padStart(2, '0') // Keep local hours
      return `${year}-${month}-${day}T${hours}:00` // Format for datetime-local
    }

    const end_date = ref(formatLocalDateTime(roundToPreviousHourLocal())) // ✅ Local time, rounded down

    const dataParams = ref({
      end_date: end_date,
      interval: 60,
    })

    let chartInstance = ref(null)
    let labels = ref([])
    let datasets = ref([])
    const isLoading = ref(false)
    const errorMessage = ref(null)
    const lastUpdated = ref(null)

    // Auto-refresh states
    const isAutoRefreshEnabled = ref(true)
    const autoRefreshInterval = ref(5) // Default: n minutes
    let autoRefreshTimer = null

    // Function to fetch data
    const fetchData = async () => {
      console.log('fetch data...')
      isLoading.value = true
      errorMessage.value = null

      try {
        const response = await TemperatureService.fetchTemperatureData(
          dataParams.value.end_date,
          dataParams.value.interval,
        )

        // Assign the labels (timestamps) from the API response
        labels.value = response.labels
        console.log('label count: ', labels.value.length)

        // Convert datasets to match Chart.js expected format
        datasets.value = response.datasets.map((sensor) => ({
          label: sensor.label, // Sensor name (e.g., "cottage-south")
          data: sensor.data, // Temperature readings
          borderColor: sensor.borderColor, // Use API color or generate one
          fill: sensor.fill || false,
        }))

        renderChart()
        lastUpdated.value = new Date().toLocaleString()
      } catch (error) {
        console.error('Error fetching temperature data:', error)
        errorMessage.value = 'Failed to load data. Please try again.'
      } finally {
        isLoading.value = false
      }
    }

    // Function to render chart
    const renderChart = async () => {
      await nextTick()

      const ctx = document.getElementById('lineChart').getContext('2d')

      if (chartInstance.value) {
        chartInstance.value.destroy()
      }

      /* no longer used, but maybe in the future...
      const all_data = datasets.value.flatMap(dataset => dataset.data);

      const miny = Math.floor(Math.min(...all_data) - 2)
      const maxy = Math.ceil(Math.max(...all_data) + 2)
      */

      chartInstance.value = new Chart(ctx, {
        type: 'line',
        data: { labels: labels.value, datasets: datasets.value },
        options: {
          responsive: true,
          plugins: {
            legend: {
              position: 'top',
            },
          },
        },
      })
    }

    // Auto-refresh function
    const startAutoRefresh = () => {
      if (autoRefreshTimer) {
        clearInterval(autoRefreshTimer)
      }
      if (isAutoRefreshEnabled.value) {
        autoRefreshTimer = setInterval(
          () => {
            fetchData()
          },
          autoRefreshInterval.value * 60 * 1000,
        ) // Convert minutes to milliseconds
      }
    }

    // Stop auto-refresh when component is unmounted
    onUnmounted(() => {
      if (autoRefreshTimer) {
        clearInterval(autoRefreshTimer)
      }
    })

    // Watch for auto-refresh toggle
    watch(isAutoRefreshEnabled, () => {
      startAutoRefresh()
    })

    watch(dataParams, fetchData, { deep: true })

    // Fetch data initially
    onMounted(() => {
      fetchData()
      startAutoRefresh()
    })

    return {
      dataParams,
      isLoading,
      errorMessage,
      lastUpdated,
      fetchData,
      isAutoRefreshEnabled,
      autoRefreshInterval,
    }
  },
})
</script>
