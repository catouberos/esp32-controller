#include "pid.hpp"

#include <Arduino.h>
#include <PID_v1.h>

#include "esp32-hal-gpio.h"
#include "esp32-hal-ledc.h"

#define MOT_TL_A 25
#define MOT_TL_B 26
#define MOT_TR_A 27
#define MOT_TR_B 14
#define MOT_BL_A 12
#define MOT_BL_B 13
#define MOT_BR_A 32
#define MOT_BR_B 33

#define MOT_TL_CHANNEL 0
#define MOT_TR_CHANNEL 1
#define MOT_BL_CHANNEL 2
#define MOT_BR_CHANNEL 3

#define PWM_FREQ 10000
#define PWM_RES 8

#define ENC_TL_A 34
#define ENC_TL_B 35
#define ENC_TR_A 21
#define ENC_TR_B 22
#define ENC_BL_A 4
#define ENC_BL_B 16
#define ENC_BR_A 5
#define ENC_BR_B 17
#define ENC_RES 330

double mot_tl_cmd = 0, mot_tr_cmd = 0, mot_bl_cmd = 0, mot_br_cmd = 0;

volatile long cnt_tl = 0, cnt_tr = 0, cnt_bl = 0, cnt_br = 0;

double speed_tl = 0, speed_tr = 0, speed_bl = 0, speed_br = 0;

long prev_cnt_tl = 0, prev_cnt_tr = 0, prev_cnt_bl = 0, prev_cnt_br = 0;
unsigned long last_time_tl = 0, last_time_tr = 0, last_time_bl = 0,
              last_time_br = 0;

double kp = 2.1, ki = 60, kd = 0;

// input, output, ref
PID MOT_TL_PID(&speed_tl, &mot_tl_cmd, &speed_tl_ref, kp, ki, kd, DIRECT);
PID MOT_TR_PID(&speed_tr, &mot_tr_cmd, &speed_tr_ref, kp, ki, kd, DIRECT);
PID MOT_BL_PID(&speed_bl, &mot_bl_cmd, &speed_bl_ref, kp, ki, kd, DIRECT);
PID MOT_BR_PID(&speed_br, &mot_br_cmd, &speed_br_ref, kp, ki, kd, DIRECT);

void init_motors() {
  pinMode(MOT_TL_A, OUTPUT);
  pinMode(MOT_TL_B, OUTPUT);
  pinMode(MOT_TR_A, OUTPUT);
  pinMode(MOT_TR_B, OUTPUT);
  pinMode(MOT_BL_A, OUTPUT);
  pinMode(MOT_BL_B, OUTPUT);
  pinMode(MOT_BR_A, OUTPUT);
  pinMode(MOT_BR_B, OUTPUT);

  ledcSetup(MOT_TL_CHANNEL, PWM_FREQ, PWM_RES);
  ledcSetup(MOT_TR_CHANNEL, PWM_FREQ, PWM_RES);
  ledcSetup(MOT_BL_CHANNEL, PWM_FREQ, PWM_RES);
  ledcSetup(MOT_BR_CHANNEL, PWM_FREQ, PWM_RES);
}

void send_pwm(int pin_a, int pin_b, double mot_cmd, int channel) {
  if (mot_cmd < 0) {
    ledcAttachPin(pin_b, channel);
    ledcDetachPin(pin_a);
    digitalWrite(pin_a, LOW);
    ledcWrite(channel, abs((int)mot_cmd));
  } else {
    ledcAttachPin(pin_a, channel);
    ledcDetachPin(pin_b);
    digitalWrite(pin_b, LOW);
    ledcWrite(channel, abs((int)mot_cmd));
  }
}

void run_motors() {
  send_pwm(MOT_TL_A, MOT_TL_B, mot_tl_cmd, MOT_TL_CHANNEL);
  send_pwm(MOT_TR_A, MOT_TR_B, mot_tr_cmd, MOT_TR_CHANNEL);
  send_pwm(MOT_BL_A, MOT_BL_B, mot_bl_cmd, MOT_BL_CHANNEL);
  send_pwm(MOT_BR_A, MOT_BR_B, mot_br_cmd, MOT_BR_CHANNEL);
}

