
#ifdef __IPC_WIN__

#include <stdio.h>

#ifndef __WINIPC_H__
    #include "winipc.h"
#endif
#ifndef __SYSIPC_H__
    #include "sysipc.h"
#endif

//-----------------------------------------------------------------------------
// ��������� �������� ��� ���������� ���������
void IPC_delay(int ms)
{
    Sleep(ms);
}

//-----------------------------------------------------------------------------
// �������� ��������� ������ ��������� �������
int IPC_sysError()
{
	return GetLastError();
}

//-----------------------------------------------------------------------------
// ������� ����������
#ifdef _WIN64
IPC_handle IPC_openDevice(const wchar_t *name, int devnum)
{
    ipc_handle_t h = allocate_ipc_object(name, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

	swprintf(devname, L"\\\\.\\%s%d", AmbDeviceName, devnum);
	h->ipc_descr = CreateFile(devname,
							GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL, 
							OPEN_EXISTING, 
							FILE_FLAG_OVERLAPPED, // 0,
							NULL);
	return h;
}
#else
IPC_handle IPC_openDevice(char *devname, const char *mainname, int devnum)
{
    if(!mainname) return NULL;

	sprintf(devname, "\\\\.\\%s%d", mainname, devnum);
	ipc_handle_t h = allocate_ipc_object(devname, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

	h->ipc_descr = CreateFile(devname,
							GENERIC_READ | GENERIC_WRITE,
							FILE_SHARE_READ | FILE_SHARE_WRITE,
							NULL, 
							OPEN_EXISTING, 
							FILE_FLAG_OVERLAPPED, // 0,
							NULL);
	return h;
}
#endif

//-----------------------------------------------------------------------------

int IPC_closeDevice(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = CloseHandle(h->ipc_descr);
    delete_ipc_object(h);
    return res;
}

//-----------------------------------------------------------------------------

int IPC_readDevice(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

	unsigned long readsize;
    int res = ReadFile(h->ipc_descr, data, size, &readsize, NULL);

    return res;
}

//-----------------------------------------------------------------------------

int IPC_writeDevice(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

	unsigned long writesize;
    int res = WriteFile(h->ipc_descr, data, size, &writesize, NULL);

    return res;
}

//-----------------------------------------------------------------------------

int IPC_ioctlDevice(IPC_handle handle, unsigned long cmd, void *srcBuf, int srcSize, void *dstBuf, int dstSize)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    //int res = ioctl(h->ipc_descr, cmd, param);

	ULONG   length;     // the return length from the driver
	int res = DeviceIoControl(h->ipc_descr,
								cmd,
								srcBuf,
								srcSize,
								dstBuf,
								dstSize,
								&length,
								NULL);

    return res;
}

int IPC_ioctlDev(IPC_handle handle, unsigned long cmd, void *srcBuf, int srcSize, void *dstBuf, int dstSize, void *overlap)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    //int res = ioctl(h->ipc_descr, cmd, param);

	ULONG   length;     // the return length from the driver
	int res = DeviceIoControl(h->ipc_descr,
								cmd,
								srcBuf,
								srcSize,
								dstBuf,
								dstSize,
								&length,
								(LPOVERLAPPED)overlap);

    return res;
}


