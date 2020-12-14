#ifndef __PT1_OUTSIDE_CTRL_CMD_DEF_H__
#define __PT1_OUTSIDE_CTRL_CMD_DEF_H__

#include "Util.h"

#define SEND_BUFF_SIZE 1024*64
#define RES_BUFF_SIZE 1024*64

//�p�C�v��
#define CMD_PT1_CTRL_PIPE _T("\\\\.\\pipe\\PT1CtrlPipe")
#define CMD_PT1_DATA_PIPE _T("\\\\.\\pipe\\PT1DataPipe_")

//�ڑ��ҋ@�p�C�x���g
#define CMD_PT1_CTRL_EVENT_WAIT_CONNECT _T("Global\\PT1CtrlConnect")
#define CMD_PT1_DATA_EVENT_WAIT_CONNECT _T("Global\\PT1DataConnect_")

//���W���[�����R�}���h���s�C�x���g
#define CMD_CTRL_EVENT_WAIT _T("CtrlCmdEvent")

//�R�}���h
#define CMD_CLOSE_EXE 1
#define CMD_OPEN_TUNER 2
#define CMD_CLOSE_TUNER 3
#define CMD_SET_CH 4
#define CMD_GET_SIGNAL 5
#define CMD_OPEN_TUNER2 6
#define CMD_SEND_DATA 10

//�G���[�R�[�h
#define CMD_SUCCESS			0 //����
#define CMD_ERR				1 //�ėp�G���[
#define CMD_NEXT			2 //Enum�R�}���h�p�A��������
#define CMD_NON_SUPPORT		3 //���T�|�[�g�̃R�}���h
#define CMD_ERR_INVALID_ARG	4 //�����G���[
#define CMD_ERR_CONNECT		5 //�T�[�o�[�ɃR�l�N�g�ł��Ȃ�����
#define CMD_ERR_DISCONNECT	6 //�T�[�o�[����ؒf���ꂽ
#define CMD_ERR_TIMEOUT		7 //�^�C���A�E�g����
#define CMD_ERR_BUSY		8 //�r�W�[��ԂŌ��ݏ����ł��Ȃ�
#define CMD_BIT_NON_STREAM	1024 //�����Ώۂ̃X�g���[�������݂��Ȃ� by 2020 LVhJPic0JSk5LiQ1ITskKVk9UGBg

//�R�}���h����M�X�g���[��
typedef struct _CMD_STREAM{
	DWORD dwParam; //���M���R�}���h�A��M���G���[�R�[�h
	DWORD dwSize; //bData�T�C�Y
	BYTE* bData;
	_CMD_STREAM(void){
		dwSize = 0;
		bData = NULL;
	}
	~_CMD_STREAM(void){
		SAFE_DELETE_ARRAY(bData);
	}
} CMD_STREAM;


#endif