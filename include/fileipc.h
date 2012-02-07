
#ifndef __FILEIPC_H__
#define __FILEIPC_H__

#ifndef __IPCLIB_H__
    #include "ipclib.h"
#endif

//----------------------------------------------------------------------
// ����� ��� ������ � ��������� ���������
//----------------------------------------------------------------------

enum IPC_flags {

    IPC_CREATE_FILE = 0x1,    //!< ������ ������� ����
    IPC_OPEN_FILE   = 0x2,    //!< ��������� ����  (���� �� ���������� - ������??!!)
    IPC_FILE_RDONLY = 0x10,    //!< ��������� ���� � ������ ������ ��� ������
    IPC_FILE_WRONLY = 0x20,    //!< ��������� ���� � ������ ������ ��� ������
    IPC_FILE_RDWR   = 0x40    //!< ��������� ���� � ������ ������/������
};

enum IPC_posMethod {

    IPC_FILE_BEG =	0,    //!< ������� ����� � ������ �����
    IPC_FILE_CUR =	1,    //!< ������� ����� � ������� �������
    IPC_FILE_END =	2    //!< ������� ����� �� ����� �����
};

enum IPC_Attribute {

    IPC_FILE_NORMAL		=	0,    //!< for WINDOWS - FILE_ATTRIBUTE_NORMAL
    IPC_FILE_NOBUFFER	=	1,    //!< for WINDOWS - FILE_FLAG_NO_BUFFERING
    IPC_FILE_WRTHROUGH	=	2    //!< for WINDOWS - FILE_FLAG_WRITE_THROUGH
};

//----------------------------------------------------------------------
// ������� ��� ������ � ���������� ��������
//----------------------------------------------------------------------

extern "C" {

    //! ������� ��������� ����
    /*!
    \param name - ��� ����� ��� ����� ����������
    \param flags - �����
    \return ���������� IPC ��� NULL
    */
GIPCY_API    IPC_handle IPC_openFile(const IPC_str *name, int flags);

    //! ������� ��������� ���� � ��������������� �����������
    /*!
    \param name - ��� ����� ��� ����� ����������
    \param flags - �����
    \return ���������� IPC ��� NULL
    */
GIPCY_API    IPC_handle IPC_openFileEx(const IPC_str *name, int flags, int attr);

    //! ������� ��������� ����
    /*!
    \param handle - ���������� IPC
    \return ��� ������
    */
GIPCY_API    int IPC_closeFile(IPC_handle handle);

    //! ������� ������ �� �����
    /*!
    \param handle - ���������� �����
    \param data - ��������� �� ����� ������
    \param size - ������ ������ ������
    \return ���������� ����������� ����
    */
GIPCY_API    int IPC_readFile(IPC_handle handle, void *data, int size);

    //! ������� ������ � ����
    /*!
    \param handle - ���������� IPC
    \param data - ��������� �� ����� ������
    \param size - ������ ������ ������
    \return ���������� ���������� ����
    */
GIPCY_API    int IPC_writeFile(IPC_handle handle, void *data, int size);

    //! ������� ��������� ������� � �����
    /*!
    \param handle - ���������� IPC
    \param pos - ����� ������� � �����
    \param method - ������ ��������� ����� �������
    \return ��� ������
    */
GIPCY_API	int IPC_setPosFile(IPC_handle handle, int pos, int method);

}

#endif //__SYSIPC_H__
