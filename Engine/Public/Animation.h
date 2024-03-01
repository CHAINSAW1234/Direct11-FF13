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

	_float								m_fDuration = { 0.0f };		 /* 전체 재생 길이. */
	_float								m_fTickPerSecond = { 0.0f }; /* 초당 얼마나 재생을 해야하는가 (속도) */
	_float								m_fTrackPosition = { 0.0f }; /* 현재 애니메이션이 어디까지 재생되었는지"? */

	_uint								m_iNumChannels = { 0 };		 // 이 애니메이션이 사용하는 체널의 개수 : 뼈의 개수
	vector<class CChannel*>				m_Channels;					// 그 체널의 데이터

public:
	static CAnimation* Create(const aiAnimation* pAIAnimation, const vector<class CBone*>& Bones);
	virtual void Free() override;
};

END

