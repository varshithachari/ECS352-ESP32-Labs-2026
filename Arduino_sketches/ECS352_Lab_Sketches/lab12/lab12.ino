// Lab 12: Two cores and the limits of parallelism
#include <math.h>

const long N = 1000000;

struct Job { long start; long end; float result; };
SemaphoreHandle_t doneSignal;

float sumRoots(long start, long end) {
  float s = 0.0f;
  for (long i = start; i < end; i++) s += sqrtf((float)i);
  return s;
}

void worker(void* arg) {
  Job* job = (Job*)arg;
  job->result = sumRoots(job->start, job->end);
  xSemaphoreGive(doneSignal);
  vTaskDelete(NULL);
}

// Run [start, end) split in two halves, one on each core
float runOnTwoCores(long start, long end) {
  long mid = start + (end - start) / 2;
  Job j0 = { start, mid, 0.0f };
  Job j1 = { mid, end, 0.0f };
  xTaskCreatePinnedToCore(worker, "w0", 4096, &j0, 1, NULL, 0);
  xTaskCreatePinnedToCore(worker, "w1", 4096, &j1, 1, NULL, 1);
  xSemaphoreTake(doneSignal, portMAX_DELAY);
  xSemaphoreTake(doneSignal, portMAX_DELAY);
  return j0.result + j1.result;
}

// Shared counter experiment
volatile uint32_t sharedCounter = 0;
portMUX_TYPE counterLock = portMUX_INITIALIZER_UNLOCKED;
volatile bool useLock = false;

void counterTask(void* arg) {
  for (int i = 0; i < 1000000; i++) {
    if (useLock) {
      portENTER_CRITICAL(&counterLock);
      sharedCounter++;
      portEXIT_CRITICAL(&counterLock);
    } else {
      sharedCounter++;
    }
  }
  xSemaphoreGive(doneSignal);
  vTaskDelete(NULL);
}

uint32_t runCounter(bool locked) {
  sharedCounter = 0;
  useLock = locked;
  xTaskCreatePinnedToCore(counterTask, "c0", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(counterTask, "c1", 4096, NULL, 1, NULL, 1);
  xSemaphoreTake(doneSignal, portMAX_DELAY);
  xSemaphoreTake(doneSignal, portMAX_DELAY);
  return sharedCounter;
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n=== Lab 12: Two cores and the limits of parallelism ===");
  Serial.printf("setup() runs on core %d\n", xPortGetCoreID());
  doneSignal = xSemaphoreCreateCounting(2, 0);

  // 1. Fully parallel job
  int64_t t0 = esp_timer_get_time();
  float one = sumRoots(0, N);
  int64_t t1 = esp_timer_get_time();
  float two = runOnTwoCores(0, N);
  int64_t t2 = esp_timer_get_time();
  Serial.printf("\nOne core : %8lld us  sum = %.2f\n", (long long)(t1 - t0), one);
  Serial.printf("Two cores: %8lld us  sum = %.2f\n", (long long)(t2 - t1), two);
  Serial.printf("Speed-up : %.2f x\n", (float)(t1 - t0) / (float)(t2 - t1));

  // 2. 25% of the work must run first on one core
  long serialPart = N / 4;
  int64_t t3 = esp_timer_get_time();
  float a = sumRoots(0, serialPart);
  float b = runOnTwoCores(serialPart, N);
  int64_t t4 = esp_timer_get_time();
  Serial.printf("\nWith a 25%% serial part: %lld us  sum = %.2f\n", (long long)(t4 - t3), a + b);
  Serial.printf("Speed-up vs one core   : %.2f x\n", (float)(t1 - t0) / (float)(t4 - t3));

  // 3. Two cores, one shared counter
  int64_t t5 = esp_timer_get_time();
  uint32_t unlocked = runCounter(false);
  int64_t t6 = esp_timer_get_time();
  uint32_t locked = runCounter(true);
  int64_t t7 = esp_timer_get_time();
  Serial.printf("\nShared counter, expected 2000000\n");
  Serial.printf("Without lock: %lu  (%lld us)\n", (unsigned long)unlocked, (long long)(t6 - t5));
  Serial.printf("With lock   : %lu  (%lld us)\n", (unsigned long)locked, (long long)(t7 - t6));
}

void loop() {}
