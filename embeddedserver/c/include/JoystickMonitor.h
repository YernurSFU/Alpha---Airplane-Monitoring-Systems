#ifndef JOYSTICK_MONITOR_H_
#define JOYSTICK_MONITOR_H_

typedef struct {
    void (*upAction)(void);
    void (*downAction)(void);
    void (*leftAction)(void);
    void (*rightAction)(void);
    void (*centerOnAction)(void);
    void (*centerOffAction)(void);
} joystickActions_t;

void JoystickMonitor_init(joystickActions_t actions);
void JoystickMonitor_cleanup();

#endif
