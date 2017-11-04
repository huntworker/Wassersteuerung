#define PIN_TRIGGER 7
#define PIN_ECHO 6
#define PIN_V1 5
#define PIN_V2 4
#define PIN_V3 3
#define PIN_V4 2
#define PIN_BTN 13

#define TIME_FLUSH1 15000 // ms, Membranspülung
#define TIME_FLUSH2 100000 // ms, Stehwasserspülung
#define TIME_FILTER_MAX 5400000 // ms (1.5h)
#define DISTANCE_OFF 300

typedef enum
{
  state_idle = 0,
  state_flush1,
  state_flush2,
  state_filter
} state_t;

uint32_t distance = 0;
uint32_t pulse = 0;
state_t state = state_idle;
uint32_t time_start;

void HC_SR04_Init()
{
  pinMode(PIN_TRIGGER, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
}

void Wasser_Init()
{
  digitalWrite(PIN_V1, LOW);
  digitalWrite(PIN_V2, LOW);
  digitalWrite(PIN_V3, LOW);
  digitalWrite(PIN_V4, LOW);
  
  pinMode(PIN_V1, OUTPUT);
  pinMode(PIN_V2, OUTPUT);
  pinMode(PIN_V3, OUTPUT);
  pinMode(PIN_V4, OUTPUT);

  pinMode(PIN_BTN, INPUT_PULLUP);
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  HC_SR04_Init();
  Wasser_Init();

  Serial.println("Parmeters: ");
  Serial.print("Membranspülung: "); Serial.print(TIME_FLUSH1/1000); Serial.println(" s"); 
  Serial.print("Stehwasserspülung: "); Serial.print(TIME_FLUSH2/1000); Serial.println(" s");
  Serial.print("Maximale Laufzeit: "); Serial.print(TIME_FILTER_MAX/1000); Serial.println(" s");
  Serial.print("minimaler Abstand zum Sensor: "); Serial.print(DISTANCE_OFF); Serial.println(" UNITS");
}

uint32_t HC_SR04_ReadDistance()
{
  digitalWrite(PIN_TRIGGER, LOW);
  delay(5); //ms
  digitalWrite(PIN_TRIGGER, HIGH);
  delay(10); //ms
  digitalWrite(PIN_TRIGGER, LOW);
  pulse = pulseIn(PIN_ECHO, HIGH);
  //distance = pulse / 58;

  Serial.println(pulse);
  /*Serial.print(", ");
  Serial.print(distance);
  Serial.println(" cm");*/
  
  return pulse;
}

void loop() {
  // put your main code here, to run repeatedly:
  static state_t old_state = state_idle;
  
  uint32_t time_delta = millis() - time_start;

  switch (state)
  {
    case state_idle:
    {
      // wait for button
      Serial.println("Waiting for button pressed...");
      while(digitalRead(PIN_BTN));

      // set new state
      state = state_flush1;

      // reset counter
      time_start = millis();
    } break;
    case state_flush1:
    {
      if (time_delta > TIME_FLUSH1)
      {
        // set new state
        state = state_flush2;

        // reset time counter
        time_start = millis();
      }
    } break;
    case state_flush2:
    {
      if (time_delta > TIME_FLUSH2)
      {
        // set new state
        state = state_filter;

        // reset time counter
        time_start = millis();
      }
    } break;
    case state_filter:
    {
      if ( (time_delta > TIME_FILTER_MAX) || (HC_SR04_ReadDistance() < DISTANCE_OFF) )
      {
        // set new state
        state = state_idle;

        // reset time counter
        time_start = millis();
      }
    } break;
  }

  if (old_state != state)
  {
    switch(state)
    {
      case state_idle:
      {
        digitalWrite(PIN_V1, LOW);
        digitalWrite(PIN_V2, LOW);
        digitalWrite(PIN_V3, LOW);
        digitalWrite(PIN_V4, LOW);
        Serial.println("Status: Fertig");
      } break;
      case state_flush1:
      {
        digitalWrite(PIN_V1, HIGH);
        digitalWrite(PIN_V2, LOW);
        digitalWrite(PIN_V3, LOW);
        digitalWrite(PIN_V4, HIGH);
        Serial.println("Status: Membranspülung...");
      } break;
      case state_flush2:
      {
        digitalWrite(PIN_V1, LOW);
        digitalWrite(PIN_V2, HIGH);
        digitalWrite(PIN_V3, LOW);
        digitalWrite(PIN_V4, HIGH);
        Serial.println("Status: Stehwasserspülung...");
      } break;
      case state_filter:
      {
        digitalWrite(PIN_V1, LOW);
        digitalWrite(PIN_V2, LOW);
        digitalWrite(PIN_V3, HIGH);
        digitalWrite(PIN_V4, HIGH);
        Serial.println("Status: Filtern...");
      } break;
    }
    old_state = state;
  }
  
}