// ������� ����
// amode (access_mode) = GENERIC_READ, GENERIC_WRITE
// smode (share_mode) = 0 (������), FILE_SHARE_READ, FILE_SHARE_WRITE
//                   ����� ������ ������������� FILE_SHARE_WRITE | FILE_SHARE_READ (_SH_DENYNO)
// cmode (exist_mode) = CREATE_ALWAYS, OPEN_EXISTING
// flag = FILE_ATTRIBUTE_NORMAL, FILE_FLAG_NO_BUFFERING
IPC_handle IPC_openFile(const IPC_str *name, IPC_flags flags)
{
    if(!name) return NULL;
	ipc_handle_t h = allocate_ipc_object(name, IPC_typeFile);
    if(!h) return NULL;

    h->ipc_size = 0;

	unsigned long amode = 0;
	unsigned long cmode = 0;
	if(IPC_CREATE_FILE == flags)
		cmode = CREATE_ALWAYS;
	if(IPC_OPEN_FILE == flags)
		cmode = OPEN_EXISTING;

	h->ipc_descr = CreateFile(name,
							  amode,
							  FILE_SHARE_WRITE | FILE_SHARE_READ,
							  NULL,
							  cmode,
							  FILE_ATTRIBUTE_NORMAL,
							  NULL);
   	return h;
}
/*
	// �������� ����� ��� ������ ������ �� ����
	HANDLE hfile = CreateFile(	fileName,
								GENERIC_WRITE,
//								FILE_SHARE_WRITE | FILE_SHARE_READ,
								0,
								NULL,
								CREATE_ALWAYS,
//								FILE_ATTRIBUTE_NORMAL,
							    FILE_FLAG_NO_BUFFERING,// | FILE_FLAG_WRITE_THROUGH,         
								NULL);
	if(hfile == INVALID_HANDLE_VALUE)
		
	// �������� ����� ��� ������ ��������� ������
	HANDLE hfile = CreateFile(	fileName,
								GENERIC_WRITE,
								FILE_SHARE_WRITE | FILE_SHARE_READ,
								NULL,
								CREATE_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
	if(hfile == INVALID_HANDLE_VALUE)

	// �������� ����� ��� �������� ��������� ������ � ����� �����
		hfile = CreateFile(	fileName,
								GENERIC_WRITE,
								FILE_SHARE_WRITE | FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
		LONG HiPart = 0;
		SetFilePointer(hfile, 0, &HiPart, FILE_END);

	// �������� ������ � ����� ����� � �������
	HANDLE hfile = CreateFile(	fileName,
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_WRITE | FILE_SHARE_READ,
								NULL,
								OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
	if(hfile == INVALID_HANDLE_VALUE)
	{
		BRDC_printf(_BRDC("Create file %s error.\n"), fileName);
		return;
	}
	long lDistanceToMoveLow = 0L;
	long lDistanceToMoveHigh = 0L;
	ULONG BytesNum = SetFilePointer(hfile, lDistanceToMoveLow, &lDistanceToMoveHigh, FILE_END);

////////////////////////////////////////////
	// ��� ������� ���� � LINUX, ��� ������ � �������� �����
	// fopen(), fclose(), fputs(), fgets()
	// �������� ���-����� � brdshell
	flog = BRDC_fopen( logFile, _BRDC("wt") );	
	// �������� ini-����� � brdshell
	fin = BRDC_fopen( REG_FILENAME, _BRDC("rt") );
////////////////////////////////////////////

	// ������ ��������� �����
	fs = BRDC_sopen(fileName, _O_WRONLY|_O_BINARY|_O_CREAT|_O_TRUNC, _SH_DENYNO, _S_IWRITE);
	// ������ ��������� �����
    fs = BRDC_sopen(fileName, _O_RDONLY|_O_BINARY, _SH_DENYNO, _S_IREAD);

}
*/
//-----------------------------------------------------------------------------

int IPC_closeFile(IPC_handle handle)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

    int res = CloseHandle(h->ipc_descr);
    delete_ipc_object(h);
    return res;
}

//-----------------------------------------------------------------------------

int IPC_readFile(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

	unsigned long readsize;
    int res = ReadFile(h->ipc_descr, data, size, &readsize, NULL);

    return res;
}

//-----------------------------------------------------------------------------

int IPC_writeFile(IPC_handle handle, void *data, int size)
{
    ipc_handle_t h = (ipc_handle_t)handle;
    if(!h) return IPC_invalidHandle;

	unsigned long writesize;
    int res = WriteFile(h->ipc_descr, data, size, &writesize, NULL);

    return res;
}

#endif //__IPC_WIN__

