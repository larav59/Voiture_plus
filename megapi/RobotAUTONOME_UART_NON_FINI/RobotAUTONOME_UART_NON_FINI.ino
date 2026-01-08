//Ce code permet au robot de se déplacer en prenant comme commande une potition X,Y et une vitesse. 

// ATTENTION, le gyroscope n'étant pas lié à la map marvelmind, il faut toujours positionner le tobot dans un certain angle à chaque setup (branchement à la carte arduinio, téléversement, et débranchement à la carte arduino).
// ATTENTION, ce programe n'est pas fonctionel! le robot ne se déplace pas dans la bonne direction : le coupable : nous n'avons pas eu le temps de créer une fonction qui setup le gyroscope à 0 au setup fonctionel (mais il n'y a plus de dérive observée)

#include "MeMegaPi.h"
#include <Wire.h>
#include <TimerFive.h>
#include <stdint.h>

// --- CONFIGURATION ---
#define BAUDRATE 9600         // Doit être PAREIL sur le Raspberry Pi
#define UART_START_BYTE 0x24


#define BELT_PITCH 2.032 // distance between teeth
#define NTEETH 90.0 // number of teeth 

#define CMPS12_ADDRESS 0x60  // (0xC0 >> 1) en 7 bits I2C


#define RPM_2_MMS BELT_PITCH*NTEETH/60.0 // conversion of the speed from rpm to mm/s

#define VOLTS_2_PWM 255.0/12.0 // conversion voltage to pwm [-255 255]
#define MAX_VOLTAGE 9 //  

#define Ts 5000 // Sampling period in µs

// à vérifier sur le robot donc pas de valeurs par défaut 
#define Temps_DT 1000 // temps que le robot met pour faire un demi-tour à la tension T_DT
#define T_DT 9 // tension avec laquelle le robot fait un demi tour en un temps Temps_DT
#define RPM_MAX 159.15 // vitesse de rotation maximal de la roue du robot 
#define MAX_SPEED 500 // vitesse maximale du robot en mm/s
#define lcm 105 // Entraxe/2 du modèle (en mm), à adapter selon le modèle
#define Lp 150.0f  //Distance entre le centre et le milieu du robot en mm20
#define Tx 0.5f
#define Ty 0.5f

enum MsgType {
    MSG_CMD_SET_POSITION = 0x10,
    MSG_CMD_SET_SPEED    = 0x11,
    MSG_CMD_STOP         = 0x12,
    MSG_TELEMETRY_POS    = 0x20
};

typedef struct __attribute__((packed)) {  // le compilateur ne fait pas d'alignement
    int16_t x;
    int16_t y;
    int16_t angle;
} PositionData_t;

typedef struct __attribute__((packed)) {
    int16_t speed;
} SpeedData_t;

typedef enum {
  STATE_WAIT_START,
  STATE_WAIT_LEN,
  STATE_WAIT_DATA
} RxState;

RxState currentState = STATE_WAIT_START;
uint8_t currentLen = 0;
uint8_t rxBuffer[256]; // DANS LE DOUTE
uint8_t rxIndex = 0;

PositionData_t pos_data = {0};
SpeedData_t speed_data = {0};
unsigned long lastTime = {0};

float L0_1 = 6887.1; //Valeur de la première valeur de L0_
float L0_2 = 0197.0; //Valeur de la seconde valeur de L0_
float L0_3 = 0037.7; //Valeur de la troisième valeur de L0_

float Kcv = 0.03; 
float Kci = -0.9455;
float Kaw = 91.5455;

float angle0 = 0;     // angle initial par rapport au nord
bool angleInitialized = false;

float v1 = -30.3030; // valeur de -1/tau
float v2 = 1.5152e+03; //valeur de k/tau
float Te = Ts*0.000001; // valeure de Ts en seconde

float angle_offset = 0.0f;
bool angle_initialized = false;

//chenille1 actuelle 
float v_k_1 = 0; // vitesse actuelle
float t_k_1 = 0; // position actuelle
float p_k_1 = 0; // perturbation actuelle
float xi_1 = 0;

