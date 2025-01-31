//
const VERSION = "0.1.0-110";
const express = require("express");
const cors = require("cors");
const fs = require("fs");
const path = require("path");

const app = express();
app.use(cors()); // Enable CORS
const PORT = 3000;

// Load sample temperature data from file
const loadTemperatureData = () => {
    const filePath = path.join(__dirname, "30-minute.txt");
    const data = fs.readFileSync(filePath, "utf-8")
        .trim()
        .split("\n")
        .map(line => {
            const [date, time, tempC, tempF] = line.split(" ");
            return { date, time, tempC: parseFloat(tempC), tempF: parseFloat(tempF) };
        });

    return data;
};

const temperatureData = loadTemperatureData();

// Function to filter data by date range
const filterByDateRange = (data, startDate, endDate) => {
    return data.filter(record => {
        const recordDate = new Date(record.date);
        return (!startDate || recordDate >= new Date(startDate)) &&
               (!endDate || recordDate <= new Date(endDate));
    });
};

// API route to fetch temperature data
app.get("/temperature", (req, res) => {
    let { start_date, end_date, interval } = req.query;

    // Default: Last 25 records if no dates are provided
    let filteredData = temperatureData;
    filteredData = temperatureData.slice(-25);

    // Handle interval filtering
    if (interval) {
        interval = parseInt(interval);
        filteredData = filteredData.filter((_, index) => index % (interval / 30) === 0);
    }

    // Format response
    const labels = filteredData.map(record => record.time);
    const datasets = [
        {
            sensor_id: "sensor_1",
            label: "cottage-south",
            data: filteredData.map(record => record.tempF),
            borderColor: "red",
            fill: false
        },
        {
            sensor_id: "sensor_2",
            label: "shed-west",
            data: filteredData.map(record => (record.tempF + 3.0)),
            borderColor: "blue",
            fill: false
        }
    ];

    // end_date = '2025-01-30';

    res.json({ labels, datasets });
});

// Start the server
app.listen(PORT, () => {
    console.log(`Mock Temperature Service Version ${VERSION} running on http://localhost:${PORT}`);
});
