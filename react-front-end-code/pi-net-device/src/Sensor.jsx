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
      fetch("http://172.20.10.3:8080/")
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
        backgroundColor: "#87CEEB", // fallback background
      }}
    >
      {/*create an effect of background scroll*/}
      <div
        style={{
          backgroundImage:
            'url("https://static.vecteezy.com/system/resources/thumbnails/004/867/154/small_2x/an-empty-straight-road-near-the-river-forest-on-the-horizon-cartoon-style-illustration-vector.jpg")',
          backgroundRepeat: "repeat-x",
          backgroundSize: "auto 100%", // keep original width, fit height
          backgroundPosition: "bottom", // align to bottom if needed
          width: "200%",
          height: "100%",
          animation: `scroll ${scrollSpeed} linear infinite`,
          animationPlayState: scrollSpeed === "20s" ? "paused" : "running",
        }}
      />

      {/* car image remains static */}
      <img
        src="https://upload.wikimedia.org/wikipedia/commons/thumb/3/38/Red_Car_Closed_Window_Cartoon_Vector.svg/2560px-Red_Car_Closed_Window_Cartoon_Vector.svg.png"
        alt="Car"
        style={{
          position: "absolute",
          bottom: "10%",
          left: "50%",
          transform: "translateX(-50%)",
          width: "200px", // adjust size as needed
          height: "auto",
          zIndex: 2,
        }}
      />

      {/* display proximity data */}
      <div
        style={{
          position: "absolute",
          top: 20,
          left: 20,
          color: "white",
          fontSize: "2rem",
          fontWeight: "bold",
          textShadow: "0 0 10px black",
          zIndex: 3,
        }}
      >
        Proximity: {sensorData}
      </div>

      {/* Keyframes */}
      <style>
        {`
    @keyframes scroll {
      0% { transform: translateX(0); }
      100% { transform: translateX(-50%); }
    }
  `}
      </style>
    </div>
  );
}

export default Sensor;