//chenille2  actuelle
float v_k_2 = 0; // vitesse actuelle
float t_k_2 = 0; // position actuelle
float p_k_2 = 0; // perturbation actuelle
float xi_2 = 0; 

volatile float angle = 0;  //actual heading in deg
volatile float speed1 = 0; //actual speed in mm/s
volatile float speed2 = 0;
volatile float position1 = 0; //actual position in mm
volatile float position2 = 0;


volatile float ref1 = 0; //actual references from serial
volatile float ref2 = 0;
volatile float ref3 = 0; //vitesse mise par l'utilisateur
float u1 = 0; // control signals in V
float u2 = 0;
float u1_sat = 0;
float u2_sat = 0;
float u_1 = 0;
float u_2 = 0;

float vx_ref = 0;
float vy_ref = 0;
float x = 0.0f;
float y = 0.0f;
volatile long compTime = 0; // actual computation time of the critical loop
volatile short overrun = 0;
bool initialAngleSet = false;

MeGyro gyro; // gyroscope object instanciation
MeEncoderOnBoard Encoder_1(SLOT1); // motor with encoder object instanciation
MeEncoderOnBoard Encoder_2(SLOT2);

void processSerial2() {
  while (Serial2.available() > 0) {
    uint8_t byte = Serial2.read();
    //Serial.print("test");
    //Serial.print(byte);

    switch (currentState) {
      case STATE_WAIT_START:
        if (byte == UART_START_BYTE) {
          currentState = STATE_WAIT_LEN;
          //Serial.println(currentState);
        }
        break;

      case STATE_WAIT_LEN:
          currentLen = byte; 
          rxIndex = 0;
          if (currentLen > 0) {
            currentState = STATE_WAIT_DATA;
            //Serial.println(currentState);
          } else {
            currentState = STATE_WAIT_START;
            //Serial.println(currentState);
          }
        break;

      case STATE_WAIT_DATA:
        rxBuffer[rxIndex++] = byte;
        if (rxIndex >= currentLen) {
          parsePacket(rxBuffer, currentLen);
          currentState = STATE_WAIT_START;
          //Serial.println(currentState);
        }
        break;
    }
  }
}

void parsePacket(uint8_t* buffer, uint8_t len) {
  if (len < 1) return;

  uint8_t msgId = buffer[0];
  uint8_t *data = &buffer[1];
  uint8_t dataLen = len - 1;

  switch (msgId) {
    case MSG_TELEMETRY_POS:
      if (dataLen == sizeof(PositionData_t)) {
        PositionData_t *pos = (PositionData_t*)data;
        pos_data = *pos;
        //Serial.println(pos->angle);
        //if (!initialAngleSet) {
        //  angle = pos_data.angle/100.00;
        //  initialAngleSet = true;
        //}
      }
      break;

    case MSG_CMD_SET_POSITION:
      if (dataLen == sizeof(PositionData_t)) {
        PositionData_t *pos = (PositionData_t*)data;
        //ref1 = pos->x; 
        //ref2 = pos->y;
      }
      break;

    case MSG_CMD_SET_SPEED:
      if (dataLen == sizeof(SpeedData_t)) {
        SpeedData_t *spd = (SpeedData_t*)data;
        speed_data = *spd;
        //ref3 = spd->speed;
      }
      break;

    case MSG_CMD_STOP:
      ref3 = 0;
      break;

    default:
      break;
  }
}


/*
 * routine that is called every 5ms by the ISR routine
 */

void Update5ms()
{
  UpdateSensors();
  UpdateControl();
  UpdateActuators();
}

