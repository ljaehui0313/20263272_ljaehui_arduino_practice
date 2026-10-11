// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100     // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300     // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficent to convert duration to distance

#define _EMA_ALPHA 0.8    // EMA weight of new sample (range: 0 to 1)

//N=3, 10, 30 variables
#define MEDIAN_N 10

// global variables
unsigned long last_sampling_time;   // unit: msec
float dist_ema;                     // EMA distance
float dist_median;                  // Median distance

// 중위수 필터용 버퍼 및 인덱스
float raw_buffer[MEDIAN_N];
int buffer_idx = 0;

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);

  // initialize serial port
  Serial.begin(57600);
}

// 배열 정렬 함수
void sort_array(float arr[], int n) {
  for (int i = 0; i < n - 1; i++) {
    for (int j = 0; j < n - i - 1; j++) {
      if (arr[j] > arr[j + 1]) {
        float temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }
}

// 중위수 계산 함수
float get_median(float new_val) {
  raw_buffer[buffer_idx] = new_val;
  buffer_idx = (buffer_idx + 1) % MEDIAN_N;

  float sorted[MEDIAN_N];
  for (int i = 0; i < MEDIAN_N; i++) {
    sorted[i] = raw_buffer[i];
  }

  sort_array(sorted, MEDIAN_N);
  return sorted[MEDIAN_N / 2];
}

void loop() {
  float dist_raw;
  
  // wait until next sampling time.
  if (millis() < last_sampling_time + INTERVAL)
    return;

  // 1. 센서 raw 측정값 읽기
  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  // 2. [직전 유효값 사용 코드 제거] raw 데이터에 중위수 필터 적용
  dist_median = get_median(dist_raw);

  // 3. EMA 계산 (raw 데이터 기반)
  dist_ema = _EMA_ALPHA * dist_raw + (1.0 - _EMA_ALPHA) * dist_ema;

  // 4. 시리얼 플로터 지정 출력 양식 준수
  Serial.print("Min:");      Serial.print(_DIST_MIN);
  Serial.print(",raw:");     Serial.print(dist_raw);
  Serial.print(",ema:");     Serial.print(dist_ema);
  Serial.print(",median:");  Serial.print(dist_median);
  Serial.print(",Max:");     Serial.print(_DIST_MAX);
  Serial.println("");

  // LED 제어
  if ((dist_raw < _DIST_MIN) || (dist_raw > _DIST_MAX))
    digitalWrite(PIN_LED, 1);       // LED OFF
  else
    digitalWrite(PIN_LED, 0);       // LED ON

  // update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm
}void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
