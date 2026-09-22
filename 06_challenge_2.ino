#define LED_PIN 7

int default_period = 1000; // 1ms = 1000us
int duty = 0;              // 0 ~ 100%
int fadeamount = 2;        // 0->100->0

void set_period(int period) {
  default_period = period;
}

void set_duty(int duty_rate) {
  int high_time = default_period * duty_rate / 100;
  int low_time = default_period - high_time;

  if (high_time > 0) {
    digitalWrite(LED_PIN, HIGH);
    delayMicroseconds(high_time);
  }
    
  if (low_time > 0) {
    digitalWrite(LED_PIN, LOW);
    delayMicroseconds(low_time);
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  //여기서 시간 설정
  set_period(10000);
}

void loop() {
  for (int d = 0; d <= 100; d += fadeamount) {
    int cycles = 10000 / default_period;
    for (int i = 0; i < cycles; i++) {
      set_duty(d);
    }
  }

  for (int d = 100; d >= 0; d -= fadeamount) {
    int cycles = 10000 / default_period; // 각 단계마다 10ms 동안 유지하기 위한 반복 횟수
    for (int i = 0; i < cycles; i++) {
      set_duty(d);
    }
  }
}
