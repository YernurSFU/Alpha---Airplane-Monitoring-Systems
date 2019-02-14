// Accelerometer module for reading accelerations from the Zen cape.
#ifndef _ACCEL_DRV_H_
#define _ACCEL_DRV_H_

typedef enum {MOVING_UP, MOVING_DOWN, STATIONARY} zState_t;

void AccelDrv_init();
void AccelDrv_cleanup();
void AccelDrv_getReading(double *dx, double *dy, double *dz);
zState_t AccelDrv_getZState();

#endif