void read_tl_encoder() {
  int b = digitalRead(ENC_TL_B);
  cnt_tl = (b > 0) ? (cnt_tl + 1) : (cnt_tl - 1);
}

void read_tr_encoder() {
  int b = digitalRead(ENC_TR_B);
  cnt_tr = (b > 0) ? (cnt_tr + 1) : (cnt_tr - 1);
}

void read_bl_encoder() {
  int b = digitalRead(ENC_BL_B);
  cnt_bl = (b > 0) ? (cnt_bl + 1) : (cnt_bl - 1);
}

void read_br_encoder() {
  int b = digitalRead(ENC_BR_B);
  cnt_br = (b > 0) ? (cnt_br + 1) : (cnt_br - 1);
}

void init_encoders() {
  pinMode(ENC_TL_A, INPUT);
  pinMode(ENC_TL_B, INPUT);
  pinMode(ENC_TR_A, INPUT);
  pinMode(ENC_TR_B, INPUT);
  pinMode(ENC_BL_A, INPUT);
  pinMode(ENC_BL_B, INPUT);
  pinMode(ENC_BR_A, INPUT);
  pinMode(ENC_BR_B, INPUT);

  attachInterrupt(digitalPinToInterrupt(ENC_TL_A), read_tl_encoder, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_TR_A), read_tr_encoder, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_BL_A), read_bl_encoder, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC_BR_A), read_br_encoder, RISING);
}

void update_speeds() {
  unsigned long now = millis();
  double dt;

  dt = (now - last_time_tl) / 1000.0;
  if (dt > 0) {
    long delta_tl = cnt_tl - prev_cnt_tl;
    speed_tl = (delta_tl * 60.0) / (ENC_RES * dt);
    prev_cnt_tl = cnt_tl;
    last_time_tl = now;
  }

  dt = (now - last_time_tr) / 1000.0;
  if (dt > 0) {
    long delta_tr = cnt_tr - prev_cnt_tr;
    speed_tr = (delta_tr * 60.0) / (ENC_RES * dt);
    prev_cnt_tr = cnt_tr;
    last_time_tr = now;
  }

  dt = (now - last_time_bl) / 1000.0;
  if (dt > 0) {
    long delta_bl = cnt_bl - prev_cnt_bl;
    speed_bl = (delta_bl * 60.0) / (ENC_RES * dt);
    prev_cnt_bl = cnt_bl;
    last_time_bl = now;
  }

  dt = (now - last_time_br) / 1000.0;
  if (dt > 0) {
    long delta_br = cnt_br - prev_cnt_br;
    speed_br = (delta_br * 60.0) / (ENC_RES * dt);  // Convert to RPM
    prev_cnt_br = cnt_br;
    last_time_br = now;
  }
}

void init_pid() {
  MOT_TL_PID.SetMode(AUTOMATIC);
  MOT_TL_PID.SetOutputLimits(-255, 255);
  MOT_TL_PID.SetSampleTime(10);

  MOT_TR_PID.SetMode(AUTOMATIC);
  MOT_TR_PID.SetOutputLimits(-255, 255);
  MOT_TR_PID.SetSampleTime(10);

  MOT_BL_PID.SetMode(AUTOMATIC);
  MOT_BL_PID.SetOutputLimits(-255, 255);
  MOT_BL_PID.SetSampleTime(10);

  MOT_BR_PID.SetMode(AUTOMATIC);
  MOT_BR_PID.SetOutputLimits(-255, 255);
  MOT_BR_PID.SetSampleTime(10);
}

void compute_pid() {
  MOT_TL_PID.SetTunings(kp, ki, kd);
  MOT_TL_PID.Compute();
  MOT_TR_PID.SetTunings(kp, ki, kd);
  MOT_TR_PID.Compute();
  MOT_BL_PID.SetTunings(kp, ki, kd);
  MOT_BL_PID.Compute();
  MOT_BR_PID.SetTunings(kp, ki, kd);
  MOT_BR_PID.Compute();
}
