import TemperatureService from "./src/services/TemperatureService.js"; // Adjust path if needed

const testTemperatureAPI = async () => {
  try {
    const start_date = "2025-01-28T00:00";
    const end_date = "2025-01-28T23:59";
    const interval = 30;

    console.log("Fetching temperature data...");

    const response = await TemperatureService.fetchTemperatureData(start_date, end_date, interval);

    console.log("✅ API Response:", response.data);
    //console.log("📊 Datasets:", response.datasets);
    //console.log("📊 Datasets:", response.datasets[0]);
    //console.log("📌 Labels:", response.data.labels);

    /*
    if (!Array.isArray(response.data.labels) || response.data.labels.length === 0) {
      console.error("❌ ERROR: labels array is empty!");
    } else {
      console.log("🎉 SUCCESS: labels array contains data!");
    }
    */
  } catch (error) {
    console.error("❌ API request failed:", error);
  }
};

testTemperatureAPI();