void UpdateSensors(){

  Wire.beginTransmission(CMPS12_ADDRESS);
  Wire.write(0x02); // Angle high byte
  Wire.endTransmission();

  Wire.requestFrom(CMPS12_ADDRESS, 2);
  uint16_t angle_raw = (Wire.read() << 8) | Wire.read();
  float angle_deg = angle_raw / 10.0f; // 0–360°

  // Initialisation de l'offset (une seule fois)
  if (!angle_initialized) {
    angle_offset = angle_deg;
    angle_initialized = true;
  }

  // Angle ramené à 0 au démarrage
  angle = (angle_deg - angle_offset);

  // Remise dans [-180 ; +180] (OPTIONNEL mais RECOMMANDÉ)
  //if (angle > 180.0f) angle -= 360.0f;
  //if (angle < -180.0f) angle += 360.0f;

  // Normalisation [0 ; 360[
  while (angle < 0.0f)   angle += 360.0f;
  while (angle >= 360.0f) angle -= 360.0f;

  Encoder_1.loop();
  Encoder_2.loop();

  speed1 = Encoder_1.getCurrentSpeed()*RPM_2_MMS;
  speed2 = Encoder_2.getCurrentSpeed()*RPM_2_MMS;

  position1 = Encoder_1.getCurPos()*BELT_PITCH*NTEETH/360.0f;
  position2 = Encoder_2.getCurPos()*BELT_PITCH*NTEETH/360.0f;
}

void UpdateControl()
{
  float v_user = ref3; 
  v_user = constrain(v_user, 0.0, MAX_VOLTAGE);
    
  // Compteur statique pour la correction GPS/RPi (tous les 200 cycles * 5ms = 1s)
  static int gps_update_counter = 0; 
  const int UPDATE_INTERVAL_CYCLES = 200; // 1000 ms / 5 ms = 200

  float x_ref = ref1;
  float y_ref = ref2;

  // --- LOGIQUE DE DÉPLACEMENT ET D'ARRÊT ---

  float dx = x_ref - x;
  float dy = y_ref - y;
  float dist = sqrt(dx*dx + dy*dy);

  float STOP_RADIUS_MM = 50.0f; // valeur de distance de non bouger

  // éviter division par zéro
  if(dist <= STOP_RADIUS_MM) {
      vx_ref = 0;
      vy_ref = 0;
      // La correction GPS n'est PLUS effectuée ici.
  }
  else {
      // vitesse utilisateur v_user (en V) convertie en mm/s
      float v_user_mm = (v_user / MAX_VOLTAGE) * MAX_SPEED;
      // direction vers la cible
      float ux = dx / dist;
      float uy = dy / dist;

      // vitesse cible en X/Y
      vx_ref = ux * v_user_mm;
      vy_ref = uy * v_user_mm;
  }

  // Le modèle continue de tourner à chaque cycle (5 ms), qu'il bouge ou non.
  x += vx_ref * Te;
  y += vy_ref * Te;

  gps_update_counter++;
  if (gps_update_counter >= UPDATE_INTERVAL_CYCLES) {
      x = (float)pos_data.x;
      y = (float)pos_data.y; 
      gps_update_counter = 0;
  }


  float angle_rad = angle*3.14/180;
  // ... Reste de votre code (kinématique, observateur, contrôle) ...
  // Le reste est inchangé car il utilise vx_ref et vy_ref, 
  // qui sont correctement calculés par le bloc if/else de navigation.
    
  float v_ref = cos(angle_rad) * vx_ref + sin(angle_rad) * vy_ref;
  
  //float t_ref_deg = ;

  float t_ref_rad = (1/Lp) * (-sin(angle_rad) * vx_ref + cos(angle_rad) * vy_ref);

  float v_ref1 = -(v_ref - lcm * t_ref_rad);
  float v_ref2 = v_ref + lcm * t_ref_rad;

  float v_ref1_sat = constrain(v_ref1,-MAX_SPEED ,MAX_SPEED);
  float v_ref2_sat = constrain(v_ref2,-MAX_SPEED, MAX_SPEED);

  //discrétisation et observateur chenille 1
  float v_k1_1 = (v1 * v_k_1 + v2 * p_k_1 + v2 * u1_sat + L0_1 * (position1 - t_k_1)) * Te + v_k_1;      //vitesse à k+1
  float t_k1_1 = (v_k_1 + L0_2 * (position1 - t_k_1)) * Te + t_k_1;                                // position à k+1
  float p_k1_1 = (L0_3 * (position1 - t_k_1)) * Te + p_k_1;                                       // perturbation à k+1


  float err1 = v_ref1_sat - v_k_1 + Kaw * (u1_sat-u_1); // en vitesse
  float xi1_1 = xi_1 + err1 * Te;
  xi_1 = xi1_1;
  u_1 = (-Kci * xi_1 - Kcv * v_k_1);
  u1_sat = constrain(u_1,-MAX_VOLTAGE ,MAX_VOLTAGE);
  u1 = u1_sat;


  v_k_1 = v_k1_1; // vitesse à k
  t_k_1 = t_k1_1; // position à k
  p_k_1 = p_k1_1; // perturbation à k


    //discrétisation et observateur chenille 2
  float v_k1_2 = (v1 * v_k_2 + v2 * p_k_2 + v2 * u2_sat + L0_1 * (position2 - t_k_2)) * Te + v_k_2;      //vitesse à k+1
  float t_k1_2 = (v_k_2 + L0_2 * (position2 - t_k_2)) * Te + t_k_2;                                // position à k+1
  float p_k1_2 = (L0_3 * (position2 - t_k_2)) * Te + p_k_2;                                       // perturbation à k+1


  float err2 = v_ref2_sat - v_k_2 + Kaw * (u2_sat-u_2); // en vitesse
  float xi1_2 = xi_2 + err2 * Te;
  xi_2 = xi1_2;
  u_2 = (-Kci * xi_2 - Kcv * v_k_2);
  u2_sat = constrain(u_2,-MAX_VOLTAGE,MAX_VOLTAGE);
  u2 = u2_sat;

    // Arrêt des moteurs si la tension demandée est trop faible (< 2V)
  if (fabs(u1) < 2.0 && fabs(u2) < 2.0) {
    u1 = 0;
    u2 = 0;
  }

  v_k_2 = v_k1_2; // vitesse à k
  t_k_2 = t_k1_2; // position à k
  p_k_2 = p_k1_2; // perturbation à k
}


