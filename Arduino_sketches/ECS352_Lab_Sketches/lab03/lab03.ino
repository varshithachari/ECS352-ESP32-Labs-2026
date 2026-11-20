// Lab 3: Floats are not real numbers
#include <string.h>

void showFloat(const char* label, float f) {
  uint32_t b;
  memcpy(&b, &f, sizeof b);              // copy the raw 32 bits
  uint32_t sign = b >> 31;
  uint32_t exponent = (b >> 23) & 0xFF;
  uint32_t mantissa = b & 0x7FFFFF;
  Serial.printf("%-8s 0x%08lX  sign=%lu  ", label, (unsigned long)b, (unsigned long)sign);
  Serial.print("exponent=");
  for (int i = 7; i >= 0; i--) Serial.print((exponent >> i) & 1);
  Serial.printf(" (%lu)  mantissa=", (unsigned long)exponent);
  for (int i = 22; i >= 0; i--) Serial.print((mantissa >> i) & 1);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n=== Lab 3: Floats are not real numbers ===");

  Serial.println("\n-- IEEE 754 bit fields --");
  showFloat("1.0", 1.0f);
  showFloat("0.1", 0.1f);
  showFloat("-2.5", -2.5f);
  showFloat("0.0", 0.0f);

  Serial.println("\n-- Adding 0.1 one million times --");
  float  sumF = 0.0f;
  double sumD = 0.0;
  for (long i = 0; i < 1000000; i++) {
    sumF += 0.1f;
    sumD += 0.1;
  }
  Serial.printf("float  total: %.8f\n", sumF);
  Serial.printf("double total: %.8f\n", sumD);
  Serial.println("expected   : 100000.00000000");

  Serial.println("\n-- The gap between floats --");
  float big = 16777216.0f;               // 2^24
  float bigPlusOne = big + 1.0f;
  Serial.printf("2^24     = %.1f\n", big);
  Serial.printf("2^24 + 1 = %.1f\n", bigPlusOne);

  Serial.println("\n-- Cost of 10 000 additions (cycles) --");
  volatile int32_t vi = 0;
  volatile float   vf = 0.0f;
  volatile double  vd = 0.0;
  uint32_t t0 = ESP.getCycleCount();
  for (int i = 0; i < 10000; i++) vi = vi + 3;
  uint32_t t1 = ESP.getCycleCount();
  for (int i = 0; i < 10000; i++) vf = vf + 3.0f;
  uint32_t t2 = ESP.getCycleCount();
  for (int i = 0; i < 10000; i++) vd = vd + 3.0;
  uint32_t t3 = ESP.getCycleCount();
  Serial.printf("int32  : %lu cycles (%.1f per add)\n", (unsigned long)(t1 - t0), (t1 - t0) / 10000.0f);
  Serial.printf("float  : %lu cycles (%.1f per add)\n", (unsigned long)(t2 - t1), (t2 - t1) / 10000.0f);
  Serial.printf("double : %lu cycles (%.1f per add)\n", (unsigned long)(t3 - t2), (t3 - t2) / 10000.0f);
}

void loop() {}
