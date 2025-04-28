import React, { useState, useEffect } from "react";

function getScrollSpeed(rawProximity) {
  const prox = parseInt(rawProximity);
  if (isNaN(prox)) return "20s";

  // Invert the logic: closer = faster
  if (prox < 50) return "2s";
  if (prox < 100) return "4s";
  if (prox < 200) return "6s";
  if (prox < 300) return "10s";
  if (prox < 500) return "15s";
  return "20s";
}

function Sensor() {
  const [sensorData, setSensorData] = useState("Loading...");

  useEffect(() => {
    const interval = setInterval(() => {
      fetch("http://10.0.0.103:8080/")
        .then((response) => response.text())
        .then((text) => setSensorData(text))
        .catch(() => setSensorData("Error"));
    }, 1000);

    return () => clearInterval(interval);
  }, []);

  const scrollSpeed = getScrollSpeed(sensorData);

  return (
    <div
      style={{
        position: "relative",
        overflow: "hidden",
        width: "100vw",
        height: "100vh",
      }}
    >
<div
  style={{
    backgroundImage: 'url("https://www.clker.com/cliparts/K/p/0/3/I/e/purple-car-hi.png")',
    backgroundRepeat: "no-repeat",
    backgroundSize: "100px auto", // resize image
    backgroundPosition: "center", // or 'center center' etc.
    width: "200%",
    height: "100%",
    animation: `scroll ${scrollSpeed} linear infinite`,
   animationPlayState: scrollSpeed === "20s" ? "paused" : "running",
  }}
/>

      <div
        style={{
          position: "absolute",
          top: 20,
          left: 20,
          color: "black",
          fontSize: "2rem",
          fontWeight: "bold",
          textShadow: "0 0 10px black",
        }}
      >
        Proximity: {sensorData}
      </div>

      <style>
        {`
          @keyframes scroll {
  0% { transform: translateX(-50%); }
  100% { transform: translateX(0); }
          }
        `}
      </style>
    </div>
  );
}

export default Sensor;
