#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

int i=0;

#define PIN 2
#define NUM_PIXELS 13
#define NUM_IMG 4

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRBW + NEO_KHZ800);

uint8_t images[NUM_IMG][NUM_PIXELS];
uint32_t image_sums[NUM_IMG];
uint16_t image_delays[NUM_IMG];

uint32_t color = pixels.Color(3,0,0,0);

void setup()  {
  pixels.begin();
  pixels.clear();
  pixels.show();

  for (int ii = 0; ii < NUM_IMG; ++ii) {
    image_delays[ii] = 1000;
    for (int jj = 0; jj < NUM_PIXELS; ++jj) {
      images[ii][jj] = (jj + NUM_IMG - ii) % NUM_IMG;
    }
  }

  Serial.begin(115200);
  Serial.println("Neopixel display booted");
}

String line;

bool reading_img = false;
uint8_t img_idx = 0;
uint16_t pixel_idx = 0;

uint8_t num_img = NUM_IMG;
uint8_t display_img_idx = 0;

void handle_line() {
  if (0 == line.length()) {
    return;
  }
  Serial.println("Handling command: ");
  Serial.println(line);
  if (line.startsWith("num-images: ")) {
    num_img = line.substring(String("num_img: ").length()).toInt();
    Serial.print("Set num_img to ");
    Serial.println(num_img);
  }
  line = "";
}

void show_img() {
  pixels.clear();
  if (num_img <= 0 || reading_img) {
    return;
  }
  display_img_idx = (display_img_idx + 1) % num_img;
  uint8_t const r = 0;
  for (int ii = 0; ii < NUM_PIXELS; ++ii) {
    pixels.setPixelColor(ii, images[display_img_idx][ii]);
  }
  pixels.show();
  Serial.print("Pixels for image #");
  Serial.print(display_img_idx);
  Serial.println(" were set and shown");
  delay(500);
}

void loop()  {

  while (Serial.available()) {
    int const current_byte = Serial.read();
    if (current_byte < 0) {
      Serial.println("Invalid byte, reason unclear");
      line = "";
      return;
    }
    if (current_byte == '\n' || current_byte == '\r') {
      handle_line();
      return;
    }
    if (reading_img) {
      images[img_idx][pixel_idx] = current_byte;
      image_sums[img_idx] += current_byte;
      pixel_idx++;
      if (NUM_PIXELS == pixel_idx) {
        Serial.println("Finished receiving image");
        pixel_idx = 0;
        reading_img = false;
      }
      return;
    }
    line += char(current_byte);
    if (line.startsWith("Set image #") && line.length() == String("Set image #x").length()) {
      img_idx = line.substring(line.length()-1, line.length()).toInt();
      pixel_idx = 0;
      if (img_idx >= NUM_IMG) {
        Serial.print("Got invalid image index: ");
        Serial.println(img_idx);
        line = "";
        return;
      }
      num_img = img_idx+1;
      line = "";
      reading_img = true;
      image_sums[img_idx] = 0;
    }
    if (line.length() > 100) {
      Serial.println("Something went wrong, line longer than 100 characters, aborting");
      line = "";
      return;
    }
    Serial.println("Current line:");
    Serial.println(line);
  }

  if (num_img <= 0) {
    return;
  }

  show_img();
}
