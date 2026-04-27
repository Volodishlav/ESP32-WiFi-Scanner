#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

const unsigned long PAGE_MS = 2500UL; // ms por página del LCD
const unsigned long SCAN_INTERVAL_MS = 15000UL;

struct NetInfo {
  String ssid;
  int32_t rssi;
  String auth;
};

std::vector<NetInfo> redes;
unsigned long lastPageMillis = 0;
unsigned long lastScanMillis = 0;
size_t currentIndex = 0;

void setup() {
  Serial.begin(115200);
  delay(100);

  Wire.begin();
  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Iniciando...");
  lcd.setCursor(0,1);
  lcd.print("WiFi");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  scanNetworks();
  lastScanMillis = millis();
  lastPageMillis = millis();
}

void loop() {
  unsigned long now = millis();

  if (now - lastScanMillis >= SCAN_INTERVAL_MS) {
    scanNetworks();
    lastScanMillis = now;
    currentIndex = 0;
  }

  if (redes.empty()) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("No hay redes");
    lcd.setCursor(0,1);
    lcd.print("Ya escaneado...");
  } else if (now - lastPageMillis >= PAGE_MS) {
    showPage(currentIndex);
    lastPageMillis = now;
    currentIndex++;
    if (currentIndex >= redes.size()) currentIndex = 0;
  }
}

void scanNetworks() {
  Serial.println();
  Serial.println("Preparando...");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Escaneando...");
  lcd.setCursor(0,1);

  int n = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);
  Serial.printf("Encontrado: %d\n", n);

  redes.clear();
  redes.reserve(max(0, n));

  for (int i = 0; i < n; ++i) {
    NetInfo ni;
    ni.ssid = WiFi.SSID(i);
    ni.rssi = WiFi.RSSI(i);
    ni.auth = authModeToString(WiFi.encryptionType(i));
    redes.push_back(ni);

    Serial.printf("%2d: SSID: %s  RSSI: %d dBm  Enc: %s\n",
                  i+1, ni.ssid.c_str(), ni.rssi, ni.auth.c_str());
  }

  if (n == 0) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("No se encontraron");
    lcd.setCursor(0,1);
    lcd.print("redes :(");
  } else {
    currentIndex = 0;
    showPage(currentIndex);
  }

  WiFi.scanDelete();
}

void showPage(size_t idx) {
  if (redes.empty()) return;
  idx = idx % redes.size();
  NetInfo &n = redes[idx];

  lcd.clear();
  String s1 = n.ssid;
  if (s1.length() > LCD_COLS) s1 = s1.substring(0, LCD_COLS);
  lcd.setCursor(0,0);
  lcd.print(s1);

  String s2 = String(n.rssi) + "dB ";
  String t = n.auth;
  if (t.length() > (LCD_COLS - s2.length())) {
    t = t.substring(0, LCD_COLS - s2.length());
  }
  s2 += t;
  lcd.setCursor(0,1);
  lcd.print(s2);

  Serial.printf("Mostrando %u/%u : %s | %s | %ddBm\n", (unsigned)(idx+1), (unsigned)redes.size(),
                n.ssid.c_str(), n.auth.c_str(), n.rssi);
}

String authModeToString(wifi_auth_mode_t m) {
  switch (m) {
    case WIFI_AUTH_OPEN: return "Abierta";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA-PSK";
    case WIFI_AUTH_WPA2_PSK: return "WPA2-PSK";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2-ENT";
    case WIFI_AUTH_WPA3_PSK: return "WPA3-PSK";
    case WIFI_AUTH_WPA2_WPA3_PSK: return "WPA2/3";
    case WIFI_AUTH_WAPI_PSK: return "WAPI";
    default: return "Desconocida";
  }
}
