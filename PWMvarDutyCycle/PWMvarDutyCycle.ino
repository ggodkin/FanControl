const int pwmPin = 3;  // PWM output pin
const int increaseButtonPin = 2;  // Pin for increasing duty cycle
const int decreaseButtonPin = 4;  // Pin for decreasing duty cycle
const int numLeds = 10;

int dutyCycle = 50;  // Initial duty cycle in percentage

unsigned long lastIncreaseTime = 0;
unsigned long lastDecreaseTime = 0;
unsigned long debounceDelay = 100;  // Adjust as needed

// Define pins for LEDs
const int ledPins[] = {5, 6, 7, 8, 9, 10, 11, 12, 0, 1};  // Adjusted LED pins

void setup() {
  // Serial.begin(115200);
  pinMode(pwmPin, OUTPUT);
  pinMode(increaseButtonPin, INPUT_PULLUP);
  pinMode(decreaseButtonPin, INPUT_PULLUP);

  // Setup LED pins
  for (int i = 0; i < numLeds; ++i) {
    pinMode(ledPins[i], OUTPUT);
  }

pwm25kHzBegin();
}

void loop() {
  updateButtons();
  updatePWM();
}

void pwm25kHzBegin() {
  TCCR2A = 0;                               // TC2 Control Register A
  TCCR2B = 0;                               // TC2 Control Register B
  TIMSK2 = 0;                               // TC2 Interrupt Mask Register
  TIFR2 = 0;                                // TC2 Interrupt Flag Register
  TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);  // OC2B cleared/set on match when up/down counting, fast PWM
  // TCCR2B |= (1 << WGM22) | (1 << CS21);     // prescaler 8
  TCCR2B |= (1 << WGM22) | (1 << CS20);     // prescaler 1 25kHz at internal clock 2mHz
  OCR2A = 79;                               // TOP overflow value (Hz)
  OCR2B = 0;
}

void updateButtons() {
  unsigned long currentTime = millis();

  // Increase button
  if (digitalRead(increaseButtonPin) == LOW && currentTime - lastIncreaseTime > debounceDelay) {
    dutyCycle = min(dutyCycle + 5, 100);  // Increase duty cycle by 5%
    lastIncreaseTime = currentTime;
  }

  // Decrease button
  if (digitalRead(decreaseButtonPin) == LOW && currentTime - lastDecreaseTime > debounceDelay) {
    dutyCycle = max(dutyCycle - 5, 1);  // Decrease duty cycle by 5%
    lastDecreaseTime = currentTime;
  }
}

void updateLeds() {
  int numLitLeds = map(dutyCycle, 1, 100, 1, numLeds);

  for (int i = 0; i < numLeds; ++i) {
    digitalWrite(ledPins[i], i < numLitLeds ? HIGH : LOW);
  }
}

void updatePWM() {
  // Map duty cycle to the PWM range (0 to 79 for Timer2)
  // Serial.print("Duty Cycle: ");
  // Serial.println(dutyCycle);
  int pwmValue = map(dutyCycle, 1, 100, 1, 79);
  OCR2B = pwmValue;
  // Serial.print("PWM: ");
  // Serial.println(pwmValue);
  updateLeds();  // Update LED bar based on the duty cycle
}
