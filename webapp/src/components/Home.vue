<template>
  <div class="container mx-auto pt-10">
    <h2 class="text-3xl font-bold text-center">Temperature Readings</h2>
    <h3 class="font-bold text-center ">2025-01-28 04:00:00 to 2025-01-28 16:00:00</h3>
    <div class="mt-4 mb-20">
      <canvas id="lineChart"></canvas>
    </div>
  </div>
</template>

<script>
import { defineComponent, onMounted } from 'vue';
import { Chart, registerables } from 'chart.js';
import TemperatureService from "@/services/TemperatureService"

Chart.register(...registerables);

export default defineComponent({
  name: 'HomePage',
  setup() {
    onMounted(() => {
      const ctx = document.getElementById('lineChart').getContext('2d');
      new Chart(ctx, {
        type: 'line',
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
                  let label = context.dataset.label || '';
                  if (label) {
                    label += ': ';
                  }
                  label += context.raw; // Shows the raw data value
                  return label;
                },
              },
            },
            legend: {
              position: 'top',
            },
          },
        },
      });
    });

    return {};
  },
});
</script>

<style>
/* Add custom styles here if needed */
</style>

