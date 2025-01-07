#ifndef PID_H
#define PID_H

extern double mot_tl_cmd, mot_tr_cmd, mot_bl_cmd, mot_br_cmd;
extern double speed_tl, speed_tr, speed_bl, speed_br;
extern double speed_tl_ref, speed_tr_ref, speed_bl_ref, speed_br_ref;

void init_motors();
void init_encoders();
void init_pid();

// void get_angles();
void compute_pid();
void run_motors();
void update_speeds();

#endif
