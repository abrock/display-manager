#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

int i=0;

#define PIN 2
#define NUM_PIXELS 578
#define NUM_IMG 1

// Maximum mean brightness to protect a slightly undersized power supply.
// All colors channels are allowed to have full brightness (255),
// but not all pixels and all color channels at the same time.
#define MAX_MEAN_BRIGHTNESS 200

uint32_t const max_total_brightness = uint32_t(MAX_MEAN_BRIGHTNESS) * NUM_PIXELS * 4;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN, NEO_GRBW + NEO_KHZ800);

uint8_t images[NUM_IMG][NUM_PIXELS][4];
uint32_t image_sums[NUM_IMG];
uint16_t image_delays[NUM_IMG];

#define seconds() (millis()/1000)

unsigned long int last_serial_input;

unsigned long last_img_switched;

void setup()  {
  pixels.begin();
  pixels.clear();
  pixels.show();

  Serial.begin(115200);
  Serial.println("Neopixel color display booted");

  last_serial_input = seconds();
  last_img_switched = millis();

  Serial.print("max-num-img: ");
  Serial.println(NUM_IMG);
  Serial.print("num-pixels: ");
  Serial.println(NUM_PIXELS);

  // Demo garlic
  uint8_t image[2312] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,135,0,0,0,196,0,0,0,231,0,0,0,223,0,0,0,195,0,0,0,132,0,0,0,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,139,0,0,0,251,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,253,0,0,0,163,0,0,0,17,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,27,0,0,0,219,0,0,0,255,0,0,0,245,0,0,0,148,0,0,0,69,0,0,0,25,0,0,0,31,0,0,0,70,0,0,0,156,0,0,0,249,0,0,0,255,0,0,0,151,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,254,0,0,0,246,0,0,0,56,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,0,0,0,203,0,0,0,255,0,0,0,217,0,0,0,25,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,206,0,0,0,255,0,0,0,203,0,0,0,16,0,0,0,1,0,0,0,20,0,0,0,42,0,0,0,45,0,0,0,14,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,162,0,0,0,255,0,0,0,98,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,96,0,0,0,255,0,0,0,167,0,0,0,1,0,0,0,95,0,0,0,200,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,233,0,0,0,167,0,0,0,91,0,0,0,219,0,0,0,255,0,0,0,156,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100,0,0,0,255,0,0,0,249,0,0,0,233,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,232,0,0,0,209,0,0,0,211,0,0,0,245,0,0,0,255,0,0,0,255,0,0,0,195,0,0,0,245,0,0,0,254,0,0,0,34,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,163,0,0,0,255,0,0,0,255,0,0,0,231,0,0,0,92,0,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,0,0,0,113,0,0,0,214,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,250,0,0,0,55,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,47,0,0,0,66,0,0,0,22,0,81,0,0,0,251,0,0,0,255,0,0,0,255,0,0,0,206,0,0,0,77,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,0,0,0,239,0,0,0,255,0,0,0,92,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,183,0,0,0,255,0,0,0,104,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,0,0,0,103,0,0,0,248,0,123,0,132,0,254,0,1,0,255,0,0,0,255,0,0,0,232,0,13,0,133,0,2,0,23,0,0,0,0,0,0,0,116,0,0,0,244,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,253,0,2,0,14,0,18,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,25,0,0,0,255,0,0,0,234,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,242,0,0,0,255,0,0,0,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,36,0,4,0,194,0,222,0,33,0,255,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,220,0,0,0,116,0,0,0,13,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,67,0,0,0,145,0,0,0,175,0,2,0,137,0,39,0,24,0,204,0,0,0,255,0,0,0,246,0,0,0,67,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,80,0,0,0,255,0,0,0,203,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,102,0,0,0,255,0,0,0,227,0,0,0,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,111,0,0,0,249,0,0,0,255,0,0,0,255,0,0,0,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,180,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,194,0,0,0,70,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,27,0,0,0,124,0,0,0,242,0,0,0,255,0,0,0,243,0,0,0,59,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,158,0,0,0,255,0,0,0,212,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,225,0,0,0,204,0,0,0,208,0,0,0,248,0,0,0,255,0,0,0,255,0,0,0,241,0,0,0,255,0,0,0,245,0,0,0,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,110,0,0,0,255,0,0,0,243,0,0,0,143,0,0,0,210,0,0,0,252,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,241,0,0,0,171,0,0,0,64,0,0,0,44,0,0,0,255,0,0,0,217,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,224,0,0,0,255,0,0,0,34,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,34,0,0,0,51,0,0,0,49,0,0,0,12,0,0,0,51,0,0,0,232,0,0,0,255,0,0,0,162,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,7,0,0,0,183,0,0,0,255,0,0,0,243,0,0,0,105,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,139,0,0,0,255,0,0,0,175,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,56,0,0,0,251,0,0,0,255,0,0,0,199,0,0,0,108,0,0,0,64,0,0,0,64,0,0,0,123,0,0,0,211,0,0,0,255,0,0,0,255,0,0,0,158,0,0,0,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,80,0,0,0,225,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,255,0,0,0,233,0,0,0,80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,11,0,0,0,93,0,0,0,160,0,0,0,191,0,0,0,193,0,0,0,149,0,0,0,71,0,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  
  for (int img_idx = 0; img_idx < NUM_IMG; img_idx++) {
    for(int px_idx = 0; px_idx < NUM_PIXELS; px_idx++) {
      for (int color_idx = 0; color_idx < 4; ++color_idx) {
        images[img_idx][px_idx][color_idx] = image[4*px_idx + color_idx];
      }
    }
  }

}

