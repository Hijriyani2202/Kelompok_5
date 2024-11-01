#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

const char* WIFI_SSID = "panca";
const char* WIFI_PASSWORD = "12345678910";

const int relay1 = 25;  // Pin yang digunakan untuk mengendalikan lampu
const int pirSensor = 26;  // Pin yang digunakan untuk sensor PIR
bool isAutoMode = true;  // Mengatur mode default ke otomatis

WebServer server(80);

void setup() {
  Serial.begin(115200);

  pinMode(relay1, OUTPUT);
  pinMode(pirSensor, INPUT);
  digitalWrite(relay1, LOW);  // Lampu off secara default

  // Menghubungkan ke WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Mengatur server web
  server.on("/", handleRoot);             // Halaman utama untuk kontrol lampu
  server.on("/manual_on", handleManualOn);  // Endpoint untuk menyalakan lampu secara manual
  server.on("/manual_off", handleManualOff);  // Endpoint untuk mematikan lampu secara manual
  server.on("/set_auto", handleSetAuto);    // Endpoint untuk mengatur mode otomatis
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();  // Menangani permintaan dari web server

  // Mode otomatis menggunakan sensor PIR
  if (isAutoMode) {
    int pirStatus = digitalRead(pirSensor);  // Membaca status sensor PIR
    if (pirStatus == HIGH) {
      digitalWrite(relay1, HIGH);  // Nyalakan lampu saat ada gerakan
      Serial.println("Lampu menyala karena deteksi gerakan");
    } else {
      digitalWrite(relay1, LOW);  // Matikan lampu saat tidak ada gerakan
      Serial.println("Lampu mati karena tidak ada gerakan");
    }
  }

  delay(1000);
}

// Fungsi untuk halaman utama
void handleRoot() {
  String html = "<html><body><h1>Kontrol Lampu</h1>";
  html += "<p><a href=\"/manual_on\">Nyalakan Lampu (Manual)</a></p>";
  html += "<p><a href=\"/manual_off\">Matikan Lampu (Manual)</a></p>";
  html += "<p><a href=\"/set_auto\">Mode Otomatis</a></p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

// Fungsi untuk mengaktifkan lampu secara manual
void handleManualOn() {
  isAutoMode = false;  // Matikan mode otomatis
  digitalWrite(relay1, HIGH);  // Nyalakan lampu
  server.send(200, "text/html", "<html><body><h1>Lampu Dinyalakan Secara Manual</h1><a href=\"/\">Kembali</a></body></html>");
  Serial.println("Lampu dinyalakan secara manual");
}

// Fungsi untuk mematikan lampu secara manual
void handleManualOff() {
  isAutoMode = false;  // Matikan mode otomatis
  digitalWrite(relay1, LOW);  // Matikan lampu
  server.send(200, "text/html", "<html><body><h1>Lampu Dimatikan Secara Manual</h1><a href=\"/\">Kembali</a></body></html>");
  Serial.println("Lampu dimatikan secara manual");
}

// Fungsi untuk mengaktifkan mode otomatis
void handleSetAuto() {
  isAutoMode = true;  // Aktifkan mode otomatis
  server.send(200, "text/html", "<html><body><h1>Mode Otomatis Diaktifkan</h1><a href=\"/\">Kembali</a></body></html>");
  Serial.println("Mode otomatis diaktifkan");
}

