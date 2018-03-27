/*
   -------------------------------------------------------------
   OCTA EIT - Especialista em TI
   Programa para Acionamento de aromatizador de ambiente por
   intervalos de tempo através de Dip Switch de 4 posições
   BY::RAFAEL VALIM
   VERSAO: 2.1
   ------------------------------------------------------------
   posição 1 = 5 min
   posição 2 = 10 min
   posição 3 = 20 min
   posição 4 = 30 min
   Ex: para 15 min pino 1+2 para cima.
   Tempo de acionamento do Relay é de 2 segundos(spray)
   Pino 6 = Relay - Motor Spray
   Pino 13 = Led auxiliar - para status do Arduino
   Pino 8 = Push Button - para acionamento manual do spray
   ------------------------------------------------------------
   Utilizar pino 13 para mostrar o status do Arduino para saber
   se esta travado
   -----------------------------------------------------------
*/

#define S1 2
#define S2 3
#define S3 4
#define S4 5
int s1state = HIGH;
int s2state = HIGH;
int s3state = HIGH;
int s4state = HIGH;
int pinButton = 8; //Pino push button

int pinoSpray = 6; //Pino do Spray
int pinoCooler = 7; //Pino do cooler
int pinoStatus = 13; //Pino do Led de Status
long previousMillis = 0;
long ledmilis = 0;
int statusButton = 0;
boolean acionaManual = false;

unsigned long timeToOnSpray = 0;
unsigned long timeToOnLed = 0;

unsigned long timeToOffSpray = 0;
unsigned long timeToOffCooler = 0;
unsigned long timeToOffLed = 0;

unsigned long timer = 0;
unsigned long ledTimer = 4; //Tempo do led de Status

unsigned long coolerTime = 0;
unsigned long sprayTime = 2;//Tempo de Spray

boolean isSprayOn;
boolean isLedOn;
boolean isCoolerOn;
int percentualCooler = 70;

void setup()
{

  Serial.begin(9600);
  pinMode(S1, INPUT_PULLUP);
  pinMode(S2, INPUT_PULLUP);
  pinMode(S3, INPUT_PULLUP);
  pinMode(S4, INPUT_PULLUP);
  pinMode(pinoSpray, OUTPUT);
  // pinMode(pinoCooler, OUTPUT);
  pinMode(pinoStatus, OUTPUT);
  pinMode(pinButton, INPUT);
}

void spayOnButton() {
  Serial.println("Acionamento manual spray ON\n");
  digitalWrite(pinoSpray, LOW);
}
void spayOffButton() {
  Serial.println("Acionamento manual spray OFF\n");
  digitalWrite(pinoSpray, HIGH);
}

void sprayOn() {
  isSprayOn = true;
  Serial.println("Liga spray\n");
  digitalWrite(pinoSpray, LOW);
}
void sprayOff() {
  digitalWrite(pinoSpray, HIGH);
  Serial.println("Desliga spray\n");
  isSprayOn = false;
}

void coolerOn() {
  Serial.println("Liga cooler\n");
  digitalWrite(pinoCooler, HIGH);
  isCoolerOn = true;
}
void coolerOff() {
  digitalWrite(pinoCooler, LOW);
  Serial.println("Desliga cooler\n");
  isCoolerOn = false;
}


void releTimer() {
  s1state = digitalRead(S1);
  s2state = digitalRead(S2);
  s3state = digitalRead(S3);
  s4state = digitalRead(S4);

  if (s1state == LOW) {
    timer += 300;
  }
  if (s2state == LOW) {
    timer += 600;
  }
  if (s3state == LOW) {
    timer += 1200;
  }
  if (s4state == LOW) {
    timer += 1800;
  }
}

void getCoolerTime() {
  coolerTime = (timer * percentualCooler) / 100;
}
void ledStatus() {
  delay(2000);
  digitalWrite(pinoStatus, HIGH);
  delay(2000);
  digitalWrite(pinoStatus, LOW);
}
void loop() {
  unsigned long currentTime = millis() / 1000; //em segundos
  releTimer();
  Serial.println("Tempo    :" + String(currentTime));
  Serial.println("Progr    :" + String(timer));
  
  //Caputra o status do push button
  statusButton = digitalRead(pinButton);
  
  if (statusButton == HIGH) { //Se o botão estiver pressionao
    spayOnButton();
    acionaManual = true;
  } else if(acionaManual) { // Se não estiver pressionado zera o tempo para começar tudo denovo e desliga o spray
    spayOffButton(); //<--Desliga o spray
    acionaManual = false; //<--Acionamento manual false
    currentTime = 0; //<--Tempo zerado 
  }
  // se acionaManual for falso
  if (!acionaManual) {
    if (timer > 0) {
      if (currentTime - timeToOnSpray >= timer && !isSprayOn) {
        timeToOnSpray = currentTime;
        sprayOn();
        timeToOffSpray = currentTime + sprayTime;
      }

      if ( currentTime >=  timeToOffSpray && isSprayOn) {
        timeToOffSpray = 0;
        sprayOff();
        /* getCoolerTime();
          coolerOn();
          timeToOffCooler = currentTime + coolerTime;
        */
      }

      /*
        if ( currentTime >=  timeToOffCooler && isCoolerOn) {
        timeToOffCooler = 0;
        coolerOff();
        }
      */
      Serial.println("Spray On :" + String(timeToOnSpray));
      Serial.println("Spray Of :" + String(timeToOffSpray));
      // Serial.println("Coolr Of :" + String(timeToOffCooler));
      // Serial.println("Coolr Ti :" + String(coolerTime));

    }
    if (currentTime - timeToOnLed >= ledTimer && !isLedOn) {
      timeToOnLed = currentTime;
      digitalWrite(pinoStatus, HIGH);
      timeToOffLed = currentTime + ledTimer;
      isLedOn = true;
    }

    if ( currentTime >= timeToOffLed && isLedOn ) {
      digitalWrite(pinoStatus, LOW);
      timeToOffLed = 0;
      isLedOn = false;
    }
    timer = 0;
    delay(1000);
  }
}
