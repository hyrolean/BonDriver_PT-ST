// PT1Ctrl.cpp : �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include "PT1Ctrl.h"

#include "PT1CtrlMain.h"
#include "../Common/ServiceUtil.h"

CPT1CtrlMain g_cMain;
HANDLE g_hMutex;
SERVICE_STATUS_HANDLE g_hStatusHandle;

#define PT1_CTRL_MUTEX L"PT1_CTRL_EXE_MUTEX"
#define SERVICE_NAME L"PT1Ctrl Service"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	if (_tcslen(lpCmdLine) > 0) {
		if (lpCmdLine[0] == '-' || lpCmdLine[0] == '/') {
			if (_tcsicmp(_T("install"), lpCmdLine + 1) == 0) {
				WCHAR strExePath[512] = L"";
				GetModuleFileName(NULL, strExePath, 512);
				InstallService(strExePath, SERVICE_NAME, SERVICE_NAME);
				return 0;
			}
			else if (_tcsicmp(_T("remove"), lpCmdLine + 1) == 0) {
				RemoveService(SERVICE_NAME);
				return 0;
			}
		}
	}

	if (IsInstallService(SERVICE_NAME) == FALSE) {
		//���ʂ�exe�Ƃ��ċN�����s��
		HANDLE h = ::OpenMutexW(SYNCHRONIZE, FALSE, PT1_GLOBAL_LOCK_MUTEX);
		if (h != NULL) {
			BOOL bErr = FALSE;
			if (::WaitForSingleObject(h, 100) == WAIT_TIMEOUT) {
				bErr = TRUE;
			}
			::ReleaseMutex(h);
			::CloseHandle(h);
			if (bErr) {
				return -1;
			}
		}

		g_hStartEnableEvent = _CreateEvent(TRUE, TRUE, PT1_STARTENABLE_EVENT);
		if (g_hStartEnableEvent == NULL) {
			return -2;
		}
		// �ʃv���Z�X���I���������̏ꍇ�͏I����҂�(�ő�1�b)
		if (::WaitForSingleObject(g_hStartEnableEvent, 1000) == WAIT_TIMEOUT) {
			::CloseHandle(g_hStartEnableEvent);
			return -3;
		}

		g_hMutex = _CreateMutex(TRUE, PT1_CTRL_MUTEX);
		if (g_hMutex == NULL) {
			::CloseHandle(g_hStartEnableEvent);
			return -4;
		}
		if (::WaitForSingleObject(g_hMutex, 100) == WAIT_TIMEOUT) {
			// �ʃv���Z�X�����s��������
			::CloseHandle(g_hMutex);
			::CloseHandle(g_hStartEnableEvent);
			return -5;
		}

		//�N��
		StartMain(FALSE);

		::ReleaseMutex(g_hMutex);
		::CloseHandle(g_hMutex);

		::SetEvent(g_hStartEnableEvent);
		::CloseHandle(g_hStartEnableEvent);
	}
	else {
		//�T�[�r�X�Ƃ��ăC���X�g�[���ς�
		if (IsStopService(SERVICE_NAME) == FALSE) {
			g_hMutex = _CreateMutex(TRUE, PT1_CTRL_MUTEX);
			int err = GetLastError();
			if (g_hMutex != NULL && err != ERROR_ALREADY_EXISTS) {
				//�N��
				SERVICE_TABLE_ENTRY dispatchTable[] = {
					{ SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)service_main },
					{ NULL, NULL }
				};
				if (StartServiceCtrlDispatcher(dispatchTable) == FALSE) {
					OutputDebugString(_T("StartServiceCtrlDispatcher failed"));
				}
			}
		}
		else {
			//Stop��ԂȂ̂ŋN������
			StartServiceCtrl(SERVICE_NAME);
		}
	}
	return 0;
}

void WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
	g_hStatusHandle = RegisterServiceCtrlHandlerEx( SERVICE_NAME, (LPHANDLER_FUNCTION_EX)service_ctrl, NULL);

	if (g_hStatusHandle == NULL){
		goto cleanup;
	}
	
	SendStatusScm(SERVICE_START_PENDING, 0, 1);

	SendStatusScm(SERVICE_RUNNING, 0, 0);
	StartMain(TRUE);

cleanup:
	SendStatusScm(SERVICE_STOPPED, 0, 0);

   return;
}

DWORD WINAPI service_ctrl(DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	switch (dwControl){
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
			SendStatusScm(SERVICE_STOP_PENDING, 0, 1);
			StopMain();
			return NO_ERROR;
			break;
		case SERVICE_CONTROL_POWEREVENT:
			OutputDebugString(_T("SERVICE_CONTROL_POWEREVENT"));
			if ( dwEventType == PBT_APMQUERYSUSPEND ){
				OutputDebugString(_T("PBT_APMQUERYSUSPEND"));
				if( g_cMain.IsFindOpen() == TRUE ){
					OutputDebugString(_T("BROADCAST_QUERY_DENY"));
					return BROADCAST_QUERY_DENY;
					}
			}else if( dwEventType == PBT_APMRESUMESUSPEND ){
				OutputDebugString(_T("PBT_APMRESUMESUSPEND"));
			}
			break;
		default:
			break;
	}
	SendStatusScm(NO_ERROR, 0, 0);
	return NO_ERROR;
}

BOOL SendStatusScm(int iState, int iExitcode, int iProgress)
{
	SERVICE_STATUS ss;

	ss.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ss.dwCurrentState = iState;
	ss.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_POWEREVENT;
	ss.dwWin32ExitCode = iExitcode;
	ss.dwServiceSpecificExitCode = 0;
	ss.dwCheckPoint = iProgress;
	ss.dwWaitHint = 10000;

	return SetServiceStatus(g_hStatusHandle, &ss);
}

void StartMain(BOOL bService)
{
	g_cMain.StartMain(bService);
}

void StopMain()
{
	g_cMain.StopMain();
}
