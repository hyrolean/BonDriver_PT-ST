#include "StdAfx.h"
#include "MicroPacketUtil.h"

CMicroPacketUtil::CMicroPacketUtil(void)
{
	Reset();
}

CMicroPacketUtil::~CMicroPacketUtil(void)
{
}

BOOL CMicroPacketUtil::MicroPacket(BYTE* pbPacket)
{
	BOOL bRet = FALSE;

	uint packetCounter = BIT_SHIFT_MASK(pbPacket[3], 2,  3);
	uint packetStart   = BIT_SHIFT_MASK(pbPacket[3], 1,  1);

	// �J�E���^�l���m�F
	uint count = BIT_SHIFT_MASK(mCount, 0, 3);
	mCount++;

	if (packetCounter != count) {
		OutputDebugString(L"�J�E���^�l���ُ�ł��B\n");
		Reset();
		return FALSE;
	}

	// �p�P�b�g�J�n�ʒu�t���O���m�F
	if (packetStart) {
		// �����o�C�g���m�F
		if (pbPacket[2] != 0x47) {
			OutputDebugString(L"�p�P�b�g�̐擪�o�C�g�� 0x47 �ɂȂ��Ă��܂���B\n");
		}

		if (mPacketOffset != 0) {
			OutputDebugString(L"�O�̃p�P�b�g���������܂���ł����B\n");
		}
		mPacketOffset = 0;
	}

	// �f�[�^���R�s�[
	for (int i=2; i>=0; i--) {
		if (mPacketOffset < 188) {
			m_b1TS[mPacketOffset] = pbPacket[i];
			mPacketOffset++;
		}
	}

	if (188 <= mPacketOffset) {
		// �ЂƂ̃p�P�b�g������
		mPacketOffset = 0;
		bRet = TRUE;
	}

	return bRet;
}
