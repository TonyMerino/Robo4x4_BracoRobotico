//Biblioteca para controlar servos
#include <ESP32Servo.h>
//Biblioteca do controle de PS4
#include <PS4Controller.h>

//Define o MAC Address do controle de PS4
#define MAC_ADDRESS "64:64:64:64:64:64"

//Define os pinos para a locomocão com a Ponte H (TB6612NG)
#define PWMA 32
#define PWMB 14
#define A1 25
#define A2 33
#define B1 26
#define B2 27
//Define os pinos para os servos da garra
#define S1 18
#define S2 19
#define S3 12//21
#define S4 13//15

//Define parâmetros para a garra (ajustar valores para posição inicial)
//Motor da base
int posicao1 = 85;
Servo servo1;
//Olhando por trás o da direita
int posicao2 = 50;
Servo servo2;
//Olhando por trás o da direita
int posicao3 = 100;
Servo servo3;
//Motor da garra
int posicao4 = 50;
Servo servo4;

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

//Função motor A (Se o valor for positivo o motor A gira para um lado e se for negativo troca o sentido)
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

//Função motor B (Se o valor for positivo o motor B gira para um lado e se for negativo troca o sentido)
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

//Função para controlar o primeiro motor da garra (configurar limites)
void garra1(bool bt11, bool bt12) {
  if(bt11 == HIGH && posicao1 < 180) {
    posicao1 += 1;
    servo1.write(posicao1);
    delay(10);
  }
  if(bt12 == HIGH && posicao1 > 0) {
    posicao1 -= 1;
    servo1.write(posicao1);
    delay(10);
  }
  //Imprime os valores no monitor serial
  Serial.print("Garra1: ");
  Serial.print(posicao1);
  Serial.print("\t");
}

//Função para controlar o segundo motor da garra (configurar limites)
void garra2(bool bt21, bool bt22) {
  if(bt21 == HIGH && posicao2 < 110) {
    posicao2 += 1;
    servo2.write(posicao2);
    delay(10);
  }
  if(bt22 == HIGH && posicao2 > 0) {
    posicao2 -= 1;
    servo2.write(posicao2);
    delay(10);
  }
  //Imprime os valores no monitor serial
  Serial.print("Garra2: ");
  Serial.print(posicao2);
  Serial.print("\t");
}

//Função para controlar o terceiro motor da garra (configurar limites)
void garra3(bool bt31, bool bt32) {
  if(bt31 == HIGH && posicao3 < 180) {
    posicao3 += 1;
    servo3.write(posicao3);
    delay(10);
  }
  if(bt32 == HIGH && posicao3 > 0) {
    posicao3 -= 1;
    servo3.write(posicao3);
    delay(10);
  }
  //Imprime os valores no monitor serial
  Serial.print("Garra3: ");
  Serial.print(posicao3);
  Serial.print("\t");
}

//Função para controlar o quarto motor da garra (configurar limites)
void garra4(bool bt41, bool bt42) {
  if(bt41 == HIGH && posicao4 < 80) {
    posicao4 += 1;
    servo4.write(posicao4);
    delay(10);
  }
  if(bt42 == HIGH && posicao4 > 45) {
    posicao4 -= 1;
    servo4.write(posicao4);
    delay(10);
  }
  //Imprime os valores no monitor serial
  Serial.print("Garra4: ");
  Serial.print(posicao4);
  Serial.print("\t");
}

void setup(void) {
  //Define a velocidade que o monitor serial é atualizado
  Serial.begin(115200);
  while (!Serial)
    delay(10);

  //Conecta com o controle de PS4
  PS4.begin(MAC_ADDRESS);
  Serial.println("Ready.");
  
  //Define pinos e canais para o PWM
  ledcAttachPin(PWMA,5);
  ledcSetup(5, 80000, 8);
  ledcAttachPin(PWMB,6);
  ledcSetup(6, 80000, 8);
  //Define os pinos como output
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);
  
  //Deixa os pinos desligados no início
  digitalWrite(A1, 0);
  digitalWrite(A2, 0);
  digitalWrite(B1, 0);
  digitalWrite(B2, 0);

  //Configurações para os motores servo
  servo1.setPeriodHertz(50); //Frequency ideal para o servo SG90
  servo1.attach(S1, 500, 2400); //Largura de pulso máxima e mínima
  servo2.setPeriodHertz(50);
  servo2.attach(S2, 500, 2400);
  servo3.setPeriodHertz(50);
  servo3.attach(S3, 500, 2400);
  servo4.setPeriodHertz(50);
  servo4.attach(S4, 500, 2400);

  //Deixa os servos na posição inicial (Valor de 0 a 180, a definir pelo robô)
  servo1.write(posicao1);
  servo2.write(posicao2);
  servo3.write(posicao3);
  servo4.write(posicao4);
  
  while(PS4.isConnected()!= true) {
  delay(20);
  }
}

void loop() {
  while(PS4.isConnected()) {
    //motors_control(linear_speed*multiplicador, angular_speed* multiplicador2);
    //Multiplicador Linear = para aumentar a velocidade linear, o quão rápido o robô vai ser (1.8)
    //Multiplicador Angular = parâmetro que varia de 1 ate a 2.3 para suavisar as curvas em alta velocidade
    motors_control((1.8)*PS4.LStickY(),(1.3)*PS4.RStickX());
    //Controle dos motores da garra, dois botões para cada servo
    garra1(PS4.Left(),PS4.Right());
    garra2(PS4.Up(),PS4.Down());
    garra3(PS4.Square(),PS4.Circle());
    garra4(PS4.Triangle(),PS4.Cross());
  delay (10);
  }

  //Failsafe, se o controle desconectar o robô para
  if(PS4.isConnected()!= true) {
    motors_control(0, 0);
    garra1(LOW, LOW);
    garra2(LOW, LOW);
    garra3(LOW, LOW);
    garra4(LOW, LOW);
    Serial.println("Restart");
    PS4.end();
    ESP.restart();
    delay(20);
  }
}
