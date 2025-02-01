import TemperatureService from "./src/services/TemperatureService.js"; // Adjust path if needed

const testTemperatureAPI = async () => {
  try {
    const end_date = "2025-01-30T16:00";
    const interval = 30;

    console.log("Fetching temperature data...");

    const response = await TemperatureService.fetchTemperatureData(end_date, interval);

    console.log("API Response:", response);
    // console.log("📊 Datasets:", response.datasets);
    //console.log("📊 Datasets:", response.datasets[0]);
    //console.log("📌 Labels:", response.data.labels);

  } catch (error) {
    console.error("❌ API request failed:", error);
  }
};

testTemperatureAPI();

