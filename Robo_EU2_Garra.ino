//Biblioteca para controlar servos
#include <ESP32Servo.h>

//Biblioteca do controle de PS4
#include <PS4Controller.h>

//Define os pinos para a locomocão com a Ponte H (TB6612NG)
#define PWMA 32
#define PWMB 13
#define A1 25
#define A2 33
#define B1 26
#define B2 27

//Define os pinos para os servos da garra
#define S1 18

//Define o MAC Address do controle de PS4
#define MAC_ADDRESS "64:64:64:64:64:64"

//Define parâmetros para a garra
int posicao1 = 0;
Servo servo1;

//Função para controlar os motores de locomoção
void motors_control(int linear, int angular) {
  //Ao somar o angular com linear em cada motor conseguimos a ideia de direção do robô
  int result_R = linear - angular;
  int result_L = linear + angular;
  //Não envia valores de potencia abaixo de 15, que não são fortes o suficiente para mover o robô
  if(result_R < 15 && result_R > -15) result_R = 0; 
  if(result_L < 15 && result_L > -15) result_L = 0;
  //Não permite valores superiores a 255 ou inferiores a -255
  if(result_R > 255) result_R = 254; 
  if(result_R < -255) result_R = -254;
  if(result_L > 255) result_L = 254;
  if(result_L < -255) result_L = -254;  
  //Manda para as funções "motor_X" um valor de -255 a 255, o sinal significa a direção  
  motor_B(result_R);
  motor_A(result_L); 
}

//Se o valor for positivo o motor A gira para um lado e se for negativo troca o sentido
void motor_A(int speedA) {
  if(speedA>0) {  
    digitalWrite(A1, 1);
    digitalWrite(A2, 0);
  }
  else {
    digitalWrite(A1, 0);
    digitalWrite(A2, 1);
  }
  ledcWrite(5,abs(speedA));
  //Imprime os valores no monitor serial
  Serial.print("R: ");
  Serial.println(speedA);
}

//Se o valor for positivo o motor B gira para um lado e se for negativo troca o sentido
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
  //Imprime os valores no monitor serial
  Serial.print("L: ");
  Serial.print(speedB);
  Serial.print("\t");
}

//Função para controlar o primeiro motor da garra
void garra1(bool bt1, bool bt2) {
  if(bt1 == HIGH && posicao1 < 180) {
    posicao1 += 1;
    servo1.write(posicao1);
    delay(10);
  }
  if(bt2 == HIGH && posicao1 > 0) {
    posicao1 -= 1;
    servo1.write(posicao1);
    delay(10);
  }
  //Imprime os valores no monitor serial
  Serial.print("Garra1: ");
  Serial.print(posicao1);
  Serial.print("\t");
}

void setup(void) {
  //Define a velocidade que o monitor serial é atualizado
  Serial.begin(115200);
  while (!Serial)
    delay(10); //will pause Zero, Leonardo, etc until serial console opens

  //Conecta com o controle de PS4
  PS4.begin(MAC_ADDRESS);
  Serial.println("Ready.");
  
  //Define pinos e canais para o PWM
  ledcAttachPin(PWMA,5);
  ledcSetup(5, 80000, 8);
  ledcAttachPin(PWMB,6);
  ledcSetup(6, 80000, 8);

  //Configurações para os motores servo
  servo1.setPeriodHertz(50); // PWM frequency for SG90
  servo1.attach(S1, 500, 2400); // Minimum and maximum pulse width (in µs) to go from 0° to 180
  
  //Define os pinos como output
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);
  pinMode(S1, OUTPUT);
  
  //Deixa os pinos desligados no início
  digitalWrite(A1, 0);
  digitalWrite(A2, 0);
  digitalWrite(B1, 0);
  digitalWrite(B2, 0);

  //Deixa os servos na posição inicial (Valor de 0 a 180, a definir pelo robô)
  servo1.write(0);
  
  while(PS4.isConnected()!= true) {
  delay(20);
  }
}

void loop() {
  while(PS4.isConnected()) {
    //motors_control(linear_speed*multiplicador, angular_speed* multiplicador2);
    //Multiplicador = 1.8 para aumentar a velocidade linear, o quão rápido o robô vai ser
    //Multiplicador2 = multiplicador da curva, parâmetro que varia de 1 ate a 2.3 para suavisar as curvas em alta velocidade
    motors_control((1.8)*PS4.LStickY(),(1.3)*PS4.RStickX());
    //Controle dos motores da garra, dois botões para cada servo
    garra1(PS4.Up(),PS4.Down());
    delay(100);
  }

  //Failsafe, se o controle desconectar o robô para
  if(PS4.isConnected()!= true) {
    motors_control(0,0);
    Serial.println("Restart");
    PS4.end();
    ESP.restart();
    delay(20);
  }
}
