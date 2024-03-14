#pragma once
#include "Base.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	void Invalidate_TransformationMatrix(_float fTimeDelta, const vector<class CBone*>& Bones, _bool isLoop);
	void Invalidate_TransformationMatrix_Linear_Interpolation(_float fTimeDelta, const vector<class CBone*>& Bones, CAnimation* pNextAnimation);
	
	void Reset_Animation();
	HRESULT Save_Animation(ofstream& OFS);

	_bool isFinished() const {
		return m_isFinished;
	}

	_float Get_TrackPosition() { return m_fTrackPosition; }

private:
	HRESULT Load_Animation(ifstream& IFS);

private:
	_bool								m_isCloned = { false };
	_char								m_szName[MAX_PATH] = { "" };

	_float								m_fDuration = { 0.0f };		 /* ��ü ��� ����. */
	_float								m_fTickPerSecond = { 0.0f }; /* �ʴ� �󸶳� ����� �ؾ��ϴ°� (�ӵ�) */
	_float								m_fTrackPosition = { 0.0f }; /* ���� �ִϸ��̼��� ������ ����Ǿ�����"? */

	_uint								m_iNumChannels = { 0 };		 // �� �ִϸ��̼��� ����ϴ� ü���� ���� : ���� ����
	vector<class CChannel*>				m_Channels;					// �� ü���� ������
	vector<_uint>						m_CurrentKeyFrameIndices;	// ���� �����ӿ��� CChannel�� ���ؾߵǴ� Ű������
																	// CChanel-> CAnimation			// �޴� Ÿ�ֿ̹� ���� ū��?
	_bool								m_isFinished = { false };
public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	static CAnimation* Create(ifstream& IFS);
	CAnimation* Clone();
	virtual void Free() override;
};

END