void UpdateActuators(){
  setMotorsVoltage(u1,u2); // set the voltages
}
/*
* routine that is called every 5ms by the timer 5
*/
void Timer5ISR(){
  static char executing = 0; // set to 1 when the update function is running
  if(executing) {
    overrun = 1;
    return;
  }
  else executing = 1; // if already running => overrunned

  interrupts(); // enable the interrupts during the execution of this loop
  long startTime = micros();
  
  Update5ms();
  
  compTime = micros()-startTime;
  executing = 0;
}

/*
 * interruption routine for encoder pulse counting (quadrature)
 */

void isr_process_encoder1(void)
{
  if(digitalRead(Encoder_1.getPortB()) == 0)
  {
    Encoder_1.pulsePosMinus();
  }
  else
  {
    Encoder_1.pulsePosPlus();
  }
}

void isr_process_encoder2(void)
{
  if(digitalRead(Encoder_2.getPortB()) == 0)
  {
    Encoder_2.pulsePosMinus();
  }
  else
  {
    Encoder_2.pulsePosPlus();
  }
}

/*
 * Setup the motors with the reduction ratio and number of pulses per turn
 */

void setupMotors(){
  Encoder_1.setPulse(8);
  Encoder_1.setRatio(46);
  Encoder_2.setPulse(8);
  Encoder_2.setRatio(46);
  
  attachInterrupt(Encoder_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Encoder_2.getIntNum(), isr_process_encoder2, RISING);
  //Set PWM 8KHz
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);

  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);
}


/*
 * Sets the voltage to the motor. Limited by MAX_VOLTAGE
 */
void setMotorsVoltage(float voltage1,float voltage2){
  Encoder_1.setMotorPwm(constrain(voltage1,-MAX_VOLTAGE,MAX_VOLTAGE)*VOLTS_2_PWM);
  Encoder_2.setMotorPwm(constrain(voltage2,-MAX_VOLTAGE,MAX_VOLTAGE)*VOLTS_2_PWM);
}



