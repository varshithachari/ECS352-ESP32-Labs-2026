// Lab 9: Watch the stack grow
// Type a command in the Serial Monitor and press Enter:
//   S 50   -> recurse 50 levels with a small frame
//   B 50   -> recurse 50 levels with a 100-byte local array in each frame

int printEvery = 1;

int recurseSmall(int depth, int maxDepth) {
  volatile int marker = depth;            // a local variable on the stack
  if (depth <= 2 || depth % printEvery == 0 || depth == maxDepth)
    Serial.printf("small depth %4d  local at %p  min stack left %5u bytes\n",
                  depth, (void*)&marker, (unsigned)uxTaskGetStackHighWaterMark(NULL));
  if (depth < maxDepth) return recurseSmall(depth + 1, maxDepth) + marker;
  return marker;
}

int recurseBig(int depth, int maxDepth) {
  volatile char buffer[100];              // 100 bytes more per call
  buffer[0] = (char)depth;
  if (depth <= 2 || depth % printEvery == 0 || depth == maxDepth)
    Serial.printf("big   depth %4d  local at %p  min stack left %5u bytes\n",
                  depth, (void*)buffer, (unsigned)uxTaskGetStackHighWaterMark(NULL));
  if (depth < maxDepth) return recurseBig(depth + 1, maxDepth) + buffer[0];
  return buffer[0];
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n=== Lab 9: Watch the stack grow ===");
  Serial.println("Type S <depth> or B <depth>, for example: S 10");
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() < 3) return;
    char kind = toupper(line.charAt(0));
    int depth = line.substring(2).toInt();
    printEvery = (depth > 50) ? depth / 20 : 1;   // keep the output short
    Serial.printf("\n--- %c %d ---\n", kind, depth);
    int result = (kind == 'B') ? recurseBig(1, depth) : recurseSmall(1, depth);
    Serial.printf("returned safely (result %d)\n", result);
  }
}
