#ifndef POSITION_TRACKER_H
#define POSITION_TRACKER_H

void PositionTracker_init();
unsigned int PositionTracker_getCurrentMarkerNum();
double PositionTracker_getCurrentHeight();
void PositionTracker_cleanup();

#endif