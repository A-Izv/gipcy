
#ifdef __IPC_LINUX__

#ifndef __LINIPC_H__
#include "linipc.h"
#endif
#ifndef __EVENTIPC_H__
#include "eventipc.h"
#endif
#ifndef __SYSIPC_H__
#include "sysipc.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <time.h>

//-----------------------------------------------------------------------------
//  manual = TRUE - manual reset, FALSE - autoreset
//  value = TRUE - начальное состояние Signaled
IPC_handle IPC_createEvent(const IPC_str *name, bool manual, bool value)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeEvent);
    if(!h)
        return NULL;

    h->ipc_data = NULL;

    if(manual) {
        h->ipc_size = 1;
    } else {
        h->ipc_size = 0;
    }

    union semun semarg;
    struct semid_ds seminfo;

    h->ipc_descr.ipc_sem = semget(h->ipc_key, 1, IPC_SVSEM_MODE | IPC_CREAT | IPC_EXCL);

    if(h->ipc_descr.ipc_sem > 0) {

        semarg.val = 1;

        int res = semctl(h->ipc_descr.ipc_sem, 0, SETVAL, semarg);
        if(res < 0) {
            DEBUG_PRINT("%s(): event - %s created but not initialized. %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            delete_ipc_object(h);
            return NULL;
        }

        if(!value) {

            struct sembuf ops;
            ops.sem_num = 0;
            ops.sem_flg = SEM_UNDO;
            ops.sem_op = -1;

            res = semtimedop(h->ipc_descr.ipc_sem, &ops, 1, NULL);
            if(res < 0) {
                delete_ipc_object(h);
                DEBUG_PRINT("%s(): %s - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
                return NULL;
            }
        }

    } else if(errno == EEXIST) {

        h->ipc_descr.ipc_sem = semget(h->ipc_key, 1, IPC_SVSEM_MODE);

        semarg.buf = &seminfo;

        for(int i=0; i<10; i++) {

            int res = semctl(h->ipc_descr.ipc_sem, 0, IPC_STAT, semarg);
            if(res < 0) continue;
            if(semarg.buf->sem_otime != 0) {
                DEBUG_PRINT("%s(): event - %s opened\n", __FUNCTION__, h->ipc_name);
                return h;
            }
            IPC_delay(10);
        }

        DEBUG_PRINT("%s(): event - %s created but not initialized\n", __FUNCTION__, h->ipc_name);
        delete_ipc_object(h);
        return NULL;
    }

    DEBUG_PRINT("%s(): event - %s created\n", __FUNCTION__, h->ipc_name);

    return h;
}

//-----------------------------------------------------------------------------

int IPC_waitEvent(const  IPC_handle handle, int timeout)
{
    if(!handle)
        return IPC_invalidHandle;

    ipc_handle_t h = (ipc_handle_t)handle;

    struct sembuf ops;

    ops.sem_num = 0;
    ops.sem_op = -1;
    ops.sem_flg = SEM_UNDO;

    if(timeout > 0) {

        struct timespec ts;

        int nsec = timeout / 1000;

        ts.tv_sec = nsec;
        ts.tv_nsec = timeout;

        //DEBUG_PRINT("%s(): ts.tv_sec = %d\n", __FUNCTION__, (int)ts.tv_sec);
        //DEBUG_PRINT("%s(): ts.tv_nsec = %d\n", __FUNCTION__, (int)ts.tv_nsec);
        //DEBUG_PRINT("%s(): Try wait event - %s\n", __FUNCTION__, h->ipc_name);

        int res = semtimedop(h->ipc_descr.ipc_sem, &ops, 1, &ts);
        if(res < 0) {
            if(errno == EINTR) {
                DEBUG_PRINT("%s(): Waiting was interrputed - %s\n", __FUNCTION__, h->ipc_name);
                return IPC_interrupted;
            }
            if(errno == EAGAIN) {
                DEBUG_PRINT("%s(): Specified timeout expired - %s\n", __FUNCTION__, h->ipc_name);
                return IPC_timeout;
            }
            DEBUG_PRINT("%s(): %s - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            return IPC_generalError;
        }

    } else {

        int res = semtimedop(h->ipc_descr.ipc_sem, &ops, 1, NULL);
        if(res < 0) {
            if(errno == EINTR) {
                DEBUG_PRINT("%s(): Waiting was interrputed - %s\n", __FUNCTION__, h->ipc_name);
                return IPC_interrupted;
            }
            DEBUG_PRINT("%s(): %s - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
            return IPC_generalError;
        }
    }

    if(!h->ipc_size) {

        // так как событие с автосбросом, то
        // мы увеличим значение счетчика сами
        // чтобы был возможен повторный вход
        ops.sem_num = 0;
        ops.sem_op = 1;
        ops.sem_flg = SEM_UNDO;

        semop(h->ipc_descr.ipc_sem, &ops, 1);
    }

    DEBUG_PRINT("%s(): event - %s locked\n", __FUNCTION__, h->ipc_name);

    return IPC_ok;
}

//-----------------------------------------------------------------------------

int IPC_setEvent(const  IPC_handle handle)
{
    if(!handle)
        return IPC_invalidHandle;

    ipc_handle_t h = (ipc_handle_t)handle;

    struct sembuf ops;

    ops.sem_num = 0;
    ops.sem_op = 1;
    ops.sem_flg = SEM_UNDO;

    int res = semop(h->ipc_descr.ipc_sem,&ops,1);
    if(res < 0) {
        if(errno == EINTR) {
            DEBUG_PRINT("%s(): Posting was interrputed - %s\n", __FUNCTION__, h->ipc_name);
            return IPC_interrupted;
        }
        DEBUG_PRINT("%s(): %s - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
        return IPC_generalError;
    }

    DEBUG_PRINT("%s(): event - %s unlocked\n", __FUNCTION__, h->ipc_name);

    return IPC_ok;
}

//-----------------------------------------------------------------------------

int IPC_resetEvent(const  IPC_handle handle)
{
    if(!handle)
        return IPC_invalidHandle;

    ipc_handle_t h = (ipc_handle_t)handle;

    struct sembuf ops;

    ops.sem_num = 0;
    ops.sem_op = -1; // возможно отрицательное значение счетчика, нужно как-то это обойти
    ops.sem_flg = SEM_UNDO;

    int res = semop(h->ipc_descr.ipc_sem,&ops,1);
    if(res < 0) {
        if(errno == EINTR) {
            DEBUG_PRINT("%s(): Posting was interrputed - %s\n", __FUNCTION__, h->ipc_name);
            return IPC_interrupted;
        }
        DEBUG_PRINT("%s(): %s - %s\n", __FUNCTION__, h->ipc_name, strerror(errno));
        return IPC_generalError;
    }

    DEBUG_PRINT("%s(): event - %s unlocked\n", __FUNCTION__, h->ipc_name);

    return IPC_ok;
}

//-----------------------------------------------------------------------------

int IPC_deleteEvent(IPC_handle handle)
{
    if(!handle)
        return IPC_invalidHandle;

    ipc_handle_t h = (ipc_handle_t)handle;

    if(h->ipc_type != IPC_typeEvent)
        return IPC_invalidHandle;

    if( is_ok_remove(h) ) {

        if( semctl(h->ipc_descr.ipc_sem, 0, IPC_RMID) < 0 ) {
            DEBUG_PRINT( "%s(): semctl(ipc_rmid) error %s\n", __FUNCTION__, h->ipc_name );
            DEBUG_PRINT( "%s(): %s\n", __FUNCTION__, strerror(errno) );
            return IPC_generalError;
        }
    }

    DEBUG_PRINT("%s(): semaphore - %s deleted\n", __FUNCTION__, h->ipc_name);

    delete_ipc_object((ipc_handle_t)handle);

    return IPC_ok;
}

//-----------------------------------------------------------------------------

#endif //__IPC_LINUX__