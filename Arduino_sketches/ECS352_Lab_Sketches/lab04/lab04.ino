// Lab 4: Which end comes first?

void dumpBytes(const char* label, const void* p, size_t n) {
  const uint8_t* bytes = (const uint8_t*)p;
  Serial.printf("\n%s\n", label);
  for (size_t i = 0; i < n; i++) {
    Serial.printf("  address %p : 0x%02X\n", (const void*)(bytes + i), bytes[i]);
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n=== Lab 4: Which end comes first? ===");

  uint32_t number = 0x12345678;
  float f = 1.0f;                // bits 0x3F800000
  char text[5] = "ABCD";

  dumpBytes("uint32_t 0x12345678:", &number, sizeof number);
  dumpBytes("float 1.0 (0x3F800000):", &f, sizeof f);
  dumpBytes("string \"ABCD\":", text, 4);
}

void loop() {}
