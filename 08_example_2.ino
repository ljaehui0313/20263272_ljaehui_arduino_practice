// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300.0   // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL) // coefficent to convert duration to distance

unsigned long last_sampling_time;   // unit: msec

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);  // sonar TRIGGER
  pinMode(PIN_ECHO, INPUT);   // sonar ECHO
  digitalWrite(PIN_TRIG, LOW);  // turn-off Sonar 
  
  // initialize serial port
  Serial.begin(57600);

  // initialize last sampling time
  last_sampling_time = millis();
}

void loop() { 
  float distance;

  // wait until next sampling time. // polling
  if (millis() < (last_sampling_time + INTERVAL))
    return;

  distance = USS_measure(PIN_TRIG, PIN_ECHO); // read distance

  // Calculate LED brightness based on distance
  int brightness = 255; // default: OFF (Active Low)

  if (distance <= _DIST_MIN || distance >= _DIST_MAX) {
      brightness = 255; // 꺼짐 (100mm 이하, 300mm 이상)
  } else if (distance <= 200.0) {
      // 100mm ~ 200mm 구간: 밝기가 점점 증가 (duty 0% -> 100%)
      // Active Low: PWM 값 255 -> 0
      brightness = 255 - (int)((distance - 100.0) / 100.0 * 255.0);
  } else {
      // 200mm ~ 300mm 구간: 밝기가 점점 감소 (duty 100% -> 0%)
      // Active Low: PWM 값 0 -> 255
      brightness = (int)((distance - 200.0) / 100.0 * 255.0);
  }

  // PWM 값 범위 제한 (0 ~ 255)
  if (brightness < 0) brightness = 0;
  if (brightness > 255) brightness = 255;

  // LED 밝기 적용 (Active Low)
  analogWrite(PIN_LED, brightness);

  // output the distance and brightness to the serial port
  Serial.print("Min:");         Serial.print(_DIST_MIN);
  Serial.print(",distance:");   Serial.print(distance);
  Serial.print(",brightness:"); Serial.print(brightness);
  Serial.print(",Max:");        Serial.print(_DIST_MAX);
  Serial.println("");
  
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
}
