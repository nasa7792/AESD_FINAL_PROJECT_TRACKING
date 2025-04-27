import React from "react";
import { useState, useEffect } from "react";

function getProximityColor(rawProximity) {
  const prox = parseInt(rawProximity);
  console.log(prox);
  if (isNaN(prox)) return "gray"; // Invalid

  if (prox > 400) return "darkred";
  if (prox > 300) return "red";
  if (prox > 150) return "orange";
  if (prox > 50) return "yellow";
  if (prox > 100) return "green";
  return "blue";
}

function Sensor() {
  const [sensorData, setSensorData] = useState("Loading value...");

  useEffect(() => {
    const interval = setInterval(() => {
      fetch("http://10.0.0.103:8080/")
        .then((response) => response.text())
        .then((text) => setSensorData(text))
        .catch((err) => setSensorData("Error fetching data"));
    }, 1000); // called every second to check for new data

    return () => clearInterval(interval);
  }, []);
  console.log(sensorData);
  return (
    <div
      style={{
        color: getProximityColor(sensorData),
        fontSize: "2rem",
        fontWeight: "bold",
      }}
    >
      Value from Proximity: {sensorData}
    </div>
  );
}

export default Sensor;
