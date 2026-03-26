#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

Servo miServo;
// Variables para animacion pantalla oled 
int contadorPuntos = 0;
String puntos = "";

// Variables de la casa
bool sl = false;
bool ba = false;
bool cc = false;
bool hb = false;
bool bm = false;
bool ser = false;

// Credenciales WiFi
const char* ssid = "Tl";
const char* password = "123456789";

// Pines
const int PINSERVO = 15;
const int slp = 4;
const int bap = 18;
const int ccp = 5;
const int hbp = 2;
const int bmp = 23;

WebServer server(80);

void mostrarEstado(String linea1, String linea2, String linea3) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  // Subido 1 píxel (y=9 en lugar de 10)
  display.setCursor(0, 9);
  display.setTextSize(1);
  display.println(linea1);
  
  // Subido 1 píxel (y=29 en lugar de 30)
  display.setCursor(0, 29);
  display.setTextSize(1);
  display.println(linea2);
  
  // Subido 1 píxel (y=29 en lugar de 30)
  display.setCursor(0, 40);
  display.setTextSize(1);
  display.println(linea3);

  display.display();
}

// ==== Función para conectarse al WiFi ====
void conectarWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    contadorPuntos++;
    if (contadorPuntos > 3) {
      contadorPuntos = 0;
      puntos = "";
    } else {
      puntos += ".";
    }
    mostrarEstado("Conectando" + puntos, "", "");
    delay(500); // Velocidad de la animación
    Serial.print(".");
  }
  Serial.println("\n✅ Conectado a WiFi");
  Serial.println("📡 IP local: " + WiFi.localIP().toString());
  mostrarEstado("WiFi: CONECTADO", "IP Local:", WiFi.localIP().toString());
}

