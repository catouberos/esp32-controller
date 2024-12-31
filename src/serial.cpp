#include <Arduino.h>

extern double kp, ki, kd;

void serial_print_write() {
  if (Serial.available()) {
    char cmd = Serial.read();  // Read a single character command

    switch (cmd) {
      case 'p':  // Increase Kp
        kp += 0.1;
        Serial.println("Kp increased");
        break;

      case 'P':                   // Decrease Kp
        kp = max(0.0, kp - 0.1);  // Ensure Kp doesn't go below 0
        Serial.println("Kp decreased");
        break;

      case 'i':  // Increase Ki
        ki += 0.01;
        Serial.println("Ki increased");
        break;

      case 'I':                    // Decrease Ki
        ki = max(0.0, ki - 0.01);  // Ensure Ki doesn't go below 0
        Serial.println("Ki decreased");
        break;

      case 'd':  // Increase Kd
        kd += 0.01;
        Serial.println("Kd increased");
        break;

      case 'D':                    // Decrease Kd
        kd = max(0.0, kd - 0.01);  // Ensure Kd doesn't go below 0
        Serial.println("Kd decreased");
        break;

      case 's':  // Show current values
        Serial.print("Current PID values: ");
        Serial.print("Kp = ");
        Serial.print(kp);
        Serial.print(", Ki = ");
        Serial.print(ki);
        Serial.print(", Kd = ");
        Serial.println(kd);
        break;

      default:
        Serial.println("Unknown command");
        break;
    }
  }

  delay(10);  // Small delay to avoid overloading the serial communication
}
