
#ifndef __DEVIPC_H__
#define __DEVIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

//----------------------------------------------------------------------
// ������� ��� ������ � ������������
//----------------------------------------------------------------------

extern "C" {

    //! ������� ��������� ����������
    /*!
    \param devname - ������ ��� ���������� � ������ �� (������������ ��������)
    \param mainname - �������� ����� ����� ����������
    \param devnum - ����� ����������
    \return ���������� IPC ��� NULL
    */
GIPCY_API    IPC_handle IPC_openDevice(IPC_str *devname, const IPC_str *mainname, int devnum);

    //! ������� ��������� ����������
    /*!
    \param handle - ���������� IPC
    \return ��� ������
    */
GIPCY_API    int IPC_closeDevice(IPC_handle handle);

    //! ������� ������ �� ����������
    /*!
    \param handle - ���������� IPC
    \param data - ��������� �� ����� ������
    \param size - ������ ������ ������
    \return ���������� ����������� ����
    */
GIPCY_API    int IPC_readDevice(IPC_handle handle, void *data, int size);

    //! ������� ������ � ����������
    /*!
    \param handle - ���������� IPC
    \param data - ��������� �� ����� ������
    \param size - ������ ������ ������
    \return ���������� ���������� ����
    */
GIPCY_API    int IPC_writeDevice(IPC_handle handle, void *data, int size);

    //! ��������� ��������� ��� ������ ���������� �����������
    struct ioctl_param {
        void *srcBuf;       //!< ����� � ������� ��� ���������� (����� ���� ���������� ������ � ������� �������� ������)
        int srcSize;        //!< ������ ������ � ������� ��� ����������
        void *dstBuf;       //!< ����� � ������� �� ����������  (����� ���� ���������� ������ �� �������� �������� ������)
        int dstSize;        //!< dstSize - ������ ������ � ������� �� ����������
    };

    //! ������� ���������� �����������
    /*!
    \param handle - ���������� IPC
    \param cmd - ��� �������
    \param srcBuf - ����� � ������� ��� ���������� (����� ���� ���������� ������ � ������� �������� ������)
    \param srcSize - ������ ������ � ������� ��� ����������
    \param dstBuf - ����� � ������� �� ����������  (����� ���� ���������� ������ �� �������� �������� ������)
    \param dstSize - ������ ������ � ������� �� ����������
    \return ��� ������
    */
GIPCY_API    int IPC_ioctlDevice(IPC_handle handle, unsigned long cmd, void *srcBuf, int srcSize, void *dstBuf, int dstSize);

    //! ���������� ������� ���������� �����������
GIPCY_API    int IPC_ioctlDeviceOvl(IPC_handle handle, unsigned long cmd, void *srcBuf, int srcSize, void *dstBuf, int dstSize, void *overlap);

    //! ������� ����������� ���������� ������ �� ����������� ������ �������� (����� ������ ��� LINUX)
GIPCY_API int IPC_mapPhysAddr(IPC_handle handle, void* virtAddr, size_t physAddr, unsigned long size);

GIPCY_API int IPC_unmapPhysAddr(IPC_handle handle, void* virtAddr, unsigned long size);

}

#endif //__DEVIPC_H__
