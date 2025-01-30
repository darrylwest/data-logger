const express = require("express");
const cors = require("cors");

const app = express();
app.use(cors()); // Enable CORS
const PORT = 3000;

// Mock sensors
const SENSOR_IDS = ["sensor_1", "sensor_2", "sensor_3"];

// Function to generate mock temperature data
const generateTemperatureData = (start, end, interval) => {
    let timestamps = [];
    let datasets = SENSOR_IDS.map(id => ({
        sensor_id: id,
        label: `Temperature (${id})`,
        data: []
    }));

    let currentTime = new Date(start).getTime();
    let endTime = new Date(end).getTime();
    let intervalMs = interval * 60 * 1000;

    while (currentTime <= endTime) {
        const localTime = new Date(currentTime).toLocaleString("en-US", { 
            timeZone: "America/Los_Angeles", // Adjust to your local timezone
            hour12: false // Ensures 24-hour format
        });

        timestamps.push(localTime);

        datasets.forEach(sensor => {
            sensor.data.push((Math.random() * 10 + 20).toFixed(2)); // Random 20-30°C
        });

        currentTime += intervalMs;
    }

    return { labels: timestamps, datasets };
};

// API route to fetch temperature data
app.get("/temperature", (req, res) => {
    const { start_date, end_date, interval } = req.query;

    if (!start_date || !end_date || !interval) {
        return res.status(400).json({ error: "Missing required query parameters" });
    }

    const { labels, datasets } = generateTemperatureData(start_date, end_date, parseInt(interval));

    res.json({ labels, datasets });
});

// Start the server
app.listen(PORT, () => {
    console.log(`Mock Temperature Service running on http://0.0.0.0:${PORT}`);
});
