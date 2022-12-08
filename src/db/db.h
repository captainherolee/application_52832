#ifndef __DB_H__
#define __DB_H__

#define FILE_ID 0x1111
#define REC_KEY 0x2222

typedef struct {
    int32_t temp1;
    int32_t temp2;
    int32_t temp3;
    int32_t temp4;
} db_save_t;

bool db_get_flash_busy(void);

void db_set_config(db_save_t *config);
uint32_t db_load(uint32_t* p_buff);
void db_update(void);
void db_backup(void);
void db_save(void);
void db_init(void);

#endif /* __DB_H__ */
