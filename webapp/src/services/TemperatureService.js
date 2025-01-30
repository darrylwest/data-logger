//
// import axios from "axios";
//
import axios from "axios";

// alamo 10.0.1.237
// const API_URL = "http://10.0.1.237:2090/api/temperature"; // Replace with actual API URL
const API_URL = "http://tiburon.local:3000/temperature"; // Replace with actual API URL

export default {
  async fetchTemperatureData(start_date, end_date, interval) {
    try {
      const response = await axios.get(API_URL, {
        params: { start_date, end_date, interval },
      });
      return response.data;
    } catch (error) {
      console.error("API request failed:", error);
      throw error;
    }
  },
}

/*
const TemperatureService = {
  dataParams: () => {
    return {
      start_date: '2025-01-29 04:00',
      end_date: '2025-01-29 16:00',
      interval: '30',
    }
  },

  fetchReadings: () => {
    const temps = [
      {
        label: 'Cottage-South',
        data: [
          42.5, 42.9, 43.2, 42.2, 41.4, 40.8, 40.3, 40.4, 40.4, 43.2, 43.9, 45.3, 50.2, 52.3, 54.1,
          55.3, 56.4, 58.2, 59.8, 60.2, 60.9, 59.3, 57.8, 56.9, 56.1,
        ],
        fill: false,
        borderColor: 'rgba(75, 192, 192, 1)',
        tension: 0.1,
      },
      {
        label: 'Shed-west',
        data: [
          43.5, 43.9, 44.2, 43.2, 42.4, 41.8, 41.3, 41.4, 41.4, 44.2, 44.9, 46.3, 51.2, 52.3, 53.1,
          54.3, 55.4, 57.2, 58.8, 61.2, 61.9, 60.3, 57.8, 56.3, 55.4,
        ],
        fill: false,
        borderColor: 'rgba(192, 75, 75, 1)',
        tension: 0.1,
      },
    ]

    return temps
  },

  fetchLabels: () => {
    const labels = [
      '04:00',
      '04:30',
      '05:00',
      '05:30',
      '06:00',
      '06:30',
      '07:00',
      '07:30',
      '08:00',
      '08:30',
      '09:00',
      '09:30',
      '10:00',
      '10:30',
      '11:00',
      '11:30',
      '12:00',
      '12:30',
      '13:00',
      '13:30',
      '14:00',
      '14:30',
      '15:00',
      '15:30',
      '16:00',
    ]

    return labels
  },
}

*/