// ==== Función que genera la página HTML ====
String generarPaginaHTML() {
String html = R"raw(
<!DOCTYPE html>
<html lang='es'>
<head>
    <title>Smart Control Panel</title>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <link rel='shortcut icon' href='https://cdn.jsdelivr.net/gh/Gouveira/SCP@main/images.png'>
    <!-- <link rel='stylesheet' href='https://cdn.jsdelivr.net/gh/Gouveira/SCP@main/style.css'> -->
</head>
<style>
  :root {
            --primary: #1a3a5c;
            --primary-light: #2e6b9e;
            --accent: #00e5ff;
            --accent-glow: rgba(0, 229, 255, 0.3);
            --bg-dark: #0a1628;
            --bg-mid: #0f2035;
            --glass: rgba(255, 255, 255, 0.06);
            --glass-border: rgba(255, 255, 255, 0.1);
            --text-primary: #e8f4fd;
            --text-secondary: #8badc4;
        }

        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            font-family: 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
            background: var(--bg-dark);
            color: var(--text-primary);
            overflow: hidden;
            position: relative;
        }

        /* === ANIMATED BACKGROUND LAYERS === */

        .bg-mesh {
            position: fixed;
            inset: 0;
            z-index: 0;
            background:
                radial-gradient(ellipse 80% 60% at 10% 90%, rgba(0, 229, 255, 0.12) 0%, transparent 60%),
                radial-gradient(ellipse 60% 80% at 90% 10%, rgba(30, 80, 160, 0.2) 0%, transparent 60%),
                radial-gradient(ellipse 50% 50% at 50% 50%, rgba(10, 30, 60, 0.8) 0%, transparent 80%);
        }

        .bg-grid {
            position: fixed;
            inset: 0;
            z-index: 0;
            background-image:
                linear-gradient(rgba(0, 229, 255, 0.04) 1px, transparent 1px),
                linear-gradient(90deg, rgba(0, 229, 255, 0.04) 1px, transparent 1px);
            background-size: 60px 60px;
            mask-image: radial-gradient(ellipse 70% 70% at 50% 50%, black 20%, transparent 70%);
            -webkit-mask-image: radial-gradient(ellipse 70% 70% at 50% 50%, black 20%, transparent 70%);
        }

        .orb {
            position: fixed;
            border-radius: 50%;
            filter: blur(80px);
            z-index: 0;
            animation: float 20s ease-in-out infinite;
        }

        .orb-1 {
            width: 500px;
            height: 500px;
            background: radial-gradient(circle, rgba(0, 229, 255, 0.15), transparent 70%);
            top: -150px;
            left: -100px;
            animation-duration: 18s;
        }

        .orb-2 {
            width: 400px;
            height: 400px;
            background: radial-gradient(circle, rgba(30, 80, 200, 0.18), transparent 70%);
            bottom: -100px;
            right: -80px;
            animation-duration: 22s;
            animation-delay: -5s;
        }

        .orb-3 {
            width: 300px;
            height: 300px;
            background: radial-gradient(circle, rgba(0, 180, 220, 0.1), transparent 70%);
            top: 40%;
            left: 60%;
            animation-duration: 25s;
            animation-delay: -10s;
        }

        @keyframes float {
            0%, 100% { transform: translate(0, 0) scale(1); }
            25% { transform: translate(30px, -40px) scale(1.05); }
            50% { transform: translate(-20px, 20px) scale(0.95); }
            75% { transform: translate(40px, 30px) scale(1.02); }
        }

        .particles {
            position: fixed;
            inset: 0;
            z-index: 0;
            overflow: hidden;
        }

        .particle {
            position: absolute;
            width: 3px;
            height: 3px;
            background: var(--accent);
            border-radius: 50%;
            opacity: 0;
            animation: rise linear infinite;
        }

        .particle:nth-child(1) { left: 10%; animation-duration: 12s; animation-delay: 0s; }
        .particle:nth-child(2) { left: 20%; animation-duration: 15s; animation-delay: -2s; width: 2px; height: 2px; }
        .particle:nth-child(3) { left: 35%; animation-duration: 10s; animation-delay: -4s; }
        .particle:nth-child(4) { left: 50%; animation-duration: 18s; animation-delay: -6s; width: 4px; height: 4px; }
        .particle:nth-child(5) { left: 65%; animation-duration: 14s; animation-delay: -3s; width: 2px; height: 2px; }
        .particle:nth-child(6) { left: 75%; animation-duration: 11s; animation-delay: -7s; }
        .particle:nth-child(7) { left: 85%; animation-duration: 16s; animation-delay: -1s; width: 2px; height: 2px; }
        .particle:nth-child(8) { left: 5%;  animation-duration: 13s; animation-delay: -9s; width: 4px; height: 4px; }
        .particle:nth-child(9) { left: 45%; animation-duration: 17s; animation-delay: -5s; }
        .particle:nth-child(10){ left: 92%; animation-duration: 14s; animation-delay: -8s; width: 2px; height: 2px; }
        .particle:nth-child(11){ left: 28%; animation-duration: 19s; animation-delay: -11s; }
        .particle:nth-child(12){ left: 58%; animation-duration: 12s; animation-delay: -3s; width: 3px; height: 3px; }

        @keyframes rise {
            0% {
                transform: translateY(100vh) translateX(0);
                opacity: 0;
            }
            10% {
                opacity: 0.6;
            }
            90% {
                opacity: 0.3;
            }
            100% {
                transform: translateY(-10vh) translateX(40px);
                opacity: 0;
            }
        }

        .scanline {
            position: fixed;
            left: 0;
            width: 100%;
            height: 1px;
            background: linear-gradient(90deg, transparent 0%, var(--accent) 50%, transparent 100%);
            opacity: 0.08;
            z-index: 0;
            animation: scan 8s linear infinite;
        }

        @keyframes scan {
            0% { top: -2%; }
            100% { top: 102%; }
        }

        .circuit-lines {
            position: fixed;
            inset: 0;
            z-index: 0;
            overflow: hidden;
            opacity: 0.06;
        }

        .circuit-lines::before,
        .circuit-lines::after {
            content: '';
            position: absolute;
            background: var(--accent);
        }

        .circuit-lines::before {
            width: 200px;
            height: 1px;
            top: 25%;
            left: 0;
            box-shadow:
                0 0 8px var(--accent),
                250px 80px 0 0 var(--accent),
                250px 80px 8px 0 var(--accent);
        }

        .circuit-lines::after {
            width: 1px;
            height: 150px;
            bottom: 10%;
            right: 15%;
            box-shadow:
                0 0 8px var(--accent),
                -60px -150px 0 0 var(--accent),
                -60px -150px 8px 0 var(--accent);
        }

        .corner-decor {
            position: fixed;
            width: 60px;
            height: 60px;
            z-index: 1;
            opacity: 0;
            animation: cornerFadeIn 0.8s ease-out 1.8s both;
        }

        .corner-decor::before,
        .corner-decor::after {
            content: '';
            position: absolute;
            background: var(--accent);
        }

        .corner-tl { top: 20px; left: 20px; }
        .corner-tl::before { width: 30px; height: 1px; top: 0; left: 0; }
        .corner-tl::after  { width: 1px; height: 30px; top: 0; left: 0; }

        .corner-tr { top: 20px; right: 20px; animation-delay: 1.9s; }
        .corner-tr::before { width: 30px; height: 1px; top: 0; right: 0; }
        .corner-tr::after  { width: 1px; height: 30px; top: 0; right: 0; }

        .corner-bl { bottom: 20px; left: 20px; animation-delay: 2.0s; }
        .corner-bl::before { width: 30px; height: 1px; bottom: 0; left: 0; }
        .corner-bl::after  { width: 1px; height: 30px; bottom: 0; left: 0; }

        .corner-br { bottom: 20px; right: 20px; animation-delay: 2.1s; }
        .corner-br::before { width: 30px; height: 1px; bottom: 0; right: 0; }
        .corner-br::after  { width: 1px; height: 30px; bottom: 0; right: 0; }

        @keyframes cornerFadeIn {
            0% { opacity: 0; }
            100% { opacity: 0.15; }
        }

        /* === MAIN CONTENT === */

        .glass-container {
            position: fixed;
            top: 10px;
            z-index: 2;
            background: var(--glass);
            backdrop-filter: blur(20px);
            -webkit-backdrop-filter: blur(20px);
            border: 1px solid var(--glass-border);
            border-radius: 24px;
            padding: 40px;
            box-shadow:
                0 8px 32px rgba(0, 0, 0, 0.3),
                inset 0 1px 0 rgba(255, 255, 255, 0.05);
            text-align: center;
            width: 90%;
            max-width: 500px;
            animation:
                containerEntrance 1.1s cubic-bezier(0.22, 1, 0.36, 1) 0.15s both,
                glowPulseIn 2.5s ease-in-out 1.2s both;
        }

        .glass-container::before {
            content: '';
            position: absolute;
            inset: -1px;
            border-radius: 24px;
            padding: 1px;
            background: linear-gradient(
                135deg,
                rgba(0, 229, 255, 0.3) 0%,
                transparent 40%,
                transparent 60%,
                rgba(30, 80, 200, 0.2) 100%
            );
            -webkit-mask: linear-gradient(#fff 0 0) content-box, linear-gradient(#fff 0 0);
            mask: linear-gradient(#fff 0 0) content-box, linear-gradient(#fff 0 0);
            -webkit-mask-composite: xor;
            mask-composite: exclude;
            pointer-events: none;
        }

        /* === TITLE LETTER-BY-LETTER ANIMATION === */

        .title {
            font-size: 28px;
            font-weight: 800;
            color: var(--accent);
            text-transform: uppercase;
            letter-spacing: 2px;
            text-shadow: 0 0 20px var(--accent-glow);
            display: flex;
            justify-content: center;
            gap: 2px;
            flex-wrap: wrap;
        }

        .title .letter {
            display: inline-block;
            opacity: 0;
            transform: translateY(-20px) scale(0.6) rotateX(90deg);
            filter: blur(4px);
            animation: letterDrop 0.5s cubic-bezier(0.22, 1, 0.36, 1) both;
        }

        .title .letter-space {
            display: inline-block;
            width: 10px;
        }

        @keyframes letterDrop {
            0% {
                opacity: 0;
                transform: translateY(-20px) scale(0.6) rotateX(90deg);
                filter: blur(4px);
                text-shadow: none;
            }
            60% {
                opacity: 1;
                transform: translateY(2px) scale(1.05) rotateX(0deg);
                filter: blur(0);
            }
            100% {
                opacity: 1;
                transform: translateY(0) scale(1) rotateX(0deg);
                filter: blur(0);
                text-shadow: 0 0 20px var(--accent-glow);
            }
        }

        /* === SUBTITLE TYPEWRITER === */

        .title2-wrapper {
            overflow: hidden;
            margin-bottom: 30px;
            display: flex;
            justify-content: center;
            align-items: center;
            gap: 2px;
        }

        .title2 {
            font-size: 16px;
            font-weight: 600;
            color: var(--text-secondary);
            text-transform: uppercase;
            letter-spacing: 3px;
            display: flex;
            gap: 1px;
        }

        .title2 .char {
            display: inline-block;
            opacity: 0;
            animation: typeChar 0.06s ease-out both;
        }

        .title2 .char-space {
            display: inline-block;
            width: 6px;
        }

        @keyframes typeChar {
            0% { opacity: 0; transform: translateX(-2px); }
            100% { opacity: 1; transform: translateX(0); }
        }

        /* === STATUS BAR ENTRANCE === */

        .status-bar {
            display: flex;
            justify-content: center;
            gap: 8px;
            margin-bottom: 25px;
            margin-top: 5px;
        }

        .status-dot {
            width: 6px;
            height: 6px;
            border-radius: 50%;
            background: var(--accent);
            opacity: 0;
            transform: scale(0);
            animation: dotAppear 0.5s cubic-bezier(0.22, 1, 0.36, 1) both;
        }

        @keyframes dotAppear {
            0% {
                opacity: 0;
                transform: scale(0);
            }
            50% {
                opacity: 1;
                transform: scale(1.6);
            }
            100% {
                opacity: 0.5;
                transform: scale(1);
            }
        }

        .status-dot.live {
            opacity: 1;
            transform: scale(1);
            animation: pulse-dot 2s ease-in-out infinite;
        }

        .status-dot:nth-child(2).live { animation-delay: 0.3s; }
        .status-dot:nth-child(3).live { animation-delay: 0.6s; }

        @keyframes pulse-dot {
            0%, 100% { opacity: 0.3; }
            50% { opacity: 1; }
        }

        /* === GRID BUTTONS ENTRANCE === */

        .grid-botones {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 12px;
        }

        .grid-botones > * {
            opacity: 0;
            transform: translateY(20px);
        }

        .grid-botones > *.revealed {
            animation: cardReveal 0.5s cubic-bezier(0.22, 1, 0.36, 1) both;
        }

        @keyframes cardReveal {
            0% {
                opacity: 0;
                transform: translateY(20px) scale(0.95);
            }
            100% {
                opacity: 1;
                transform: translateY(0) scale(1);
            }
        }

        .btn {
            background: rgba(255, 255, 255, 0.04);
            border: 1px solid rgba(255, 255, 255, 0.08);
            border-radius: 16px;
            color: var(--text-primary);
            padding: 20px 10px;
            font-size: 14px;
            font-weight: 600;
            text-decoration: none;
            cursor: pointer;
            transition: all 0.3s ease;
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 8px;
            position: relative;
            overflow: hidden;
        }

        .btn::before {
            content: '';
            position: absolute;
            inset: 0;
            background: linear-gradient(135deg, rgba(0, 229, 255, 0.1), transparent);
            opacity: 0;
            transition: opacity 0.3s ease;
        }

         .btn:hover {
            border-color: var(--accent);
            transform: translateY(-3px);
            box-shadow: 0 8px 25px rgba(0, 229, 255, 0.15);
        }

        .btn:hover::before {
            opacity: 1;
        }

        .btn:active {
            transform: translateY(0);
        }

        .btn-off:hover {
            border-color: #ff4757;
            box-shadow: 0 8px 25px rgba(255, 71, 87, 0.2);
        }

        .btn-off:hover::before {
            background: linear-gradient(135deg, rgba(255, 71, 87, 0.1), transparent);
        }

        .section-label {
            grid-column: span 2;
            text-align: left;
            font-size: 11px;
            color: var(--text-secondary);
            margin-top: 10px;
            font-weight: 700;
            text-transform: uppercase;
            letter-spacing: 2px;
            padding-left: 4px;
            border-left: 2px solid var(--accent);
        }

        /* === LOGO IMAGE ENTRANCE === */

        .top-link {
            position: fixed;
            top: 20px;
            right: 20px;
            z-index: 10;
            opacity: 0;
            animation: logoEntrance 0.9s cubic-bezier(0.22, 1, 0.36, 1) 0.3s both;
        }

        .top-link img {
            width: 60px;
            height: 60px;
            border-radius: 50%;
            border: 2px solid rgba(0, 229, 255, 0.4);
            transition: all 0.4s cubic-bezier(0.22, 1, 0.36, 1);
            box-shadow: 0 0 15px rgba(0, 229, 255, 0.15);
        }

        .top-link img:hover {
            border-color: var(--accent);
            box-shadow:
                0 0 25px rgba(0, 229, 255, 0.3),
                0 0 50px rgba(0, 229, 255, 0.1);
            transform: scale(1.08);
        }

        @keyframes logoEntrance {
            0% {
                opacity: 0;
                transform: translateY(-30px) scale(0.3) rotate(-180deg);
                filter: blur(10px);
            }
            50% {
                opacity: 1;
                filter: blur(0);
            }
            70% {
                transform: translateY(4px) scale(1.1) rotate(10deg);
            }
            85% {
                transform: translateY(-2px) scale(0.98) rotate(-3deg);
            }
            100% {
                opacity: 1;
                transform: translateY(0) scale(1) rotate(0deg);
                filter: blur(0);
            }
        }

        .top-link2 {
            position: fixed;
            top: 20px;
            left: 20px;
            z-index: 10;
            opacity: 0;
            animation: logoEntrance2 0.9s cubic-bezier(0.22, 1, 0.36, 1) 0.3s both;
        }

        .top-link2 img {
            width: 60px;
            height: 60px;
            border-radius: 50%;
            border: 2px solid rgba(0, 229, 255, 0.4);
            transition: all 0.4s cubic-bezier(0.22, 1, 0.36, 1);
            box-shadow: 0 0 15px rgba(0, 229, 255, 0.15);
        }

        .top-link2 img:hover {
            border-color: var(--accent);
            box-shadow:
                0 0 25px rgba(0, 229, 255, 0.3),
                0 0 50px rgba(0, 229, 255, 0.1);
            transform: scale(1.08);
        }

        @keyframes logoEntrance2 {
            0% {
                opacity: 0;
                transform: translateY(-30px) scale(0.3) rotate(-180deg);
                filter: blur(10px);
            }
            50% {
                opacity: 1;
                filter: blur(0);
            }
            70% {
                transform: translateY(4px) scale(1.1) rotate(10deg);
            }
            85% {
                transform: translateY(-2px) scale(0.98) rotate(-3deg);
            }
            100% {
                opacity: 1;
                transform: translateY(0) scale(1) rotate(0deg);
                filter: blur(0);
            }
        }
        .top-link2r {
            position: fixed;
            top: 20px;
            left: 20px;
            z-index: 10;
            opacity: 0;
            animation: logoEntrance2r 0.9s cubic-bezier(0.22, 1, 0.36, 1) 0.3s both;
        }
        .top-link2r img {
            width: 60px;
            height: 60px;
            border-radius: 50%;
            border: 2px solid rgba(0, 229, 255, 0.4);
            transition: all 0.4s cubic-bezier(0.22, 1, 0.36, 1);
            box-shadow: 0 0 15px rgba(0, 229, 255, 0.15);
        }

        .top-link2r img:hover {
            border-color: var(--accent);
            box-shadow:
                0 0 25px rgba(255, 71, 87, 0.2),
                0 0 50px rgba(255, 71, 87, 0.1);
            transform: scale(1.08);
        }

        @keyframes logoEntrance2r {
            0% {
                opacity: 0;
                transform: translateY(-30px) scale(0.3) rotate(-180deg);
                filter: blur(10px);
            }
            50% {
                opacity: 1;
                filter: blur(0);
            }
            70% {
                transform: translateY(4px) scale(1.1) rotate(10deg);
            }
            85% {
                transform: translateY(-2px) scale(0.98) rotate(-3deg);
            }
            100% {
                opacity: 1;
                transform: translateY(0) scale(1) rotate(0deg);
                filter: blur(0);
            }
        }
        /* === CREDITS ENTRANCE === */

        .credit {
            position: fixed;
            bottom: 20px;
            left: 25px;
            z-index: 10;
            color: var(--text-secondary);
            font-weight: 500;
            font-size: 12px;
            letter-spacing: 0.5px;
            line-height: 1.6;
            opacity: 0;
            animation: creditSlideIn 0.8s ease-out 2.2s both;
        }

        @keyframes creditSlideIn {
            0% {
                opacity: 0;
                transform: translateX(-20px);
                filter: blur(4px);
            }
            100% {
                opacity: 1;
                transform: translateX(0);
                filter: blur(0);
            }
        }

        /* === CONTAINER ENTRANCE === */

        @keyframes containerEntrance {
            0% {
                opacity: 0;
                transform: translateY(50px) scale(0.92);
                filter: blur(12px);
            }
            50% {
                opacity: 1;
                filter: blur(0);
            }
            70% {
                transform: translateY(-6px) scale(1.015);
            }
            85% {
                transform: translateY(2px) scale(0.998);
            }
            100% {
                opacity: 1;
                transform: translateY(0) scale(1);
                filter: blur(0);
            }
        }

        @keyframes glowPulseIn {
            0% {
                box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3), inset 0 1px 0 rgba(255, 255, 255, 0.05);
            }
            40% {
                box-shadow:
                    0 8px 32px rgba(0, 0, 0, 0.3),
                    inset 0 1px 0 rgba(255, 255, 255, 0.05),
                    0 0 60px rgba(0, 229, 255, 0.2),
                    0 0 120px rgba(0, 229, 255, 0.06);
            }
            100% {
                box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3), inset 0 1px 0 rgba(255, 255, 255, 0.05);
            }
        }

        @media (max-width: 520px) {
            .glass-container {
                padding: 25px 20px;
            }
            .title {
                font-size: 22px;
            }
            .credit {
                font-size: 10px;
            }

        }
