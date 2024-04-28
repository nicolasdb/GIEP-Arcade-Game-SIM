
#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NUM_PIXELS 48
#define RAIN_START 0
#define RAIN_END 15
#define SEWER_START 16
#define SEWER_END 31
#define SEWER_LEVEL_MAX 200
#define CANAL_START 32
#define CANAL_END 47
#define CANAL_LEVEL_MAX 100



Adafruit_NeoPixel pixels(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

int* rainDropPositions;
byte* pixelBrightness;
float sewerLevelCounter = 0;
float canalWaterCounter = 0;
bool overflowBlink = false;
bool canalBlink = false;

int rainSpeed = 100;
int numDrops = 4;
byte fadeAmount = 80;

enum State {
  NO_RAIN,
  NORMAL_RAIN,
  HEAVY_RAIN,
  STORM,
  OVERFLOW_BLINK,
  CANAL_BLINK
};

State state = NORMAL_RAIN;

void setup() {
  pixels.begin();
  randomSeed(analogRead(A0));
  Serial.begin(9600);
  delay(1000);

  rainDropPositions = new int[numDrops];
  pixelBrightness = new byte[NUM_PIXELS];

  for (int d = 0; d < numDrops; d++) {
    rainDropPositions[d] = random(RAIN_START, RAIN_END + 1);
  }

  for (int i = 0; i < NUM_PIXELS; i++) {
    pixelBrightness[i] = 0;
  }
}




void updateSewerLevelCounter() {
  if (state != OVERFLOW_BLINK && state != CANAL_BLINK) {
    sewerLevelCounter += 0.4; // Ajuster cette valeur en fonction du type de pluie
    // Serial.println(sewerLevelCounter);

    if (sewerLevelCounter >= SEWER_LEVEL_MAX) {
      sewerLevelCounter = SEWER_LEVEL_MAX;
      pixels.setPixelColor(NUM_PIXELS, pixels.Color(255, 0, 0));
      pixels.show();
      state = OVERFLOW_BLINK;
    }
  }
}


void updateCanalWaterCounter() {
  if (state != OVERFLOW_BLINK && state != CANAL_BLINK) {
    canalWaterCounter += 0.4; // Ajuster cette valeur en fonction du type de pluie et du canal
    Serial.println(canalWaterCounter);

    if (canalWaterCounter >= CANAL_LEVEL_MAX) {
      canalWaterCounter = CANAL_LEVEL_MAX;
      state = CANAL_BLINK;
    }
  }
}


void updateNormalRain() {
  // Update rain drop positions
  for (int d = 0; d < numDrops; d++) {
    rainDropPositions[d]++;
    if (rainDropPositions[d] > RAIN_END) {
      rainDropPositions[d] = RAIN_START;
      // Serial.println("Drop reset to start");
    }
  }
}

void updateHeavyRain() {
  // Update rain drop positions
  for (int d = 0; d < numDrops; d++) {
    rainDropPositions[d]++;
    if (rainDropPositions[d] > RAIN_END) {
      rainDropPositions[d] = RAIN_START;
      // Serial.println("Drop reset to start");
    }
  }
}

void updateStorm() {
  // Update rain drop positions
  for (int d = 0; d < numDrops; d++) {
    rainDropPositions[d]++;
    if (rainDropPositions[d] > RAIN_END) {
      rainDropPositions[d] = RAIN_START;
      // Serial.println("Drop reset to start");
    }
  }
}


void drawScene() {
  // Clear all pixels
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }

  // Draw rain drops with varying brightness
  for (int d = 0; d < numDrops; d++) {
    pixelBrightness[rainDropPositions[d]] = 255;
  }

  // Draw water level
  int mappedSewerLevel = map(sewerLevelCounter, 0, SEWER_LEVEL_MAX, SEWER_START, SEWER_END + 1);
  for (int i = SEWER_START; i < mappedSewerLevel; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 255, 0)); // Yellow color 
  }

// Draw canal level
  int mappedCanalLevel = map(canalWaterCounter, 0, CANAL_LEVEL_MAX, CANAL_START, CANAL_END + 1);
  for (int i = CANAL_START; i < mappedCanalLevel; i++) {
  pixels.setPixelColor(i, pixels.Color(0, 0, 240)); // Blue color 
}


  // Apply pixelBrightness to pixels
  for (int i = RAIN_START; i <= RAIN_END; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, pixelBrightness[i]));
  }

  pixels.show();
}



void fadeTrail() {
  for (int i = RAIN_START; i <= RAIN_END; i++) {
    pixelBrightness[i] = max(0, pixelBrightness[i] - fadeAmount);
  }
}



void blinkOverflow() {
  static unsigned long lastBlinkTime = 0;
  static const unsigned long BLINK_INTERVAL = 500; // Blink interval in milliseconds

  if (millis() - lastBlinkTime >= BLINK_INTERVAL) {
    lastBlinkTime = millis();
    overflowBlink = !overflowBlink;
  }

  for (int i = SEWER_START; i <= SEWER_END; i++) {
    uint32_t color = overflowBlink ? pixels.Color(255, 0, 0) : pixels.Color(255, 255, 0);
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}


void blinkCanal() {
  static unsigned long lastBlinkTime = 0;
  static const unsigned long BLINK_INTERVAL = 500; // Blink interval in milliseconds

  if (millis() - lastBlinkTime >= BLINK_INTERVAL) {
    lastBlinkTime = millis();
    canalBlink = !canalBlink; // Utiliser canalBlink au lieu de overflowBlink
  }

  for (int i = CANAL_START; i <= CANAL_END; i++) {
    uint32_t color = canalBlink ? pixels.Color(255, 0, 0) : pixels.Color(0, 0, 240); // Utiliser canalBlink au lieu de overflowBlink
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}



void loop() {
 switch (state) {
    case NO_RAIN:
      // ... (gestion de l'état sans pluie)
      break;
    case NORMAL_RAIN:
      updateNormalRain();
      updateSewerLevelCounter();
      updateCanalWaterCounter();
      drawScene();
      fadeTrail();
      break;
    case HEAVY_RAIN:
      updateHeavyRain();
      updateSewerLevelCounter();
      updateCanalWaterCounter();
      drawScene();
      fadeTrail();
      break;
    case STORM:
      updateStorm();
      updateSewerLevelCounter();
      updateCanalWaterCounter();
      drawScene();
      fadeTrail();
      break;
    case OVERFLOW_BLINK:
      blinkOverflow();
      break;
    case CANAL_BLINK:
      blinkCanal();
      break;
 }
   delay(rainSpeed);
}


