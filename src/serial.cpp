#include <Arduino.h>

#include "pid.hpp"

extern double kp, ki, kd;

#define MANUAL_MODE 0
#define CIRCLE_MODE 1
int mode = 0;

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

      case 'c':
        mode = 1;
        Serial.println("Circle mode enabled. Ignoring WebSocket commands.");
        break;

      case 'x':
        mode = 0;
        speed_tr_ref = 0;
        speed_tl_ref = 0;
        speed_br_ref = 0;
        speed_bl_ref = 0;
        Serial.println("Circle mode disabled. Resuming WebSocket commands.");
        break;

      default:
        Serial.println("Unknown command");
        break;
    }
  }

  // clang-format off
  Serial.printf(
      "%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
      speed_tl, mot_tl_cmd, speed_tl_ref,
      speed_tr, mot_tr_cmd, speed_tr_ref,
      speed_bl, mot_bl_cmd, speed_bl_ref,
      speed_br, mot_br_cmd, speed_br_ref);
  // clang-format on

  delay(10);  // Small delay to avoid overloading the serial communication
}
