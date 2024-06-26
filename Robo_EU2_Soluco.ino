//Biblioteca do controle de PS4
#include <PS4Controller.h>

//Define os pinos de locomocão da Ponte H (TB6612NG)
#define PWMA 32
#define PWMB 13
#define A1  25
#define A2  33
#define B1  26
#define B2  27

//Define o MAS Address do controle de PS4
#define MAC_ADDRESS "64:64:64:64:64:64"

int inv = -1; //Permite inverter a pilotagem conforme o lado do robo que esta para cima
void motors_control(int linear, int angular) {
  //Ao somar o angular com linear em cada motor conseguimos a ideia de direção do robô
  int result_R = linear - angular;
  int result_L = linear + angular;
  //Não envia valores de potencia abaixo de 15, que não são fortes o suficiente para mover o robô
  if(result_R<15 && result_R >-15) result_R=0; 
  if(result_L<15 && result_L >-15 ) result_L=0;
  //Não permite valores superiores a 255 ou inferiores a -255
  if(result_R >255 ) result_R= 254; 
  if(result_R<-255 ) result_R=-254;
  if(result_L >255 ) result_L= 254;
  if(result_L<-255 ) result_L=-254;  
  //manda para a função motor um valor de -255 a 255, o sinal significa a direção  
  motor_B(result_R);
  motor_A(result_L); 
}

void motor_A(int speedA) {
  //Se o valor for positivo gira para um lado e se for negativo troca o sentido
  if(speedA>0) {  
    digitalWrite(A1, 1);
    digitalWrite(A2, 0);
  }
  else {
    digitalWrite(A1, 0);
    digitalWrite(A2, 1);
  }
  ledcWrite(5,abs(speedA));
  Serial.print("R: ");
  Serial.println(speedA);
}

void motor_B(int speedB) {
  if(speedB>0) {
    digitalWrite(B1, 1);
    digitalWrite(B2, 0);
  }
  else {
    digitalWrite(B1, 0);
    digitalWrite(B2, 1);
  }
  ledcWrite(6,abs(speedB));
  Serial.print("L: ");
  Serial.print(speedB);
  Serial.print("\t");
}

void setup(void) {
  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  PS4.begin(MAC_ADDRESS);
  Serial.println("Ready.");
  
  ledcAttachPin(PWMA,5);
  ledcAttachPin(PWMB,6);
  
  ledcSetup(5, 80000, 8);
  ledcSetup(6, 80000, 8);
  
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);
  digitalWrite(A1, 0);
  digitalWrite(A2, 0);
  digitalWrite(B1, 0);
  digitalWrite(B2, 0);

  while(PS4.isConnected()!= true) {
  delay(20);
  }
}

void loop() {
  while(PS4.isConnected()) {
  //motors_control(linear_speed*multiplicador, angular_speed* multiplicador2);
  // Multiplicadcor = 1.8 para aumentar a velocidade linear, o quao rapido o robo vai ser
  // Multiplicadcor2 = multiplic_curva, parametro que varia de 1 ate a 2.3 para suavisar as curvas em alta velocidade
    motors_control((-1.8)*inv*PS4.LStickY(),(1.3)*PS4.RStickX());
    delay(100);
    }
    
  //Failsafe
  if(PS4.isConnected()!= true) {
    motors_control(0,0);
    Serial.println("Restart");
    PS4.end();
    ESP.restart();
    delay(20);
  }
}
