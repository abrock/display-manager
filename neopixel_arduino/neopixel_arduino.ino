#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

int i=0;

#define PIN 2
#define NUM_PIXELS 13
#define NUM_IMG 1

// Maximum mean brightness to protect a slightly undersized power supply.
// All colors channels are allowed to have full brightness (255),
// but not all pixels and all color channels at the same time.
#define MAX_MEAN_BRIGHTNESS 3

uint32_t const max_total_brightness = uint32_t(MAX_MEAN_BRIGHTNESS) * NUM_PIXELS * 4;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRBW + NEO_KHZ800);

uint8_t images[NUM_IMG][NUM_PIXELS];
uint32_t image_sums[NUM_IMG];
uint16_t image_delays[NUM_IMG];

uint8_t color[4] = {255,0,0,0};

#define seconds() (millis()/1000)

unsigned long int last_serial_input;

unsigned long last_img_switched;

void setup()  {
  pixels.begin();
  pixels.clear();
  pixels.show();

  for (int ii = 0; ii < NUM_IMG; ++ii) {
    image_delays[ii] = 1000;
    for (int jj = 0; jj < NUM_PIXELS; ++jj) {
      images[ii][jj] = ((jj + NUM_IMG - ii) % (NUM_IMG+1))*(255/NUM_IMG);
    }
  }

  Serial.begin(115200);
  Serial.println("Neopixel display booted");

  last_serial_input = seconds();
  last_img_switched = millis();
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
    num_img = line.substring(String("num-images: ").length()).toInt();
    Serial.print("Set num_img to ");
    Serial.println(num_img);
  }
  if (line.startsWith("set-color-channel-")) {
    if (line.length() < String("set-color-channel-").length()+4) {
      Serial.println("Invalid attempt at setting color:");
      Serial.println(line);
      line = "";
      return;
    }
    uint8_t brightness = line.substring(String("set-color-channel-x: ").length()).toInt();
    switch (line[String("set-color-channel-").length()]) {
      case 'r': color[0] = brightness; break;
      case 'g': color[1] = brightness; break;
      case 'b': color[2] = brightness; break;
      case 'w': color[3] = brightness; break;
    }
  }
  line = "";
}

uint32_t total_brightness = 0;

// Automatic global brightness scale value to make the display stay below the man_mean_brightness.
uint8_t global_brightness_scale = 255;

uint8_t scale_uint8_t(uint8_t a, uint8_t b) {
  return uint8_t((uint32_t(a) * uint32_t(b) * uint32_t(global_brightness_scale))/255/255);
}

void set_px_accumulate_total_brightness(
  uint16_t px_idx,
  uint8_t r,
  uint8_t g,
  uint8_t b,
  uint8_t w
) {
  pixels.setPixelColor(px_idx, r, g, b, w);
  total_brightness += r;
  total_brightness += g;
  total_brightness += b;
  total_brightness += w;
}

void set_px_color_scaled(uint16_t px_idx, uint8_t scale) {
  set_px_accumulate_total_brightness(
    px_idx,
    scale_uint8_t(scale, color[0]),
    scale_uint8_t(scale, color[1]),
    scale_uint8_t(scale, color[2]),
    scale_uint8_t(scale, color[3])
  );
}

void show_img_sub() {
  total_brightness = 0;
  pixels.clear();
  for (uint16_t ii = 0; ii < NUM_PIXELS; ++ii) {
    set_px_color_scaled(ii, images[display_img_idx][ii]);
  }
  Serial.print("Max total brightness:     ");
  Serial.println(max_total_brightness);
  Serial.print("Current total brightness: ");
  Serial.println(total_brightness);
}

void show_img() {
  Serial.print("max-num-img: ");
  Serial.println(NUM_IMG);
  Serial.print("num-pixels: ");
  Serial.println(NUM_PIXELS);
  if (num_img <= 0 || reading_img) {
    return;
  }
  display_img_idx = (display_img_idx + 1) % num_img;
  Serial.print("\n\nStarting show_img for image #");
  Serial.println(display_img_idx);
  global_brightness_scale = 255;
  show_img_sub();
  if (total_brightness > max_total_brightness) {
    global_brightness_scale = uint8_t((uint64_t(max_total_brightness)*uint64_t(255))/uint64_t(total_brightness));
    Serial.print("Maximum total brightness too high, scaling by ");
    Serial.println(global_brightness_scale);
    show_img_sub();
  }
  pixels.show();
  Serial.print("Pixels for image #");
  Serial.print(display_img_idx);
  Serial.println(" were set and shown");
}

void loop()  {

  while (Serial.available()) {
    last_serial_input = seconds();
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

  unsigned long time = millis();
  if (time - last_img_switched > image_delays[display_img_idx]) {
    last_img_switched = time;
    show_img();
  }

  if (seconds() - last_serial_input > 2) {
    if (line.length() > 0) {
      Serial.println("Got incomplete line, timed out:");
      Serial.println(line);
      line = "";
    }
  }
}
