// Lab 7: A tiny ALU and the cost of operations
enum Op { ADD, SUB, AND_OP, OR_OP, XOR_OP, NOT_OP, SHL };
const char* opNames[] = { "ADD", "SUB", "AND", "OR", "XOR", "NOT", "SHL" };

struct Flags { uint8_t zero, carry, sign, overflow; };

// 8-bit ALU: the switch plays the role of the multiplexer that selects the result
uint8_t alu(uint8_t a, uint8_t b, Op op, Flags* f) {
  uint16_t wide = 0;
  uint8_t r = 0;
  f->carry = 0;
  f->overflow = 0;
  switch (op) {
    case ADD:
      wide = (uint16_t)a + b;
      r = (uint8_t)wide;
      f->carry = wide > 0xFF;
      f->overflow = ((~(a ^ b) & (a ^ r)) >> 7) & 1;
      break;
    case SUB:
      r = (uint8_t)(a - b);
      f->carry = a >= b;                       // no borrow
      f->overflow = (((a ^ b) & (a ^ r)) >> 7) & 1;
      break;
    case AND_OP: r = a & b; break;
    case OR_OP:  r = a | b; break;
    case XOR_OP: r = a ^ b; break;
    case NOT_OP: r = ~a; break;
    case SHL:
      r = (uint8_t)(a << 1);
      f->carry = (a >> 7) & 1;                 // bit shifted out
      break;
  }
  f->zero = (r == 0);
  f->sign = (r >> 7) & 1;
  return r;
}

// Multiplication by shift-and-add: only shifts, AND and addition
uint16_t shiftAddMultiply(uint8_t a, uint8_t b) {
  uint16_t product = 0;
  uint16_t shifted = a;
  for (int i = 0; i < 8; i++) {
    if ((b >> i) & 1) product += shifted;
    shifted <<= 1;
  }
  return product;
}

#define TIME_OP(label, expr)                                         \
  {                                                                  \
    uint32_t t0 = ESP.getCycleCount();                               \
    for (int i = 0; i < 10000; i++) { expr; }                        \
    uint32_t t1 = ESP.getCycleCount();                               \
    Serial.printf("%-14s %6.1f cycles\n", label, (t1 - t0) / 10000.0f); \
  }

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n=== Lab 7: A tiny ALU and the cost of operations ===");

  Serial.println("\nALU with a = 200, b = 100");
  Serial.println(" op  | result | Z C S V");
  for (int op = ADD; op <= SHL; op++) {
    Flags f;
    uint8_t r = alu(200, 100, (Op)op, &f);
    Serial.printf(" %-4s|  %3u   | %u %u %u %u\n", opNames[op], r, f.zero, f.carry, f.sign, f.overflow);
  }

  long errors = 0;
  for (int a = 0; a < 256; a++)
    for (int b = 0; b < 256; b++)
      if (shiftAddMultiply(a, b) != a * b) errors++;
  Serial.printf("\nShift-and-add multiply, all 65 536 pairs: %ld errors\n", errors);

  Serial.println("\nAverage cost per operation (includes loop overhead):");
  volatile int32_t ia = 123456, ib = 789, ir;
  volatile float fa = 1234.5f, fb = 6.78f, fr;
  TIME_OP("empty loop", ir = ia);
  TIME_OP("int32 +", ir = ia + ib);
  TIME_OP("int32 *", ir = ia * ib);
  TIME_OP("int32 /", ir = ia / ib);
  TIME_OP("int32 %", ir = ia % ib);
  TIME_OP("int32 <<", ir = ia << 3);
  TIME_OP("float +", fr = fa + fb);
  TIME_OP("float *", fr = fa * fb);
  TIME_OP("float /", fr = fa / fb);
  (void)ir; (void)fr;

  Serial.println("\nx / 8 vs x >> 3 for negative x:");
  int32_t xs[] = { 17, -17, -8, -1 };
  for (int32_t x : xs)
    Serial.printf("x = %4ld   x / 8 = %3ld   x >> 3 = %3ld\n", (long)x, (long)(x / 8), (long)(x >> 3));
}

void loop() {}
