#ifndef __POWER_MGMT_H__
#define __POWER_MGMT_H__

void device_power_mgmt_init_reset_mode_status(void);
RESET_MODE device_power_mgmt_get_static_reset_mode_status(void);
void device_power_mgmt_set_reset_mode_status(uint32_t status);
void device_power_mgmt_init_reset_cutoff_status(void);
void device_power_mgmt_get_reset_cutoff_status(uint32_t *p_status);
void device_power_mgmt_set_reset_cutoff_status(uint32_t status);
void device_power_mgmt_enter_very_low_power(void);
void device_power_mgmt_set_is_sleepable(bool attribute);
bool device_power_mgmt_get_is_sleepable(void);


#endif /* __POWER_MGMT_H__ */
