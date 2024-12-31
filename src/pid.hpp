#ifndef PID_H
#define PID_H

void init_motors();
void init_encoders();
void init_pid();

// void get_angles();
void compute_pid();
void run_motors();
void update_speeds();

#endif
