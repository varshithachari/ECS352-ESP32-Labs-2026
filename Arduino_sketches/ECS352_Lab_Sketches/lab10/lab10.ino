// Lab 10: Heap and fragmentation
const int MAX_BLOCKS = 400;
void* blocks[MAX_BLOCKS];

void heapReport(const char* when) {
  Serial.printf("%-34s free heap %7lu  largest free block %7lu\n", when,
                (unsigned long)ESP.getFreeHeap(),
                (unsigned long)heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
}

void tryAllocate(size_t bytes) {
  void* p = malloc(bytes);
  Serial.printf("  malloc(%u) -> %s\n", (unsigned)bytes, p ? "SUCCESS" : "FAILED");
  free(p);
}

void taskFunction(void* arg) {
  int localInTask = 1;
  Serial.printf("\nA local variable inside a new task is at %p\n", (void*)&localInTask);
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n=== Lab 10: Heap and fragmentation ===\n");

  // 1. How big is an allocation really?
  uint8_t* a = (uint8_t*)malloc(1);
  uint8_t* b = (uint8_t*)malloc(1);
  uint8_t* c = (uint8_t*)malloc(8);
  uint8_t* d = (uint8_t*)malloc(100);
  uint8_t* e = (uint8_t*)malloc(1);
  Serial.printf("malloc(1)   at %p\n", (void*)a);
  Serial.printf("malloc(1)   at %p  (gap %ld)\n", (void*)b, (long)(b - a));
  Serial.printf("malloc(8)   at %p  (gap %ld)\n", (void*)c, (long)(c - b));
  Serial.printf("malloc(100) at %p  (gap %ld)\n", (void*)d, (long)(d - c));
  Serial.printf("malloc(1)   at %p  (gap %ld)\n\n", (void*)e, (long)(e - d));
  free(a); free(b); free(c); free(d); free(e);

  // 2. Fill most of the heap with 1 KB blocks
  heapReport("At start:");
  int count = 0;
  while (count < MAX_BLOCKS && ESP.getFreeHeap() > 16 * 1024) {
    blocks[count] = malloc(1024);
    if (!blocks[count]) break;
    if (count < 5) Serial.printf("  block %d at %p\n", count, blocks[count]);
    count++;
  }
  Serial.printf("  ... allocated %d blocks of 1 KB\n", count);
  heapReport("After filling:");

  // 3. Free every second block
  for (int i = 0; i < count; i += 2) { free(blocks[i]); blocks[i] = NULL; }
  heapReport("After freeing every second block:");
  tryAllocate(24 * 1024);

  // 4. Free the rest
  for (int i = 1; i < count; i += 2) { free(blocks[i]); blocks[i] = NULL; }
  heapReport("After freeing all blocks:");
  tryAllocate(24 * 1024);

  // 5. Where does a task's stack live?
  void* probe = malloc(16);
  Serial.printf("\nA fresh heap block is at %p\n", probe);
  free(probe);
  xTaskCreate(taskFunction, "probe", 4096, NULL, 1, NULL);
}

void loop() {}
