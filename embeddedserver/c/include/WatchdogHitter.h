#ifndef WATCHDOG_HITTER_H
#define WATCHDOG_HITTER_H

void WatchdogHitter_start();
void WatchdogHitter_hit();
int WatchdogHitter_getTimeout();
void WatchdogHitter_setTimeout(int newTimeout);

#endif