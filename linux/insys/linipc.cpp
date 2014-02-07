
#ifdef __IPC_LINUX__

#ifndef __LINIPC_H__
#include "linipc.h"
#endif
#ifndef __IPCIOCTL_H__
#include "ipcioctl.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <error.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <libgen.h>

//-----------------------------------------------------------------------------
#define IPC_SHM_PREFIX "/"
//#define __VERBOSE__
//-----------------------------------------------------------------------------

static char* create_raw_name(const char *name)
{
    size_t len = strlen(name)+1;

    if(!name)
        return NULL;

    char *ipc_name = (char*)malloc(len);
    if(!ipc_name) {
        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno));
        return NULL;
    }

    memset(ipc_name, 0, len);

    snprintf(ipc_name, len, "%s", name);

    DEBUG_PRINT("%s(): %s\n", __FUNCTION__, ipc_name);

    return ipc_name;
}

//-----------------------------------------------------------------------------

static void delete_ipc_name(char *ipc_name)
{
    free(ipc_name);
}

//-----------------------------------------------------------------------------

ipc_handle_t allocate_ipc_object(const char *name, IPC_type type)
{
    ipc_handle_t h = (ipc_handle_t)malloc(sizeof(struct ipc_t));
    if(!h) {
        return NULL;
    }

    memset(h,0,sizeof(struct ipc_t));

    const char* pname = 0;
    if(!name) {
      pname = basename(tmpnam(0));
    } else {
      pname = name;
    }

    h->ipc_type = type;
    if(h->ipc_type == IPC_typeSharedMem) {

        char tmpName[PATH_MAX] = {'\0'};

        snprintf(tmpName, PATH_MAX, "%s%s", IPC_SHM_PREFIX, pname);

        int len = strlen(tmpName)+1;

        h->ipc_name = (char*)malloc(len);
        if(!h->ipc_name) {
            DEBUG_PRINT("%s(): %s\n", __FUNCTION__, strerror(errno));
            return NULL;
        }

        snprintf(h->ipc_name, len, "%s", tmpName);

        DEBUG_PRINT("%s(): %s\n", __FUNCTION__, h->ipc_name);

    } else {
        h->ipc_name = create_raw_name(pname);
    }

    return h;
}

//-----------------------------------------------------------------------------

void delete_ipc_object(ipc_handle_t h)
{
    if(!h) return;

    //! сделать что-нить зависящее от типа IPC
    switch(h->ipc_type) {
    case IPC_typeSemaphore:
    case IPC_typeSharedMem:
    case IPC_typeMutex:
    case IPC_typeEvent:
    case IPC_typeThread:
    case IPC_typeLibrary:
    case IPC_typeFile:
    case IPC_typeDevice:
    case IPC_typeDir:
    case IPC_typeSocket:
        break;
    }

    if(h->ipc_name)
        delete_ipc_name(h->ipc_name);

    free(h);
}

//-----------------------------------------------------------------------------

bool chechk_handle(ipc_handle_t h, int h_type)
{
    if(!h) return false;

    if(h->ipc_type != h_type)
        return false;

    return true;
}

//-----------------------------------------------------------------------------

#ifdef _INSYS_IPC_

//-----------------------------------------------------------------------------
static int ipc_driver_fd = -1;
//-----------------------------------------------------------------------------

int open_ipc_driver(const char *name)
{
    if(ipc_driver_fd > 0) {
        DEBUG_PRINT("%s(): IPC device driver already was opened\n", __FUNCTION__);
        return ipc_driver_fd;
    }

    ipc_driver_fd = open(name, O_RDWR);
    if(ipc_driver_fd < 0) {
        DEBUG_PRINT("%s(): error open IPC device driver - %s\n", __FUNCTION__, name);
    }

    return ipc_driver_fd;
}

//-----------------------------------------------------------------------------

int ipc_driver_handle(void)
{
    if(ipc_driver_fd > 0) {
        return ipc_driver_fd;
    }

    return -1;
}

//-----------------------------------------------------------------------------

int close_ipc_driver(int fd)
{
    if(ipc_driver_fd > 0) {
        close(ipc_driver_fd);
        ipc_driver_fd = -1;
    }

    return 0;
}

//-----------------------------------------------------------------------------

int IPC_init(void)
{
    return open_ipc_driver("/dev/"IPC_DRIVER_NAME);
}

//-----------------------------------------------------------------------------

int IPC_cleanup(void)
{
    return close_ipc_driver(-1);
}

//-----------------------------------------------------------------------------

#endif //_INSYS_IPC_
#endif //__IPC_LINUX__