</style>
<body>

    <!-- Background layers -->
    <div class='bg-mesh'></div>
    <div class='bg-grid'></div>

    <div class='orb orb-1'></div>
    <div class='orb orb-2'></div>
    <div class='orb orb-3'></div>

    <div class='particles'>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
    </div>

    <div class='scanline'></div>
    <div class='circuit-lines'></div>

    <!-- Corner decorations -->
    <div class='corner-decor corner-tl'></div>
    <div class='corner-decor corner-tr'></div>
    <div class='corner-decor corner-bl'></div>
    <div class='corner-decor corner-br'></div>

    <!-- Logo -->
    <a href='https://belagua.com.ve/' target='_blank' class='top-link'>
        <img src='https://cdn.jsdelivr.net/gh/Gouveira/SCP@main/images.png' alt='Belagua-icon'>
    </a>
    <!-- Credits -->
    <div class='credit'>
        Daniel Gouveia /
        Lenin Espinoza /
        Susej Villamizar
    </div>

    <!-- Panel Principal -->
    <div class='glass-container'>
        <div class='title' id='mainTitle'></div>
        <div class='title2-wrapper'>
            <div class='title2' id='subTitle'></div>
        </div>
        <div class='status-bar' id='statusBar'>
            <div class='status-dot' id='dot1'></div>
            <div class='status-dot' id='dot2'></div>
            <div class='status-dot' id='dot3'></div>
        </div>

        <div class='grid-botones' id='gridBotones'>
            <div class='section-label'>Luces de la casa</div>)raw";
          if (sl == false) {
          html += R"raw(<button class='btn' onclick='location.href="/sl"'>&#x1F7E2; Encender Habitacion 1</button>)raw";
          } else if (sl == true) {
          html += R"raw(<button class='btn btn-off' onclick='location.href="/sl"'>&#x1F534; Apagar Habitación 1</button>)raw";
          }
          if (ba == false) {
          html += R"raw(<button class='btn' onclick='location.href="/ba"'>&#x1F7E2; Encender Habitación 2</button>)raw";
          } else if (ba == true) {
          html += R"raw(<button class='btn btn-off' onclick='location.href="/ba"'>&#x1F534; Apagar Habitación 2</button>)raw";
          }
          if (cc == false) {
          html += R"raw(<button class='btn' onclick='location.href="/cc"'>&#x1F7E2; Encender Habitación 3</button>)raw";
          } else if (cc == true) {
          html += R"raw(<button class='btn btn-off' onclick='location.href="/cc"'>&#x1F534; Apaga Habitación 3</button>)raw";
          }
          if (hb == false) {
          html += R"raw(<button class='btn' onclick='location.href="/hb"'>&#x1F7E2; Encender Habitación 4</button>)raw";
          } else if (hb == true) {
          html += R"raw(<button class='btn btn-off' onclick='location.href="/hb"'>&#x1F534; Apaga Habitación 4</button>)raw";
          }

          html += R"raw(<div class='section-label'>Extras</div>)raw";

          if (ser == false) {
          html += R"raw(<button class='btn' onclick='location.href="/servo"'>&#x1F7E2; Abrir Portón</button>)raw";
          } else if (ser == true) {
          html += R"raw(<button class='btn btn-off' onclick='location.href="/servo"'>&#x1F534; Cerrar Portón</button>)raw";
          }
          if (bm == false) {
          html += R"raw(<button class='btn' onclick='location.href="/bm"'>&#x1F7E2; Encender Bombillo</button>)raw";
          } else if (bm == true) {
          html += R"raw(<button class='btn btn-off' onclick='location.href="/bm"'>&#x1F534; Apaga Bombillo</button>)raw";
          }
        html += R"raw(</div>
    </div>

    
    <script src='https://cdn.jsdelivr.net/gh/Gouveira/SCP@main/script.js' defer></script>