String line;

bool reading_img = false;
uint8_t img_idx = 0;
uint16_t pixel_idx = 0;
uint8_t color_idx = 0;

uint8_t num_img = NUM_IMG;
uint8_t display_img_idx = 0;

void handle_line() {
  if (0 == line.length()) {
    return;
  }
  Serial.print("Handling command with ");
  Serial.print(line.length());
  Serial.print(" characters: ");
  Serial.println(line);
  if (line.startsWith("num-images: ")) {
    num_img = line.substring(String("num-images: ").length()).toInt();
    Serial.print("Set num_img to ");
    Serial.println(num_img);
    line = "";
  }
  if (line.startsWith("delay: ")) {
    int const delay = line.substring(String("delay: ").length()).toInt();
    for (int ii = 0; ii < NUM_IMG; ++ii) {
      image_delays[ii] = delay;
    }
    Serial.print("Set delay to ");
    Serial.println(delay);
    line = "";
  }
  if (line.startsWith("set-color-channel-")) {
    Serial.println("Setting the color is not supported in the full RGBW version. Send a color image instead.");
  }
  line = "";
}

uint32_t total_brightness = 0;

// Automatic global brightness scale value to make the display stay below the man_mean_brightness.
uint8_t global_brightness_scale = 255;

uint8_t scale_uint8_t(uint8_t val) {
  return uint8_t((uint16_t(val) * uint16_t(global_brightness_scale))/255);
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

void set_px_color_scaled(uint16_t px_idx, uint8_t color[4]) {
  set_px_accumulate_total_brightness(
    px_idx,
    scale_uint8_t(color[0]),
    scale_uint8_t(color[1]),
    scale_uint8_t(color[2]),
    scale_uint8_t(color[3])
  );
}

void show_img_sub() {
  total_brightness = 0;
  pixels.clear();
  for (uint16_t ii = 0; ii < NUM_PIXELS; ++ii) {
    set_px_color_scaled(ii, images[display_img_idx][ii]);
  }
  /*
  Serial.print("Max total brightness:     ");
  Serial.println(max_total_brightness);
  Serial.print("Current total brightness: ");
  Serial.println(total_brightness);
  */
}

void show_img() {
  /*
  Serial.print("max-num-img: ");
  Serial.println(NUM_IMG);
  Serial.print("num-pixels: ");
  Serial.println(NUM_PIXELS);
  // */
  if (num_img <= 0 || reading_img) {
    return;
  }
  display_img_idx = (display_img_idx + 1) % num_img;
  /*
  Serial.print("\n\nStarting show_img for image #");
  Serial.println(display_img_idx);
  */
  global_brightness_scale = 255;
  show_img_sub();
  if (total_brightness > max_total_brightness) {
    global_brightness_scale = uint8_t((uint64_t(max_total_brightness)*uint64_t(255))/uint64_t(total_brightness));
    /*
    Serial.print("Maximum total brightness too high, scaling by ");
    Serial.println(global_brightness_scale);
    // */
    show_img_sub();
  }
  pixels.show();
  /*
  Serial.print("Pixels for image #");
  Serial.print(display_img_idx);
  Serial.println(" were set and shown");
  // */
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
    if (reading_img) {
      images[img_idx][pixel_idx][color_idx] = current_byte;
      image_sums[img_idx] += current_byte;
      color_idx++;
      if (color_idx >= 4) {
        pixel_idx++;
        color_idx = 0;
      }
      if (NUM_PIXELS == pixel_idx && 3 == color_idx) {
        Serial.print("Finished receiving image #");
        Serial.print(img_idx);
        Serial.print(", image sum is ");
        Serial.println(image_sums[img_idx]);
        pixel_idx = 0;
        reading_img = false;
      }
      return;
    }
    if (current_byte == '\n' || current_byte == '\r') {
      handle_line();
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
    /*
    Serial.println("Current line:");
    Serial.println(line);
    // */
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
      last_serial_input = seconds();
    }
  }
}
