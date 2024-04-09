#include <ESP32Servo.h>
#include <PS4Controller.h>
#define pino1 18 // Output pin used

//Define o MAC Address do controle de PS4
#define MAC_ADDRESS "64:64:64:64:64:64"

Servo servo1;

int pos = 0;

void setup() {
  //Define a velocidade que o monitor serial é atualizado
  Serial.begin(115200);
  while (!Serial)
    delay(10); //will pause Zero, Leonardo, etc until serial console opens

  //Conecta com o controle de PS4
  PS4.begin(MAC_ADDRESS);
  Serial.println("Ready.");
  
  servo1.setPeriodHertz(50); // PWM frequency for servo1
  servo1.attach(pino1, 500, 2400); // Minimum and maximum pulse width (in µs) to go from 0° to 180

  while(PS4.isConnected()!= true) {
  delay(20);
  }
}

void loop() {
  servo1.write(0);
  while(PS4.isConnected()) {
    //rotation from 0 to 180°
    if(PS4.Up() == HIGH && pos < 180){
        pos += 1;
        servo1.write(pos);
        delay(10);
    }
    // Rotation from 180° to 0
    if(PS4.Down() == HIGH && pos > 0){
        pos -= 1;
        servo1.write(pos);
        delay(10);
    }
  delay(10);
  }
  //Failsafe, se o controle desconectar o robô para
  if(PS4.isConnected()!= true) {
    Serial.println("Restart");
    PS4.end();
    ESP.restart();
    delay(20);
  }
}
