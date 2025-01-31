//
const VERSION = "0.1.0-118";
const express = require("express");
const cors = require("cors");
const fs = require("fs");
const path = require("path");
const { setMaxIdleHTTPParsers } = require("http");
const { threadId } = require("worker_threads");

const app = express();
app.use(cors()); // Enable CORS
const PORT = 3000;

// Load sample temperature data from file
const loadTemperatureData = (interval) => {
    const filePath = path.join(__dirname, `${interval}-minute.txt`);
    console.log("read file: ", filePath);
    const data = fs.readFileSync(filePath, "utf-8")
        .trim()
        .split("\n")
        .map(line => {
            const [date, time, tempC, tempF] = line.split(" ");
            return { date, time, tempC: parseFloat(tempC), tempF: parseFloat(tempF) };
        });

    return data;
};

// Function to filter data by date range
const filterByDateRange = (data, endDate) => {
    return data.filter(record => {
        const recordDate = new Date(record.date);
        return (!endDate || recordDate <= new Date(endDate));
    });
};

// API route to fetch temperature data
app.get("/temperature", (req, res) => {
    let { end_date, interval } = req.query;

    console.log("end: ", end_date, ", interval: ", interval);

    // Handle interval filtering
    if (interval) {
        interval = parseInt(interval);
    } else {
      interval = 30;
    }

    // Default: Last 25 records if no dates are provided
    const temperatureData = loadTemperatureData(interval);
    filteredData = temperatureData.slice(-25);

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
            data: filteredData.map(record => (record.tempF + (Math.random() * 2 - 1))),
            borderColor: "blue",
            fill: false
        }
    ];

    // simulate a slow connection
    setTimeout(() => {
      res.json({ labels, datasets });
    }, 1000);
});

// Start the server
app.listen(PORT, () => {
    console.log(`Mock Temperature Service Version ${VERSION} running on http://localhost:${PORT}`);
});
