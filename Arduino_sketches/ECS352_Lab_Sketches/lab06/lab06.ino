// Lab 6: Build an adder from gates (no + allowed inside the adder)
uint8_t AND(uint8_t a, uint8_t b) { return a & b; }
uint8_t OR (uint8_t a, uint8_t b) { return a | b; }
uint8_t XOR(uint8_t a, uint8_t b) { return a ^ b; }
uint8_t NOT(uint8_t a)            { return a ^ 1; }

void halfAdder(uint8_t a, uint8_t b, uint8_t* sum, uint8_t* carry) {
  *sum = XOR(a, b);
  *carry = AND(a, b);
}

void fullAdder(uint8_t a, uint8_t b, uint8_t cin, uint8_t* sum, uint8_t* cout) {
  uint8_t s1, c1, c2;
  halfAdder(a, b, &s1, &c1);
  halfAdder(s1, cin, sum, &c2);
  *cout = OR(c1, c2);
}

// 8-bit ripple-carry adder. If subtract = 1, computes a - b as a + NOT(b) + 1.
uint8_t addSub8(uint8_t a, uint8_t b, uint8_t subtract, uint8_t* carryOut, uint8_t* overflow) {
  uint8_t result = 0;
  uint8_t carry = subtract;          // carry-in is 1 for subtraction
  uint8_t carryIntoMsb = 0;
  for (int i = 0; i < 8; i++) {
    uint8_t ai = (a >> i) & 1;
    uint8_t bi = (b >> i) & 1;
    if (subtract) bi = NOT(bi);
    if (i == 7) carryIntoMsb = carry;
    uint8_t s;
    fullAdder(ai, bi, carry, &s, &carry);
    result |= (uint8_t)(s << i);
  }
  *carryOut = carry;
  *overflow = XOR(carryIntoMsb, carry);  // signed overflow
  return result;
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n=== Lab 6: Build an adder from gates ===");

  // A few examples
  uint8_t c, v;
  uint8_t examples[][2] = { {100, 27}, {200, 100}, {100, 100}, {128, 128}, {5, 7} };
  Serial.println("\n   a    b |  a+b  C V |  a-b  C V");
  for (auto& e : examples) {
    uint8_t s = addSub8(e[0], e[1], 0, &c, &v);
    Serial.printf(" %3u  %3u |  %3u  %u %u |", e[0], e[1], s, c, v);
    s = addSub8(e[0], e[1], 1, &c, &v);
    Serial.printf("  %3u  %u %u\n", s, c, v);
  }

  // Exhaustive test against the processor's own + and -
  long errors = 0;
  for (int a = 0; a < 256; a++) {
    for (int b = 0; b < 256; b++) {
      uint8_t s = addSub8(a, b, 0, &c, &v);
      int signedSum = (int8_t)a + (int8_t)b;
      if (s != (uint8_t)(a + b)) errors++;
      if (c != ((a + b) > 255)) errors++;
      if (v != (signedSum < -128 || signedSum > 127)) errors++;

      s = addSub8(a, b, 1, &c, &v);
      int signedDiff = (int8_t)a - (int8_t)b;
      if (s != (uint8_t)(a - b)) errors++;
      if (c != (a >= b)) errors++;           // carry = 1 means "no borrow"
      if (v != (signedDiff < -128 || signedDiff > 127)) errors++;
    }
  }
  Serial.printf("\nExhaustive test (65 536 pairs, add and subtract): %ld errors\n", errors);

  // Speed: gate-level adder vs the hardware +
  volatile uint8_t x = 123, y = 45, out;
  uint32_t t0 = ESP.getCycleCount();
  for (int i = 0; i < 1000; i++) out = addSub8(x, y, 0, &c, &v);
  uint32_t t1 = ESP.getCycleCount();
  for (int i = 0; i < 1000; i++) out = x + y;
  uint32_t t2 = ESP.getCycleCount();
  Serial.printf("Gate-level adder : %.1f cycles per addition\n", (t1 - t0) / 1000.0f);
  Serial.printf("Hardware +       : %.1f cycles per addition\n", (t2 - t1) / 1000.0f);
  (void)out;
}

void loop() {}
