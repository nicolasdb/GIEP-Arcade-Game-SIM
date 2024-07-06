#include <Adafruit_NeoPixel.h>

const int PIN = 6;
const int NUM_PIXELS = 40;
const uint8_t BRIGHTNESS = 50;

Adafruit_NeoPixel pixels(NUM_PIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Zone definitions
const int RAIN_ZONE_START = 0, RAIN_ZONE_END = 23;
const int BUTTON_PIXELS[] = {24, 25, 32, 33};
const int NUM_BUTTONS = sizeof(BUTTON_PIXELS) / sizeof(BUTTON_PIXELS[0]);
const int SEWER_ZONE_START = 31, SEWER_ZONE_END = 26;
const int BASIN_ZONE_START = 39, BASIN_ZONE_END = 34;

// Button pins (adjust as needed)
const int BUTTON_PINS[] = {8, 9, 10, 11};

// Game parameters for fine-tuning
const int RAIN_ANIMATION_INTERVAL = 50;   // milliseconds between rain animation updates
const int GAME_UPDATE_INTERVAL = 100;     // milliseconds between game state updates

const float MAX_RAIN_INTENSITY = 1.0;     // maximum rate of water increase per second
const float MIN_RAIN_INTENSITY = 0.5;     // minimum rate of water increase per second
const int RAIN_DURATION = 10000;          // duration of rain in milliseconds
const int DRY_DURATION = 5000;            // duration of dry weather in milliseconds

const float BUTTON_EFFECTS[] = {0.15, 0.15, 0.15, 0.15};  // water reduction rate per second for each button
const float SEWER_DRAIN_RATE = 0.05;      // natural drain rate of sewer per second

// Rain visualization parameters
const int NUM_RAINDROPS = 2;
const byte FADE_AMOUNT = 50;

// Game variables
float sewerLevel = 0;
float basinLevel = 0;
unsigned long lastRainAnimationUpdate = 0;
unsigned long lastGameUpdate = 0;
unsigned long weatherChangeTime = 0;
bool isRaining = true;
float currentRainIntensity = MAX_RAIN_INTENSITY;

// Rain visualization variables
int rainDropPositions[NUM_RAINDROPS];
byte pixelBrightness[NUM_PIXELS];

void setup() {
  pixels.begin();
  pixels.setBrightness(BRIGHTNESS);
  pixels.show();

  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
  }

  Serial.begin(9600);
  randomSeed(analogRead(0));

  // Initialize rain drops
  for (int i = 0; i < NUM_RAINDROPS; i++) {
    rainDropPositions[i] = random(RAIN_ZONE_START, RAIN_ZONE_END + 1);
  }

  // Initialize pixel brightness
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixelBrightness[i] = 0;
  }
}

void updateRainAnimation() {
  if (isRaining) {
    // Update raindrop positions
    for (int i = 0; i < NUM_RAINDROPS; i++) {
      rainDropPositions[i]++;
      if (rainDropPositions[i] > RAIN_ZONE_END) {
        rainDropPositions[i] = RAIN_ZONE_START;
      }
      pixelBrightness[rainDropPositions[i]] = 255;
    }
  }

  // Apply fade effect (this happens even when not raining to fade out existing drops)
  for (int i = RAIN_ZONE_START; i <= RAIN_ZONE_END; i++) {
    pixelBrightness[i] = max(0, pixelBrightness[i] - FADE_AMOUNT);
  }

  // Set rain pixels
  for (int i = RAIN_ZONE_START; i <= RAIN_ZONE_END; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, pixelBrightness[i]));
  }
}

void updateWeather(unsigned long currentTime) {
  if (currentTime - weatherChangeTime >= (isRaining ? RAIN_DURATION : DRY_DURATION)) {
    isRaining = !isRaining;
    weatherChangeTime = currentTime;
    if (isRaining) {
      currentRainIntensity = random(MIN_RAIN_INTENSITY * 100, MAX_RAIN_INTENSITY * 100) / 100.0;
      // Initialize new raindrops when it starts raining
      for (int i = 0; i < NUM_RAINDROPS; i++) {
        rainDropPositions[i] = random(RAIN_ZONE_START, RAIN_ZONE_END + 1);
      }
    } else {
      // Clear all raindrops when it stops raining
      for (int i = RAIN_ZONE_START; i <= RAIN_ZONE_END; i++) {
        pixelBrightness[i] = 0;
      }
    }
  }
}

void updateWaterLevels(float deltaTime) {
  float waterIncrease = 0;
  
  if (isRaining) {
    waterIncrease += currentRainIntensity * deltaTime;
  }
  
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (digitalRead(BUTTON_PINS[i]) == LOW) {
      waterIncrease -= BUTTON_EFFECTS[i] * deltaTime;
    }
  }
  
  waterIncrease -= SEWER_DRAIN_RATE * deltaTime;
  
  sewerLevel = max(0.0f, min(sewerLevel + waterIncrease, float(SEWER_ZONE_START - SEWER_ZONE_END + 1)));
  
  if (sewerLevel == float(SEWER_ZONE_START - SEWER_ZONE_END + 1)) {
    basinLevel = min(basinLevel + waterIncrease, float(BASIN_ZONE_START - BASIN_ZONE_END + 1));
  }
}

void updateSewerDisplay() {
  int filledPixels = int(sewerLevel);
  for (int i = SEWER_ZONE_START; i >= SEWER_ZONE_END; i--) {
    if (SEWER_ZONE_START - i < filledPixels) {
      pixels.setPixelColor(i, pixels.Color(255, 165, 0));
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
  }
}

void updateBasinDisplay() {
  int filledPixels = int(basinLevel);
  for (int i = BASIN_ZONE_START; i >= BASIN_ZONE_END; i--) {
    if (BASIN_ZONE_START - i < filledPixels) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 255));
    } else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
  }
}

void updateButtonDisplay() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (digitalRead(BUTTON_PINS[i]) == LOW) {
      pixels.setPixelColor(BUTTON_PIXELS[i], pixels.Color(0, 255, 0));
    } else {
      pixels.setPixelColor(BUTTON_PIXELS[i], pixels.Color(50, 50, 50));
    }
  }
}

bool isGameOver() {
  return basinLevel >= float(BASIN_ZONE_START - BASIN_ZONE_END + 1);
}

void gameOverAnimation() {
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(255, 0, 0));
  }
  pixels.show();
  delay(500);
  for (int i = 0; i < NUM_PIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();
  delay(500);
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastRainAnimationUpdate >= RAIN_ANIMATION_INTERVAL) {
    updateRainAnimation();
    lastRainAnimationUpdate = currentTime;
  }

  if (currentTime - lastGameUpdate >= GAME_UPDATE_INTERVAL) {
    float deltaTime = (currentTime - lastGameUpdate) / 1000.0;
    
    updateWeather(currentTime);
    updateWaterLevels(deltaTime);
    updateSewerDisplay();
    updateBasinDisplay();
    updateButtonDisplay();
    
    lastGameUpdate = currentTime;
  }

  pixels.show();

  if (isGameOver()) {
    gameOverAnimation();
  }
}