void setup()
{
   // 1. Initialisation LED (Flash quand on reçoit)
  pinMode(13, OUTPUT); 
  digitalWrite(13, LOW);


  // 2. Serial USB (Pour voir les infos sur ton PC)
  Serial.begin(BAUDRATE);
  Serial.setTimeout(1);
  Serial2.begin(BAUDRATE); 
  Serial2.setTimeout(50); 
  gyro.begin();
  Wire.setClock(400000);
  setupMotors();
  Timer5.initialize(Ts);
  Timer5.attachInterrupt(Timer5ISR); 

  // Initialisation de l'angle
  //angle = pos_data.angle/100;
  // --- INITIALISATION DES DONNÉES GPS ---        
  // Initialisation de la position x et y avec les données GPS reçues
   x = (float)pos_data.x;
   y = (float)pos_data.y;

  }


void loop()
{
  static float lastRef1 = 0;
  static float lastRef2 = 0;
  static float lastRef3 = 0;
  noInterrupts();
  processSerial2();
  ref1 = lastRef1;
  ref2 = lastRef2;
  ref3 = lastRef3;

  float angleCopy = angle;
  float speed1Copy = speed1;
  float speed2Copy = speed2;
  float position1Copy = position1;
  float position2Copy = position2;
  float x_poss = x;
  float y_poss = y;
  float speed1_obsCopy = v_k_1;
  float speed2_obsCopy = v_k_2;
  float position1_obsCopy = t_k_1;
  float position2_obsCopy = t_k_2;
  float perturbation1_obsCopy = p_k_1;
  float perturbation2_obsCopy = p_k_2;
  float moteur_1 = u1;
  float moteur_2 = u2;
  long compTimeCopy = compTime;

  interrupts();
  
  //Serial.read();

  Serial.print("X_cmd: ");
  Serial.print(ref1);


  Serial.print("  Y_cmd: ");
  Serial.print(ref2);

  Serial.print("  V_cmd: ");
  Serial.print(ref3);
  
  Serial.print("       X_sur: ");
  Serial.print(pos_data.x);

  Serial.print(" Y_sur: ");
  Serial.print(pos_data.y);


  Serial.print(" angle_sur: ");
  Serial.println(angle);

  //Serial.print("       X_poss: ");
  //Serial.print(x_poss,2);

  //Serial.print(" Y_poss: ");
  //Serial.print(y_poss,2);

  //Serial.print(" angle: ");
  //Serial.print(angleCopy,2);
  

  //Serial.print(" Moteur 1: ");
  //Serial.print(moteur_1,2);

  //Serial.print(" Moteur 2: ");
  //Serial.print(moteur_2,2);

  //Serial.print(" overrun: ");
  //Serial.print(overrun);
  
//  Serial.print(" speed1: ");
//  Serial.print(speed1Copy,2);

  //Serial.print(" speed1_obs: ");
  //Serial.print(speed1_obsCopy,2);
  
  // Serial.print(" speed2: ");
  // Serial.print(speed2Copy,2);

  //Serial.print(" speed2_obs: ");
  //Serial.print(speed2_obsCopy,2);
  
  // Serial.print(" position1: ");
  // Serial.print(position1Copy,2);

  //Serial.print(" position1_obs: ");
  //Serial.print(position1_obsCopy,2);
  
  // Serial.print(" position2: ");
  // Serial.print(position2Copy,2);

  //Serial.print(" position2_obs: ");
  //Serial.print(position2_obsCopy,2);

  //Serial.print(" pertubation1_obs: ");
  //Serial.print(perturbation1_obsCopy,2);

  //Serial.print(" perturbation2_obs: ");
  //Serial.print(perturbation2_obsCopy,2);
  
  // Serial.print(" v-ref1: ");
  // Serial.print(xi_1,2);

  //   Serial.print(" v-ref2: ");
  // Serial.print(xi_2,2);
  if(Serial.available()){
    lastRef1 = Serial.parseFloat();
    lastRef2 = Serial.parseFloat();
    lastRef3 = Serial.parseFloat();
  }

  delay(10);

  
}
