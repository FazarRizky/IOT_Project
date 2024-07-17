const express = require('express');
const bodyParser = require('body-parser');
const mongoose = require('mongoose');

const app = express();
const port = 3001; // Ubah ke port yang sesuai
const mongoUrl = 'mongodb://localhost:27017/iot_data'; // Pastikan URL ini benar

mongoose.connect(mongoUrl).then(
  () => { console.log('Connected to MongoDB'); },
  err => { console.log('Failed to connect to MongoDB', err); }
);

const sensorDataSchema = new mongoose.Schema({
  temperature: Number,
  humidity: Number,
  air_quality: Number,
  timestamp: { type: Date, default: Date.now }
});

const SensorData = mongoose.model('SensorData', sensorDataSchema);

app.use(bodyParser.json());

app.post('/api/data', async (req, res) => {
  try {
    const data = new SensorData(req.body);
    await data.save();
    res.status(200).send('Data saved to MongoDB');
  } catch (err) {
    res.status(500).send('Error saving data to MongoDB');
  }
});

app.get('/api/data', async (req, res) => {
  try {
    const data = await SensorData.find({});
    res.status(200).json(data);
  } catch (err) {
    res.status(500).send('Error retrieving data from MongoDB');
  }
});

app.listen(port, () => {
  console.log(`Server listening at http://localhost:${port}`);
});
