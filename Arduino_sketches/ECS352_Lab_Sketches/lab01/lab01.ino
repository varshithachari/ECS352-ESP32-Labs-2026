// Lab 1: Meet your board
void setup() {
  Serial.begin(115200);
  delay(2000);  // give the Serial Monitor time to connect
  Serial.println("\n=== Lab 1: Meet your board ===");

  Serial.printf("Chip model    : %s (revision %d)\n", ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("CPU cores     : %d\n", ESP.getChipCores());
  Serial.printf("CPU clock     : %lu MHz\n", (unsigned long)ESP.getCpuFreqMHz());
  Serial.printf("Flash size    : %lu bytes\n", (unsigned long)ESP.getFlashChipSize());
  Serial.printf("Free heap     : %lu bytes\n", (unsigned long)ESP.getFreeHeap());

  // Count CPU cycles across one second
  uint32_t c1 = ESP.getCycleCount();
  delay(1000);
  uint32_t c2 = ESP.getCycleCount();
  Serial.printf("Cycles in 1 s : %lu\n", (unsigned long)(c2 - c1));

  // Size of each C type on this processor
  Serial.println("\nType sizes (bytes):");
  Serial.printf("char      : %u\n", (unsigned)sizeof(char));
  Serial.printf("short     : %u\n", (unsigned)sizeof(short));
  Serial.printf("int       : %u\n", (unsigned)sizeof(int));
  Serial.printf("long      : %u\n", (unsigned)sizeof(long));
  Serial.printf("long long : %u\n", (unsigned)sizeof(long long));
  Serial.printf("float     : %u\n", (unsigned)sizeof(float));
  Serial.printf("double    : %u\n", (unsigned)sizeof(double));
  Serial.printf("pointer   : %u\n", (unsigned)sizeof(void*));
}

void loop() {}
