<template>
  <div class="container mx-auto pt-10">
    <h2 class="text-3xl font-bold text-center">Temperature Readings</h2>
    <h3 class="font-bold text-center ">January 28th, 2025 - 4am to 4pm</h3>
    <div class="mt-4 mb-20">
      <canvas id="lineChart"></canvas>
    </div>
  </div>
</template>

<script>
import { defineComponent, onMounted } from 'vue';
import { Chart, registerables } from 'chart.js';

Chart.register(...registerables);

// TODO : move this to an external file; passing start/stop datetime and interval (10min, 30min, 1 hour, etc.)
function fetchReadings() {
  const temps = [
    {
      label: 'Cottage-South',
      data: [42.5, 42.9, 43.2, 42.2, 41.4, 40.8, 40.3, 40.4, 40.4, 43.2, 43.9, 45.3, 50.2, 52.3, 54.1, 55.3, 56.4, 58.2, 59.8, 60.2, 60.9, 59.3, 57.8, 56.9, 56.1],
      fill: false,
      borderColor: 'rgba(75, 192, 192, 1)',
      tension: 0.1,
    },
    {
      label: 'Shed-west',
      data: [43.5, 43.9, 44.2, 43.2, 42.4, 41.8, 41.3, 41.4, 41.4, 44.2, 44.9, 46.3, 51.2, 52.3, 53.1, 54.3, 55.4, 57.2, 58.8, 61.2, 61.9, 60.3, 57.8, 56.3, 55.4],
      fill: false,
      borderColor: 'rgba(192, 75, 75, 1)',
      tension: 0.1,
    },
  ]

  return temps;
}

export default defineComponent({
  name: 'HomePage',
  setup() {
    onMounted(() => {
      const ctx = document.getElementById('lineChart').getContext('2d');
      new Chart(ctx, {
        type: 'line',
        data: {
          labels: ['4am', '', '5am', '', '6am', '', '7am', '', '8am', '', '9am', '', '10am', '', '11am', '', 'Noon', '', '1pm', '', '2pm', '', '3pm', '', '4pm'],
          datasets: fetchReadings(),
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

