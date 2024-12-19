#define FASTLED_RMT_BUILTIN_DRIVER 1
#include <FastLED.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#define NUM_LEDS 64
#define DATA_PIN 14
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8

// LED Matrix Setup
CRGB leds[NUM_LEDS];

// WiFi Credentials
const char* ssid = "NETGEARE62A7F";
const char* password = "unevenlotus477";

// Web Server
AsyncWebServer server(80);

// Current demo mode
int currentMode = 3;

// Function prototypes
void setupWiFi();
void setupServer();
void displayRainbow();
void displayBlink();
void displayWipe();
void displayOff();
void displayScrollingText(const char* text);
void displayMovingDot();
void displayBreathingEffect();
void displayFireEffect();
void displayRandomSparkles();

void setup() {
  Serial.begin(115200);
  // Initialize LED Matrix
  FastLED.addLeds<WS2812B, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(40);
  FastLED.clear();
  FastLED.show();

  // Connect to WiFi
  setupWiFi();

  // Setup Web Server
  setupServer();

  Serial.println("Setup complete.");
}

void loop() {
  switch (currentMode) {
    case 0: displayRainbow(); break;
    case 1: displayBlink(); break;
    case 2: displayWipe(); break;
    case 3: displayOff(); break;
    case 4: displayScrollingText("ZLI BLJ"); break;
    case 5: displayMovingDot(); break;
    case 6: displayBreathingEffect(); break;
    case 7: displayFireEffect(); break;
    case 8: displayRandomSparkles(); break;
  }
}

void setupWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setupServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<!DOCTYPE html><html lang='en'><head><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>LED Matrix Demo</title>";
    html += "<style>body { font-family: Arial, sans-serif; text-align: center; background-color: #1A648C; color: white; }";
    html += "h1 { margin-top: 20px; } button { padding: 10px 20px; margin: 10px; font-size: 16px; border: none; border-radius: 5px; cursor: pointer; }";
    html += "button:hover { opacity: 0.9; } .rainbow { background-color: #ff4757; color: white; } .blink { background-color: #1e90ff; color: white; } .wipe { background-color: #2ed573; color: white; } .off { background-color: #747d8c; color: white; } .scroll { background-color: #ffa502; color: white; } .dot { background-color: #7bed9f; color: white; } .breathing { background-color: #70a1ff; color: white; } .fire { background-color: #ff6348; color: white; } .sparkles { background-color: #fffa65; color: black; }</style></head><body><h1>LED Matrix Demo</h1>";
    html += "<button class='rainbow' onclick=\"fetch('/mode?value=0').then(() => console.log('Success'))\">Rainbow</button>";
    html += "<button class='blink' onclick=\"fetch('/mode?value=1').then(() => console.log('Success'))\">Blink</button>";
    html += "<button class='wipe' onclick=\"fetch('/mode?value=2').then(() => console.log('Success'))\">Wipe</button>";
    html += "<button class='off' onclick=\"fetch('/mode?value=3').then(() => console.log('Success'))\">Off</button>";
    html += "<button class='scroll' onclick=\"fetch('/mode?value=4').then(() => console.log('Success'))\">Scroll Text</button>";
    html += "<button class='dot' onclick=\"fetch('/mode?value=5').then(() => console.log('Success'))\">Moving Dot</button>";
    html += "<button class='breathing' onclick=\"fetch('/mode?value=6').then(() => console.log('Success'))\">Breathing</button>";
    html += "<button class='fire' onclick=\"fetch('/mode?value=7').then(() => console.log('Success'))\">Fire</button>";
    html += "<button class='sparkles' onclick=\"fetch('/mode?value=8').then(() => console.log('Success'))\">Sparkles</button>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/mode", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("value")) {
      currentMode = request->getParam("value")->value().toInt();
    }
    request->send(200, "text/plain", "OK");
  });

  server.begin();
}

void displayRainbow() {
  static uint8_t hue = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue + (i * 10), 255, 255);
  }
  hue++;
  FastLED.show();
  delay(50);
}

void displayBlink() {
  static bool on = false;
  fill_solid(leds, NUM_LEDS, on ? CRGB::White : CRGB::Black);
  on = !on;
  FastLED.show();
  delay(500);
}

void displayWipe() {
  static int index = 0;
  leds[index] = CRGB::Blue;
  FastLED.show();
  delay(50);
  leds[index] = CRGB::Black;
  index = (index + 1) % NUM_LEDS;
}

void displayOff() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void displayScrollingText(const char* text) {
  static int offset = 0;
  FastLED.clear();
  for (int i = 0; i < strlen(text); i++) {
    int charIndex = i - offset / 8;
    if (charIndex >= 0 && charIndex < MATRIX_WIDTH) {
      char c = text[i];
      CRGB color = CHSV((i * 40) % 255, 255, 255); // Different color for each character
      for (int row = 0; row < 8; row++) {
        if (c & (1 << row)) {
          leds[row * MATRIX_WIDTH + charIndex] = color;
        }
      }
    }
  }
  FastLED.show();
  delay(100);
  offset++;
  if (offset >= strlen(text) * 8) {
    offset = 0;
  }
}

void displayMovingDot() {
  static int position = 0;
  FastLED.clear();
  leds[position] = CRGB::Red;
  FastLED.show();
  delay(100);
  position = (position + 1) % NUM_LEDS;
}

void displayBreathingEffect() {
  static uint8_t brightness = 0;
  static int8_t direction = 1;
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
  FastLED.setBrightness(brightness);
  FastLED.show();
  brightness += direction;
  if (brightness == 0 || brightness == 40) {
    direction = -direction;
  }
  delay(10);
}

void displayFireEffect() {
  static byte heat[MATRIX_WIDTH][MATRIX_HEIGHT];

  // Cool down every cell a little
  for (int x = 0; x < MATRIX_WIDTH; x++) {
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
      heat[x][y] = qsub8(heat[x][y], random8(0, ((20 * 10) / MATRIX_HEIGHT) + 2));
    }
  }

  // Heat from each cell drifts up and diffuses a little
  for (int x = 0; x < MATRIX_WIDTH; x++) {
    for (int y = MATRIX_HEIGHT - 1; y >= 2; y--) {
      heat[x][y] = (heat[x][y - 1] + heat[x][y - 2] + heat[x][y - 2]) / 3;
    }
  }

  // Add random sparks near the bottom
  for (int x = 0; x < MATRIX_WIDTH; x++) {
    if (random8() < 50) {
      int y = random8(2);
      heat[x][y] = qadd8(heat[x][y], random8(160, 255));
    }
  }

  // Map from heat array to LED colors
  for (int x = 0; x < MATRIX_WIDTH; x++) {
    for (int y = 0; y < MATRIX_HEIGHT; y++) {
      byte colorIndex = scale8(heat[x][y], 240);
      leds[(y * MATRIX_WIDTH) + x] = ColorFromPalette(HeatColors_p, colorIndex);
    }
  }

  FastLED.show();
  delay(50);
}

void displayRandomSparkles() {
  FastLED.clear();
  for (int i = 0; i < 10; i++) {
    leds[random(NUM_LEDS)] = CRGB::White;
  }
  FastLED.show();
  delay(100);
}
