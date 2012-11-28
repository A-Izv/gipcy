
#ifndef __IPCMODULE_H__
#define __IPCMODULE_H__

#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/version.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,37)
#include <linux/semaphore.h>
#endif


#ifndef __IPCIOTCL_H__
    #include "ipcioctl.h"
#endif

//-----------------------------------------------------------------------------

#define ms_to_jiffies( ms ) (HZ*ms/1000)
#define jiffies_to_ms( jf ) (jf*1000/HZ)

//-----------------------------------------------------------------------------

//! Структура описывает внутренний тип struct ipcsem_t используемый библиотекой IPC
/*!
    struct ipcsem_t - это внутренний тип данных, используемый библиотекой IPC,
    для представления объектов синхронизации типа семафор
*/
struct ipcsem_t {

    struct list_head        sem_list;                  //!< Связанный список семафоров
    char                    sem_name[128];             //!< Имя семафора
    void*                   sem_file;                  //!< Дескриптор файла устройства
    void*                   sem_handle;                //!< Адрес этой структуры
    atomic_t                sem_owner_count;           //!< Счетчик пользователей семафора
    atomic_t                sem_lock_count;            //!< Счетчик блокировок семафора
    atomic_t                sem_unlock_count;          //!< Счетчик разблокировок семафора
    struct semaphore        sem;                       //!< Семафор
};

//-----------------------------------------------------------------------------

//! Структура описывает внутренний тип struct ipcmutex_t используемый библиотекой IPC
/*!
    struct ipcmutex_t - это внутренний тип данных, используемый библиотекой IPC,
    для представления объектов синхронизации типа семафор
*/
struct ipcmutex_t {

    struct list_head        mutex_list;                  //!< Связанный список семафоров
    char                    mutex_name[128];             //!< Имя семафора
    void*                   mutex_file;                  //!< Дескриптор файла устройства
    void*                   mutex_handle;                //!< Адрес этой структуры
    atomic_t                mutex_owner_count;           //!< Счетчик пользователей семафора
    atomic_t                mutex_lock_count;            //!< Счетчик блокировок семафора
    atomic_t                mutex_unlock_count;          //!< Счетчик разблокировок семафора
    struct semaphore        mutex;                       //!< Семафор
};

//-----------------------------------------------------------------------------

//! Структура описывает внутренний тип struct ipcevent_t используемый библиотекой IPC
/*!
    struct ipcevent_t - это внутренний тип данных, используемый библиотекой IPC,
    для представления объектов синхронизации типа семафор
*/
struct ipcevent_t {

    struct list_head        event_list;                  //!< Связанный список семафоров
    char                    event_name[128];             //!< Имя семафора
    void*                   event_file;                  //!< Дескриптор файла устройства
    void*                   event_handle;                //!< Адрес этой структуры
    atomic_t                event_owner_count;           //!< Счетчик пользователей семафора
    atomic_t                event_lock_count;            //!< Счетчик блокировок семафора
    atomic_t                event_unlock_count;          //!< Счетчик разблокировок семафора
    struct semaphore        event;                       //!< Семафор
};

//-----------------------------------------------------------------------------

struct ipc_driver {

    dev_t                   m_devno;
    struct list_head        m_list;
    char                    m_name[128];
    struct class           *m_class;
    struct device          *m_ipc_device;
    struct mutex            m_ipc_mutex;
    struct semaphore        m_ipc_sem;
    spinlock_t              m_ipc_lock;
    atomic_t                m_usage;
    struct device           *m_device;

    struct list_head        m_file_list;
    struct mutex            m_file_lock;

    struct list_head        m_sem_list;
    struct mutex            m_sem_lock;

    struct list_head        m_mutex_list;
    struct mutex            m_mutex_lock;

    struct list_head        m_event_list;
    struct mutex            m_event_lock;

    struct cdev             m_cdev;
};

//-----------------------------------------------------------------------------

extern int dbg_trace;
extern int err_trace;

#ifndef PRINTK
#define PRINTK(S...) printk(S)
#endif

#define dbg_msg(flag, S...) do { if(flag) PRINTK(KERN_DEBUG"IPCDRV: " S); } while(0)
#define err_msg(flag, S...) do { if(flag) PRINTK(KERN_ERR"IPCDRV: " S); } while(0)

//-----------------------------------------------------------------------------

void* ipc_sem_create( struct ipc_driver *drv, struct ipc_create_t *sem_param );
int ipc_sem_lock( struct ipc_driver *drv, struct ipc_lock_t *sem_param );
int ipc_sem_unlock( struct ipc_driver *drv, struct ipc_unlock_t *sem_param );
int ipc_sem_close( struct ipc_driver *drv, struct ipc_close_t *sem_param );

void* ipc_mutex_create( struct ipc_driver *drv, struct ipc_create_t *sem_param );
int ipc_mutex_lock( struct ipc_driver *drv, struct ipc_lock_t *sem_param );
int ipc_mutex_unlock( struct ipc_driver *drv, struct ipc_unlock_t *sem_param );
int ipc_mutex_close( struct ipc_driver *drv, struct ipc_close_t *sem_param );

void* ipc_event_create( struct ipc_driver *drv, struct ipc_create_t *sem_param );
int ipc_event_lock( struct ipc_driver *drv, struct ipc_lock_t *sem_param );
int ipc_event_unlock( struct ipc_driver *drv, struct ipc_unlock_t *sem_param );
int ipc_event_reset( struct ipc_driver *drv, struct ipc_reset_t *sem_param );
int ipc_event_close( struct ipc_driver *drv, struct ipc_close_t *sem_param );


#endif //_PEXDEV_H_