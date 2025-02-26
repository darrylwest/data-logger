//
// import axios from "axios";
//
import axios from 'axios'

// alamo 10.0.1.237
const TEMPS_API_URL = import.meta.env.VITE_API_URL;

export default {
  async fetchTemperatureData(end_date, interval) {
    try {
      const response = await axios.get(TEMPS_API_URL, {
        params: { end_date, interval },
      })

      console.log('response data: ', response.data)

      if (!response.data || !response.data.labels) {
        console.error('missing labels in response.')
        return { labels: [], datasets: [] }
      }

      return {
        labels: response.data.labels,
        datasets: response.data.datasets || [],
      }
    } catch (error) {
      console.error('API request failed:', error)
      throw error
    }
  },
}
