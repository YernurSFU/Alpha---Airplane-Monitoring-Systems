// Reference: https://www.kernel.org/doc/Documentation/watchdog/watchdog-api.txt

#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>

#include "System.h"
#include "Utils.h"

#define WATCHDOG_FILE_PATH "/dev/watchdog"

static int watchdogFileDesc = -1;
static _Bool doneInit = false;
static pthread_mutex_t libLock = PTHREAD_MUTEX_INITIALIZER;

void WatchdogHitter_start()
{
    pthread_mutex_lock(&libLock);
    watchdogFileDesc = open(WATCHDOG_FILE_PATH, O_WRONLY);
    if (watchdogFileDesc == -1) {
        System_exitError("Failed to open watchdog file");
    }
    doneInit = true;
    pthread_mutex_unlock(&libLock);
}

void WatchdogHitter_hit()
{
    pthread_mutex_lock(&libLock);
    assert(doneInit);
    ioctl(watchdogFileDesc, WDIOC_KEEPALIVE, 0);
    pthread_mutex_unlock(&libLock);
}

int WatchdogHitter_getTimeout()
{
    pthread_mutex_lock(&libLock);
    assert(doneInit);
    int timeout = -1;
    ioctl(watchdogFileDesc, WDIOC_GETTIMEOUT, &timeout);
    return timeout;
    pthread_mutex_unlock(&libLock);
}

// FIXME: In general, will need to retrieve watchdog options via ioctl to determine
//        if this operation is possible, but we know it can be done for BBG
void WatchdogHitter_setTimeout(int newTimeout)
{
    pthread_mutex_lock(&libLock);
    assert(doneInit && newTimeout > 0);
    ioctl(watchdogFileDesc, WDIOC_SETTIMEOUT, &newTimeout);
    pthread_mutex_unlock(&libLock);
}