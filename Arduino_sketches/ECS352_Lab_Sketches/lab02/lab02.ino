// Lab 2: Integers have edges

// Print a 32-bit value in binary, with a space every 8 bits
void printBin32(uint32_t v) {
  for (int i = 31; i >= 0; i--) {
    Serial.print((v >> i) & 1);
    if (i % 8 == 0 && i != 0) Serial.print(' ');
  }
}

void show(const char* label, int32_t v) {
  Serial.printf("%-22s %12ld  0x%08lX  ", label, (long)v, (unsigned long)(uint32_t)v);
  printBin32((uint32_t)v);
  Serial.println();
}

void showU(const char* label, uint32_t v) {
  Serial.printf("%-22s %12lu  0x%08lX  ", label, (unsigned long)v, (unsigned long)v);
  printBin32(v);
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n=== Lab 2: Integers have edges ===");

  Serial.println("\n-- Some signed 32-bit values --");
  show("5", 5);
  show("-5", -5);
  show("0", 0);
  show("-1", -1);
  show("INT32_MAX", INT32_MAX);
  show("INT32_MIN", INT32_MIN);

  Serial.println("\n-- Overflow --");
  // The hardware adder simply wraps around. (In C, signed overflow is
  // formally undefined, so we add the bit patterns as unsigned numbers.)
  int32_t wrapped = (int32_t)((uint32_t)INT32_MAX + 1u);
  show("INT32_MAX + 1", wrapped);
  uint32_t uwrapped = UINT32_MAX + 1u;
  showU("UINT32_MAX", UINT32_MAX);
  showU("UINT32_MAX + 1", uwrapped);

  Serial.println("\n-- Same 8 bits, two meanings --");
  int8_t  s8 = -1;    // bits 1111 1111
  uint8_t u8 = 255;   // bits 1111 1111
  show("int8_t -1 -> int32_t", (int32_t)s8);
  show("uint8_t 255 -> int32_t", (int32_t)u8);
  showU("int8_t -1 -> uint32_t", (uint32_t)(int32_t)s8);
}

void loop() {}
