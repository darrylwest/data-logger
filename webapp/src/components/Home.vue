<template>
  <div class="container mx-auto pt-10">
    <h2 class="text-3xl font-bold text-center">Temperature Readings</h2>

    <!-- Display Selected Date Range -->
    <h3 class="font-bold text-center">
      {{ dataParams.start_date }} to {{ dataParams.end_date }}
    </h3>

    <div class="mt-4 mb-20">
      <canvas id="lineChart"></canvas>
    </div>

    <!-- Editable Start and End Date Inputs -->
    <div class="flex justify-center gap-4 my-4">
      <label class="flex flex-col">
        Start Date:
        <input v-model="dataParams.start_date" type="datetime-local" class="border p-2 rounded" />
      </label>
      <label class="flex flex-col">
        End Date:
        <input v-model="dataParams.end_date" type="datetime-local" class="border p-2 rounded" />
      </label>
    </div>

  </div>
</template>

<script>
import { defineComponent, onMounted, ref, watch } from 'vue'
import { Chart, registerables } from 'chart.js'
import TemperatureService from '@/services/TemperatureService'

Chart.register(...registerables)

export default defineComponent({
  name: 'HomePage',
  setup() {
    const dataParams = ref(TemperatureService.dataParams())
    let chartInstance = null;

    const renderChart = () => {
      const ctx = document.getElementById("lineChart").getContext("2d");

      // Destroy existing chart to prevent duplicates
      if (chartInstance) {
        chartInstance.destroy();
      }

      chartInstance = new Chart(ctx, {
        type: "line",
        data: {
          labels: TemperatureService.fetchLabels(),
          datasets: TemperatureService.fetchReadings(),
        },
        options: {
          responsive: true,
          plugins: {
            tooltip: {
              enabled: true, // Ensures tooltips are shown
              mode: 'nearest', // Shows tooltip for the nearest point
              intersect: false, // Allows tooltip to appear even if not directly over a point
              callbacks: {
                label: function (context) {
                  let label = context.dataset.label || ''
                  if (label) {
                    label += ': '
                  }
                  label += context.raw // Shows the raw data value
                  return label
                },
              },
            },
            legend: {
              position: 'top',
            },
          },
        },
      });
    };

    onMounted(() => {
      renderChart();
    })

    // Watch for changes in start_date or end_date and refresh the chart
    watch(dataParams, () => {
      renderChart();
    }, { deep: true });

    return { dataParams }
  },
})
</script>

<style>
/* Add custom styles here if needed */
</style>
