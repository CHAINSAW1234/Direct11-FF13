#pragma once

#include "Base.h"

/* 특정 애니메이션이 사용하고 있는 뼈들 중, 하나의 정보를 표현한다.. */
/* 이 뼈는 시간에 따라 어떤 상태(KeyFrame)를 취해야하는가? */

BEGIN(Engine)
class CBone;

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	_uint Get_iBoneIndex() { return m_iBoneIndex; }
public:

#ifdef _ASSIMP
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<CBone*>& Bones);

#endif
	void Invalidate_TransformationMatrix(const vector<CBone*>& Bones, _float fTrackPosition, _float fTickPerSecond, _uint* pCurrentKeyFrameIndex);
	void Invalidate_TransformationMatrix_Linear_Interpolation(const vector<CBone*>& Bones, _float fTrackPosition, _uint* pCurrentKeyFrameIndex, _float fTimeDelta, CChannel* pNextChannel, _uint pNextKeyFrameIndex);
	void Update_KeyFrame(_float fTrackPosition, _uint* pCurrentKeyFrameIndex);

public:
	HRESULT Save_Channel(ofstream& OFS);

private:
	HRESULT Load_Channel(ifstream& IFS);

private:
	_char				m_szName[MAX_PATH] = { "" };		// 이 체널이 사용하는 뼈의 이름		// 필요 한가?
	_uint				m_iBoneIndex = { 0 };				// 이 체널이 사용하는 뼈의 모델에서의 인덱스
	_uint				m_iNumKeyFrames = { 0 };			// 이 체널에서 사용하는 키프레임의 개수
	vector<KEYFRAME>	m_KeyFrames;						// 각각의 키프레임의 데이터를 벡터 컨테이너에 저장

public:

#ifdef _ASSIMP
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<CBone*>& Bones);
#endif

	static CChannel* Create(ifstream& IFS);
	virtual void Free() override;
};

END