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

	_float								m_fDuration = { 0.0f };		 /* 전체 재생 길이. */
	_float								m_fTickPerSecond = { 0.0f }; /* 초당 얼마나 재생을 해야하는가 (속도) */
	_float								m_fTrackPosition = { 0.0f }; /* 현재 애니메이션이 어디까지 재생되었는지"? */

	_uint								m_iNumChannels = { 0 };		 // 이 애니메이션이 사용하는 체널의 개수 : 뼈의 개수
	vector<class CChannel*>				m_Channels;					// 그 체널의 데이터
	vector<_uint>						m_CurrentKeyFrameIndices;	// 현재 프레임에서 CChannel이 취해야되는 키프레임
																	// CChanel-> CAnimation			// 받는 타이밍에 가장 큰거?
	_bool								m_isFinished = { false };
public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	static CAnimation* Create(ifstream& IFS);
	CAnimation* Clone();
	virtual void Free() override;
};

END

