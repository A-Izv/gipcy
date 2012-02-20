#ifndef __DIRIPC_H__
#define __DIRIPC_H__

#ifndef __IPCLIB_H__
	#include "ipclib.h"
#endif

//----------------------------------------------------------------------
// ������� ��� ������ ������ 
//----------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

	//! ������� ��������� ����������� ���������� c ��������������� �����������
    /*!
    \sNameFilter - ����� ������
    \sDirPath - ���� � �����, � ������� ����� �������������� �����
	\return � ������ ������ ���������� NULL
    */
GIPCY_API	IPC_handle IPC_OpenDir(const IPC_str *sNameFilter, const IPC_str *sDirPath);

	//! ������� ��������� ����������� ���������� c ��������������� �����������
    /*!
    \handle - ���������� ����������
    */
GIPCY_API	void IPC_CloseDir(IPC_handle handle);

	//! ������� ������������ ����� �����
    /*!
    \handle - ���������� ����������
	\sFindFile - ��������� ����
	\return � ������ ������ ���������� -1
    */
GIPCY_API	int IPC_FindFile(IPC_handle handle, const IPC_str *sFindFile);

	//! ������� ������������ ����� ������
    /*!
	\sNameFilter - ����� ������
	\sDirPath - ���� � �����, � ������� ����� �������������� �����
    \asEntries - ������ ��������� ������
	\nEntrCount - ������ ������� asEntries
	\pAllCount - ���������� ��������� ������
	\return � ������ ������ ���������� -1
    */
GIPCY_API	int IPC_FindFiles(const IPC_str *sNameFilter, const IPC_str *sDirPath, const IPC_str (*asEntries)[256], int nEntrCount, int *pAllCount);

#ifdef __cplusplus
};
#endif

#endif //__DIRIPC_H__