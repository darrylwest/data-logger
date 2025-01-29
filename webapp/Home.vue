<template>
  <div class="container mx-auto pt-10">
    <h2 class="text-3xl font-bold text-center bg-gray-100">Home Page</h2>
    <div class="mt-10">
      <canvas id="lineChart"></canvas>
    </div>
  </div>
</template>

<script>
import { defineComponent, onMounted } from 'vue';
import { Chart, registerables } from 'chart.js';

Chart.register(...registerables);

export default defineComponent({
  name: 'HomePage',
  setup() {
    onMounted(() => {
      const ctx = document.getElementById('lineChart').getContext('2d');
      new Chart(ctx, {
        type: 'line',
        data: {
          labels: ['4am', '', '5am', '', '6am', '', '7am', '', '8am', '', '9am', '', '10am', '', '11am', '', 'Noon', '', '1pm', '', '2pm', '', '3pm', '', '4pm'],
          datasets: [
            {
              label: 'Temperature Data - Cottage-South - Jan 28, 2025',
              data: [42.5, 42.9, 43.2, 42.2, 41.4, 40.8, 40.3, 40.4, 40.4, 43.2, 43.9, 45.3, 50.2, 52.3, 54.1, 55.3, 56.4, 58.2, 59.8, 60.2, 60.9, 59.3, 57.8, 56.9, 56.1],
              fill: false,
              borderColor: 'rgba(75, 192, 192, 1)',
              tension: 0.1,
            },
          ],
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
