// Lab 8: Draw the memory map
int globalInitialised = 42;                 // initialised global
int globalUninitialised[100];               // uninitialised global
const uint8_t constTable[1000] = { 1, 2, 3 }; // constant data

void normalFunction() {}
void IRAM_ATTR fastFunction() {}            // asked to be placed in internal RAM

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n=== Lab 8: Draw the memory map ===");

  static int staticLocal = 7;               // static local variable
  int localVariable = 5;                    // ordinary local (on the stack)
  int* heapBlock = (int*)malloc(100);       // dynamically allocated (on the heap)
  const char* literal = "hello";            // string literal

  Serial.printf("normalFunction (code)          : %p\n", (void*)&normalFunction);
  Serial.printf("fastFunction (IRAM_ATTR code)  : %p\n", (void*)&fastFunction);
  Serial.printf("constTable (const data)        : %p\n", (const void*)constTable);
  Serial.printf("string literal \"hello\"         : %p\n", (const void*)literal);
  Serial.printf("globalInitialised              : %p\n", (void*)&globalInitialised);
  Serial.printf("globalUninitialised            : %p\n", (void*)globalUninitialised);
  Serial.printf("staticLocal                    : %p\n", (void*)&staticLocal);
  Serial.printf("heapBlock (malloc)             : %p\n", (void*)heapBlock);
  Serial.printf("localVariable (stack)          : %p\n", (void*)&localVariable);

  // Use the values so the compiler keeps everything
  Serial.printf("\n(values: %d %d %u %d %d %c)\n", globalInitialised, globalUninitialised[0],
                constTable[1], staticLocal, localVariable, literal[0]);
  free(heapBlock);
}

void loop() {}
