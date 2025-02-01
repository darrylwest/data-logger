//
// import axios from "axios";
//
import axios from 'axios'

// alamo 10.0.1.237
// const API_URL = "http://10.0.1.237:2090/api/temperature"; // Replace with actual API URL
// const API_URL = 'http://10.0.1.192:3000/api/temps' // Replace with actual API URL
// const API_URL = 'http://localhost:9999/api/temps' // Replace with actual API URL
// const API_URL = 'http://10.0.1.192:9999/api/temps' // Replace with actual API URL
const API_URL = 'http://10.0.1.237:9999/api/temps' // Replace with actual API URL


export default {
  async fetchTemperatureData(end_date, interval) {
    try {
      const response = await axios.get(API_URL, {
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
