#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__

void watchdog_init(uint32_t time_sec);
void watchdog_update(void);

#endif /* __WATCHDOG_H__ */