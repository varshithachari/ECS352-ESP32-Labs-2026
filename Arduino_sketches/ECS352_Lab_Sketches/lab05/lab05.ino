// Lab 5: Gates in software and on pins
// Inputs and outputs are single bits: 0 or 1.
uint8_t AND (uint8_t a, uint8_t b) { return a & b; }
uint8_t OR  (uint8_t a, uint8_t b) { return a | b; }
uint8_t NOT (uint8_t a)            { return a ^ 1; }
uint8_t XOR (uint8_t a, uint8_t b) { return a ^ b; }
uint8_t NAND(uint8_t a, uint8_t b) { return NOT(AND(a, b)); }
uint8_t NOR (uint8_t a, uint8_t b) { return NOT(OR(a, b)); }

// The same gates built only from NAND
uint8_t notN(uint8_t a)            { return NAND(a, a); }
uint8_t andN(uint8_t a, uint8_t b) { return notN(NAND(a, b)); }
uint8_t orN (uint8_t a, uint8_t b) { return NAND(notN(a), notN(b)); }
uint8_t xorN(uint8_t a, uint8_t b) {
  uint8_t m = NAND(a, b);
  return NAND(NAND(a, m), NAND(b, m));
}

const int LED_PIN = 2;      // on-board LED on most ESP32 DevKit boards
const int BUTTON_PIN = 0;   // BOOT button: reads 0 when pressed

void printTruthTable(const char* name, uint8_t (*gate)(uint8_t, uint8_t)) {
  Serial.printf("\n%s\n A B | Y\n", name);
  for (uint8_t a = 0; a <= 1; a++)
    for (uint8_t b = 0; b <= 1; b++)
      Serial.printf(" %u %u | %u\n", a, b, gate(a, b));
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n=== Lab 5: Gates in software and on pins ===");

  printTruthTable("AND", AND);
  printTruthTable("OR", OR);
  printTruthTable("XOR", XOR);
  printTruthTable("NAND", NAND);
  printTruthTable("NOR", NOR);
  Serial.printf("\nNOT\n A | Y\n 0 | %u\n 1 | %u\n", NOT(0), NOT(1));

  // Check the NAND-only gates against the originals
  int mismatches = 0;
  for (uint8_t a = 0; a <= 1; a++) {
    if (notN(a) != NOT(a)) mismatches++;
    for (uint8_t b = 0; b <= 1; b++) {
      if (andN(a, b) != AND(a, b)) mismatches++;
      if (orN(a, b)  != OR(a, b))  mismatches++;
      if (xorN(a, b) != XOR(a, b)) mismatches++;
    }
  }
  Serial.printf("\nNAND-only gates: %d mismatches\n", mismatches);

  // De Morgan's laws on every pair of bytes (65 536 cases)
  long fail1 = 0, fail2 = 0;
  for (int x = 0; x < 256; x++) {
    for (int y = 0; y < 256; y++) {
      uint8_t a = x, b = y;
      if ((uint8_t)~(a & b) != (uint8_t)(~a | ~b)) fail1++;
      if ((uint8_t)~(a | b) != (uint8_t)(~a & ~b)) fail2++;
    }
  }
  Serial.printf("De Morgan NOT(A AND B) = NOT A OR NOT B : %ld failures\n", fail1);
  Serial.printf("De Morgan NOT(A OR B) = NOT A AND NOT B : %ld failures\n", fail2);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("\nLED = XOR(button, clock). Press and hold BOOT to see the LED change.");
}

void loop() {
  uint8_t button = (digitalRead(BUTTON_PIN) == LOW) ? 1 : 0;  // 1 when pressed
  uint8_t clockBit = (millis() / 1000) % 2;                   // 0,1,0,1... each second
  uint8_t led = XOR(button, clockBit);
  digitalWrite(LED_PIN, led);
  Serial.printf("button=%u clock=%u -> LED=%u\n", button, clockBit, led);
  delay(250);
}