</body>
</html>
)raw";
return html;
}

// ==== Función que genera la página HTML ====
String generarPaginasin() {
String html = R"raw(
<!DOCTYPE html>
<html lang='es'>
<head>
    <title>Smart Control Panel</title>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <link rel='shortcut icon' href='https://cdn.jsdelivr.net/gh/Gouveira/SCP@main/images.png'>
    <!-- <link rel='stylesheet' href='https://cdn.jsdelivr.net/gh/Gouveira/SCP@main/BT/style.css'> -->
</head>
<style>
  :root {
            --primary: #1a3a5c;
            --primary-light: #2e6b9e;
            --accent: #00e5ff;
            --accent-glow: rgba(0, 229, 255, 0.3);
            --bg-dark: #0a1628;
            --bg-mid: #0f2035;
            --glass: rgba(255, 255, 255, 0.06);
            --glass-border: rgba(255, 255, 255, 0.1);
            --text-primary: #e8f4fd;
            --text-secondary: #8badc4;
        }

        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            font-family: 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
            background: var(--bg-dark);
            color: var(--text-primary);
            overflow: hidden;
            position: relative;
        }

        /* === ANIMATED BACKGROUND LAYERS === */

        .bg-mesh {
            position: fixed;
            inset: 0;
            z-index: 0;
            background:
                radial-gradient(ellipse 80% 60% at 10% 90%, rgba(0, 229, 255, 0.12) 0%, transparent 60%),
                radial-gradient(ellipse 60% 80% at 90% 10%, rgba(30, 80, 160, 0.2) 0%, transparent 60%),
                radial-gradient(ellipse 50% 50% at 50% 50%, rgba(10, 30, 60, 0.8) 0%, transparent 80%);
        }

        .bg-grid {
            position: fixed;
            inset: 0;
            z-index: 0;
            background-image:
                linear-gradient(rgba(0, 229, 255, 0.04) 1px, transparent 1px),
                linear-gradient(90deg, rgba(0, 229, 255, 0.04) 1px, transparent 1px);
            background-size: 60px 60px;
            mask-image: radial-gradient(ellipse 70% 70% at 50% 50%, black 20%, transparent 70%);
            -webkit-mask-image: radial-gradient(ellipse 70% 70% at 50% 50%, black 20%, transparent 70%);
        }

        .orb {
            position: fixed;
            border-radius: 50%;
            filter: blur(80px);
            z-index: 0;
            animation: float 20s ease-in-out infinite;
        }

        .orb-1 {
            width: 500px;
            height: 500px;
            background: radial-gradient(circle, rgba(0, 229, 255, 0.15), transparent 70%);
            top: -150px;
            left: -100px;
            animation-duration: 18s;
        }

        .orb-2 {
            width: 400px;
            height: 400px;
            background: radial-gradient(circle, rgba(30, 80, 200, 0.18), transparent 70%);
            bottom: -100px;
            right: -80px;
            animation-duration: 22s;
            animation-delay: -5s;
        }

        .orb-3 {
            width: 300px;
            height: 300px;
            background: radial-gradient(circle, rgba(0, 180, 220, 0.1), transparent 70%);
            top: 40%;
            left: 60%;
            animation-duration: 25s;
            animation-delay: -10s;
        }

        @keyframes float {
            0%, 100% { transform: translate(0, 0) scale(1); }
            25% { transform: translate(30px, -40px) scale(1.05); }
            50% { transform: translate(-20px, 20px) scale(0.95); }
            75% { transform: translate(40px, 30px) scale(1.02); }
        }

        .particles {
            position: fixed;
            inset: 0;
            z-index: 0;
            overflow: hidden;
        }

        .particle {
            position: absolute;
            width: 3px;
            height: 3px;
            background: var(--accent);
            border-radius: 50%;
            opacity: 0;
            animation: rise linear infinite;
        }

        .particle:nth-child(1) { left: 10%; animation-duration: 12s; animation-delay: 0s; }
        .particle:nth-child(2) { left: 20%; animation-duration: 15s; animation-delay: -2s; width: 2px; height: 2px; }
        .particle:nth-child(3) { left: 35%; animation-duration: 10s; animation-delay: -4s; }
        .particle:nth-child(4) { left: 50%; animation-duration: 18s; animation-delay: -6s; width: 4px; height: 4px; }
        .particle:nth-child(5) { left: 65%; animation-duration: 14s; animation-delay: -3s; width: 2px; height: 2px; }
        .particle:nth-child(6) { left: 75%; animation-duration: 11s; animation-delay: -7s; }
        .particle:nth-child(7) { left: 85%; animation-duration: 16s; animation-delay: -1s; width: 2px; height: 2px; }
        .particle:nth-child(8) { left: 5%;  animation-duration: 13s; animation-delay: -9s; width: 4px; height: 4px; }
        .particle:nth-child(9) { left: 45%; animation-duration: 17s; animation-delay: -5s; }
        .particle:nth-child(10){ left: 92%; animation-duration: 14s; animation-delay: -8s; width: 2px; height: 2px; }
        .particle:nth-child(11){ left: 28%; animation-duration: 19s; animation-delay: -11s; }
        .particle:nth-child(12){ left: 58%; animation-duration: 12s; animation-delay: -3s; width: 3px; height: 3px; }

        @keyframes rise {
            0% {
                transform: translateY(100vh) translateX(0);
                opacity: 0;
            }
            10% {
                opacity: 0.6;
            }
            90% {
                opacity: 0.3;
            }
            100% {
                transform: translateY(-10vh) translateX(40px);
                opacity: 0;
            }
        }

        .scanline {
            position: fixed;
            left: 0;
            width: 100%;
            height: 1px;
            background: linear-gradient(90deg, transparent 0%, var(--accent) 50%, transparent 100%);
            opacity: 0.08;
            z-index: 0;
            animation: scan 8s linear infinite;
        }

        @keyframes scan {
            0% { top: -2%; }
            100% { top: 102%; }
        }

        .circuit-lines {
            position: fixed;
            inset: 0;
            z-index: 0;
            overflow: hidden;
            opacity: 0.06;
        }

        .circuit-lines::before,
        .circuit-lines::after {
            content: '';
            position: absolute;
            background: var(--accent);
        }

        .circuit-lines::before {
            width: 200px;
            height: 1px;
            top: 25%;
            left: 0;
            box-shadow:
                0 0 8px var(--accent),
                250px 80px 0 0 var(--accent),
                250px 80px 8px 0 var(--accent);
        }

        .circuit-lines::after {
            width: 1px;
            height: 150px;
            bottom: 10%;
            right: 15%;
            box-shadow:
                0 0 8px var(--accent),
                -60px -150px 0 0 var(--accent),
                -60px -150px 8px 0 var(--accent);
        }

        .corner-decor {
            position: fixed;
            width: 60px;
            height: 60px;
            z-index: 1;
            opacity: 0.15;
        }

        .corner-decor::before,
        .corner-decor::after {
            content: '';
            position: absolute;
            background: var(--accent);
        }

        .corner-tl { top: 20px; left: 20px; }
        .corner-tl::before { width: 30px; height: 1px; top: 0; left: 0; }
        .corner-tl::after  { width: 1px; height: 30px; top: 0; left: 0; }

        .corner-tr { top: 20px; right: 20px; }
        .corner-tr::before { width: 30px; height: 1px; top: 0; right: 0; }
        .corner-tr::after  { width: 1px; height: 30px; top: 0; right: 0; }

        .corner-bl { bottom: 20px; left: 20px; }
        .corner-bl::before { width: 30px; height: 1px; bottom: 0; left: 0; }
        .corner-bl::after  { width: 1px; height: 30px; bottom: 0; left: 0; }

        .corner-br { bottom: 20px; right: 20px; }
        .corner-br::before { width: 30px; height: 1px; bottom: 0; right: 0; }
        .corner-br::after  { width: 1px; height: 30px; bottom: 0; right: 0; }

        /* === MAIN CONTENT === */

        .glass-container {
            position: fixed;
            top: 10px;
            z-index: 2;
            background: var(--glass);
            backdrop-filter: blur(20px);
            -webkit-backdrop-filter: blur(20px);
            border: 1px solid var(--glass-border);
            border-radius: 24px;
            padding: 40px;
            box-shadow:
                0 8px 32px rgba(0, 0, 0, 0.3),
                inset 0 1px 0 rgba(255, 255, 255, 0.05);
            text-align: center;
            width: 90%;
            max-width: 500px;
        }

        .glass-container::before {
            content: '';
            position: absolute;
            inset: -1px;
            border-radius: 24px;
            padding: 1px;
            background: linear-gradient(
                135deg,
                rgba(0, 229, 255, 0.3) 0%,
                transparent 40%,
                transparent 60%,
                rgba(30, 80, 200, 0.2) 100%
            );
            -webkit-mask: linear-gradient(#fff 0 0) content-box, linear-gradient(#fff 0 0);
            mask: linear-gradient(#fff 0 0) content-box, linear-gradient(#fff 0 0);
            -webkit-mask-composite: xor;
            mask-composite: exclude;
            pointer-events: none;
        }

        .title {
            font-size: 28px;
            font-weight: 800;
            color: var(--accent);
            text-transform: uppercase;
            letter-spacing: 2px;
            text-shadow: 0 0 20px var(--accent-glow);
        }

        .title2 {
            font-size: 16px;
            font-weight: 600;
            color: var(--text-secondary);
            margin-bottom: 30px;
            text-transform: uppercase;
            letter-spacing: 3px;
        }

        .grid-botones {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 12px;
        }

        .btn {
            background: rgba(255, 255, 255, 0.04);
            border: 1px solid rgba(255, 255, 255, 0.08);
            border-radius: 16px;
            color: var(--text-primary);
            padding: 20px 10px;
            font-size: 14px;
            font-weight: 600;
            text-decoration: none;
            cursor: pointer;
            transition: all 0.3s ease;
            display: flex;
            flex-direction: column;
            align-items: center;
            gap: 8px;
            position: relative;
            overflow: hidden;
        }

        .btn::before {
            content: '';
            position: absolute;
            inset: 0;
            background: linear-gradient(135deg, rgba(0, 229, 255, 0.1), transparent);
            opacity: 0;
            transition: opacity 0.3s ease;
        }

        .btn:hover {
            border-color: var(--accent);
            transform: translateY(-3px);
            box-shadow: 0 8px 25px rgba(0, 229, 255, 0.15);
        }

        .btn:hover::before {
            opacity: 1;
        }

        .btn:active {
            transform: translateY(0);
        }

        .btn-off:hover {
            border-color: #ff4757;
            box-shadow: 0 8px 25px rgba(255, 71, 87, 0.2);
        }

        .btn-off:hover::before {
            background: linear-gradient(135deg, rgba(255, 71, 87, 0.1), transparent);
        }

        .section-label {
            grid-column: span 2;
            text-align: left;
            font-size: 11px;
            color: var(--text-secondary);
            margin-top: 10px;
            font-weight: 700;
            text-transform: uppercase;
            letter-spacing: 2px;
            padding-left: 4px;
            border-left: 2px solid var(--accent);
        }

        .status-bar {
            display: flex;
            justify-content: center;
            gap: 8px;
            margin-bottom: 25px;
            margin-top: 5px;
        }

        .status-dot {
            width: 6px;
            height: 6px;
            border-radius: 50%;
            background: var(--accent);
            opacity: 0.5;
            animation: pulse-dot 2s ease-in-out infinite;
        }

        .status-dot:nth-child(2) { animation-delay: 0.3s; }
        .status-dot:nth-child(3) { animation-delay: 0.6s; }

        @keyframes pulse-dot {
            0%, 100% { opacity: 0.3; }
            50% { opacity: 1; }
        }

        .top-link {
            position: fixed;
            top: 20px;
            right: 20px;
            z-index: 10;
        }

        .top-link img {
            width: 60px;
            height: 60px;
            border-radius: 50%;
            border: 2px solid rgba(0, 229, 255, 0.4);
            transition: 0.3s;
            box-shadow: 0 0 15px rgba(0, 229, 255, 0.15);
        }

        .top-link img:hover {
            border-color: var(--accent);
            box-shadow:
                0 0 25px rgba(0, 229, 255, 0.3),
                0 0 50px rgba(0, 229, 255, 0.1);
            transform: scale(1.08);
        }
        .top-link2 {
            position: fixed;
            top: 20px;
            left: 20px;
            z-index: 10;
        }

        .top-link2 img {
            width: 60px;
            height: 60px;
            border-radius: 50%;
            border: 2px solid rgba(0, 229, 255, 0.4);
            transition: 0.3s;
            box-shadow: 0 0 15px rgba(0, 229, 255, 0.15);
        }

        .top-link2 img:hover {
            border-color: var(--accent);
            box-shadow:
                0 0 25px rgba(0, 229, 255, 0.3),
                0 0 50px rgba(0, 229, 255, 0.1);
            transform: scale(1.08);
        }
        .top-link2r {
            position: fixed;
            top: 20px;
            left: 20px;
            z-index: 10;
        }

        .top-link2r img {
            width: 60px;
            height: 60px;
            border-radius: 50%;
            border: 2px solid rgba(0, 229, 255, 0.4);
            transition: 0.3s;
            box-shadow: 0 0 15px rgba(0, 229, 255, 0.15);
        }

        .top-link2r img:hover {
            border-color: var(--accent);
            box-shadow:
                0 0 25px rgba(255, 71, 87, 0.2),
                0 0 50px rgba(255, 71, 87, 0.1);
            transform: scale(1.08);
        }


        .credit {
            position: fixed;
            bottom: 20px;
            left: 25px;
            z-index: 10;
            color: var(--text-secondary);
            font-weight: 500;
            font-size: 12px;
            letter-spacing: 0.5px;
            line-height: 1.6;
        }

        @media (max-width: 520px) {
            .glass-container {
                padding: 25px 20px;
            }
            .title {
                font-size: 22px;
            }
            .credit {
                font-size: 10px;
            }
        }
</style>
<body>

    <!-- Background layers -->
    <div class='bg-mesh'></div>
    <div class='bg-grid'></div>

    <div class='orb orb-1'></div>
    <div class='orb orb-2'></div>
    <div class='orb orb-3'></div>

    <div class='particles'>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
        <div class='particle'></div>
    </div>

    <div class='scanline'></div>
    <div class='circuit-lines'></div>

    <!-- Corner decorations -->
    <div class='corner-decor corner-tl'></div>
    <div class='corner-decor corner-tr'></div>
    <div class='corner-decor corner-bl'></div>
    <div class='corner-decor corner-br'></div>

    <!-- Logo -->
    <a href='https://belagua.com.ve/' target='_blank' class='top-link'>
      <img src='https://cdn.jsdelivr.net/gh/Gouveira/SCP@main/images.png' alt='Belagua-icon'>
    </a>
    <!-- Credits -->
    <div class='credit'>
        Daniel Gouveia /
        Lenin Espinoza /
        Susej Villamizar
    </div>

    <!-- Panel Principal -->
    <div class='glass-container'>
        <div class='title'>Smart House</div>
        <div class='title2'>Panel de Control</div>
        <div class='status-bar'>
            <div class='status-dot'></div>
            <div class='status-dot'></div>
            <div class='status-dot'></div>
        </div>

        <div class='grid-botones'>
            <div class='section-label'>Luces de la casa</div>)raw";
          if (sl == false) {
          html += R"raw(<button class='btn' onclick='location.href="/sl"'>&#x1F7E2; Encender Habitacion 1</button>)raw";
          } else if (sl == true) {
          html += R"raw(<button class='btn btn-off' onclick='location.href="/sl"'>&#x1F534; Apagar Habitación 1</button>)raw";
          }
          if (ba == false) {
          html += R"raw(<button class='btn' onclick='location.href="/ba"'>&#x1F7E2; Encender Habitación 2</button>)raw";
          } else if (ba == true) {
          html += R"raw(<button class='btn btn-off' onclick='location.href="/ba"'>&#x1F534; Apagar Habitación 2</button>)raw";
          }
          if (cc == false) {
          html += R"raw(<button class='btn' onclick='location.href="/cc"'>&#x1F7E2; Encender Habitación 3</button>)raw";
          } else if (cc == true) {
          html += R"raw(<button class='btn btn-off' onclick='location.href="/cc"'>&#x1F534; Apaga Habitación 3</button>)raw";
          }
          if (hb == false) {
          html += R"raw(<button class='btn' onclick='location.href="/hb"'>&#x1F7E2; Encender Habitación 4</button>)raw";
          } else if (hb == true) {
          html += R"raw(<button class='btn btn-off' onclick='location.href="/hb"'>&#x1F534; Apaga Habitación 4</button>)raw";
          }
          
          html += R"raw(<div class='section-label'>Extras</div>)raw";

          if (ser == false) {
          html += R"raw(<button class='btn' onclick='location.href="/servo"'>&#x1F7E2; Abrir Portón</button>)raw";
          } else if (ser == true) {
          html += R"raw(<button class='btn btn-off' onclick='location.href="/servo"'>&#x1F534; Cerrar Portón</button>)raw";
          }
          if (bm == false) {
          html += R"raw(<button class='btn' onclick='location.href="/bm"'>&#x1F7E2; Encender Bombillo</button>)raw";
          } else if (bm == true) {
          html += R"raw(<button class='btn btn-off' onclick='location.href="/bm"'>&#x1F534; Apaga Bombillo</button>)raw";
          }
        html += R"raw(</div>
    </div>
</body>
</html>
)raw";
return html;
}
// ==== Función para manejar las rutas del servidor ====
void configurarServidor() {
  // Ruta principal
  server.on("/", []() {
    server.send(200, "text/html", generarPaginaHTML());
  });
  server.on("/bm", []() {
    if(bm == false) {
    digitalWrite(bmp, HIGH);
    bm = true;
    } else {
    digitalWrite(bmp, LOW);
    bm = false;
    }
    server.send(200, "text/html", generarPaginasin());
  });

  // sala LED
  server.on("/sl", []() {
    if(sl == false) {
    digitalWrite(slp, HIGH);
    sl = true;
    } else {
    digitalWrite(slp, LOW);
    sl = false;
    }
    server.send(200, "text/html", generarPaginasin());
  });

  // baño LED
  server.on("/ba", []() {
    if(ba == false) {
    digitalWrite(bap, HIGH);
    ba = true;
    } else {
    digitalWrite(bap, LOW);
    ba = false;
    }
    server.send(200, "text/html", generarPaginasin());
  });

  // cocina LED
  server.on("/cc", []() {
    if(cc == false) {
    digitalWrite(ccp, HIGH);
    cc = true;
    } else {
    digitalWrite(ccp, LOW);
    cc = false;
    }
    server.send(200, "text/html", generarPaginasin());
  });

  // habitación LED
  server.on("/hb", []() {
    if(hb == false) {
    digitalWrite(hbp, HIGH);
    hb = true;
    } else {
    digitalWrite(hbp, LOW);
    hb = false;
    }
    server.send(200, "text/html", generarPaginasin());
  });

  // Servo
   server.on("/servo", []() {
    if(ser == false) {
    miServo.write(80);
    ser = true;
    } else {
    ser = false;
    for (int pos = 80; pos >= 0; pos -= 5) { 
        miServo.write(pos);
        delay(20);                       
    }
}
    server.send(200, "text/html", generarPaginasin());
  });

  server.begin();
}

// ==== Setup ====
void setup() {
  Serial.begin(115200);
  pinMode(slp, OUTPUT);
  pinMode(bap, OUTPUT);
  pinMode(ccp, OUTPUT);
  pinMode(hbp, OUTPUT);
  pinMode(bmp, OUTPUT);
  miServo.attach(PINSERVO);
  digitalWrite(slp, LOW);
  digitalWrite(bap, LOW);
  digitalWrite(ccp, LOW);
  digitalWrite(hbp, LOW);
  digitalWrite(bmp, LOW);
  miServo.write(0);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;);
  }

  conectarWiFi();
  configurarServidor();
}

// ==== Loop ====
void loop() {
  server.handleClient();
}