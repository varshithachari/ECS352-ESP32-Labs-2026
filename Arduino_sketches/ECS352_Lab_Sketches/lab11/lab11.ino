// Lab 11: What survives a reset?
#include <Preferences.h>

Preferences prefs;
int ramCounter = 0;          // lives in RAM: volatile

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n=== Lab 11: What survives a reset? ===");

  ramCounter++;

  prefs.begin("lab11", false);                       // open storage in flash
  unsigned int flashCounter = prefs.getUInt("boots", 0);
  flashCounter++;
  prefs.putUInt("boots", flashCounter);

  Serial.printf("RAM counter   (global variable): %d\n", ramCounter);
  Serial.printf("Flash counter (Preferences)    : %u\n", flashCounter);

  // Timing: RAM write/read vs flash write/read
  volatile uint32_t ramValue = 0;
  int64_t t0 = esp_timer_get_time();
  ramValue = 12345;
  int64_t t1 = esp_timer_get_time();
  uint32_t r = ramValue;
  int64_t t2 = esp_timer_get_time();
  prefs.putUInt("test", 12345 + flashCounter);       // a new value forces a real write
  int64_t t3 = esp_timer_get_time();
  uint32_t f = prefs.getUInt("test", 0);
  int64_t t4 = esp_timer_get_time();

  Serial.printf("\nRAM   write: %lld us   read: %lld us\n", (long long)(t1 - t0), (long long)(t2 - t1));
  Serial.printf("Flash write: %lld us   read: %lld us\n", (long long)(t3 - t2), (long long)(t4 - t3));
  Serial.printf("(values read back: %lu, %lu)\n", (unsigned long)r, (unsigned long)f);
  prefs.end();

  Serial.println("\nNow press the EN (reset) button, then try unplugging the board.");
}

void loop() {}
