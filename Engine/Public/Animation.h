#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	void Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones);

private:
	_char								m_szName[MAX_PATH] = { "" };

	_float								m_fDuration = { 0.0f };		 /* ��ü ��� ����. */
	_float								m_fTickPerSecond = { 0.0f }; /* �ʴ� �󸶳� ����� �ؾ��ϴ°� (�ӵ�) */
	_float								m_fTrackPosition = { 0.0f }; /* ���� �ִϸ��̼��� ������ ����Ǿ�����"? */

	_uint								m_iNumChannels = { 0 };		 // �� �ִϸ��̼��� ����ϴ� ü���� ���� : ���� ����
	vector<class CChannel*>				m_Channels;					// �� ü���� ������

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END

