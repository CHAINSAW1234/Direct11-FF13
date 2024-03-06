#include "Animation.h"
#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_isCloned{ true }
	, m_fDuration{rhs.m_fDuration}
	, m_fTickPerSecond{rhs.m_fTickPerSecond}
	, m_fTrackPosition{rhs.m_fTrackPosition}
	, m_iNumChannels{rhs.m_iNumChannels}
	, m_Channels{rhs.m_Channels}
	, m_CurrentKeyFrameIndices{rhs.m_CurrentKeyFrameIndices}
	, m_isFinished{rhs.m_isFinished}
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CAnimation::Initialize(const aiAnimation * pAIAnimation, const vector<CBone*>& Bones)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);

	m_fDuration = pAIAnimation->mDuration;

	m_fTickPerSecond = pAIAnimation->mTicksPerSecond;


	/* 이 애니메이션은 몇개의 뼈를 컨트롤해야하는가? */
	m_iNumChannels = pAIAnimation->mNumChannels;
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);	// Prototype은 실제 재생하지 않는다 가정하면, 이 코드는 필요할까?

	for (size_t i = 0; i < m_iNumChannels; ++i) {
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], Bones);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::Invalidate_TransformationMatrix(_float fTimeDelta, const vector<CBone*>& Bones, _bool isLoop)
{
	m_isFinished = false;	// 끝났는지 여부를 매 틱 초기화

	m_fTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (m_fDuration <= m_fTrackPosition) {	// 현재 재생 시간이 전체 재생 시간보다 길다 -> 애니메이션 종료 시
		if (false == isLoop) {	
			m_isFinished = true;	// 애니메이션이 끝났다고 전달
			return;					// 루프 애니메이션이 아닌 경우 행렬 업데이트 중지
		}
		// 루프 애니메이션인 경우
		m_fTrackPosition = 0.f;		// 현재 재생 시간을 초기화
	}

	for (size_t i = 0; i < m_iNumChannels; ++i) {
		/* 이 뼈의 상태행렬을 만들어서 CBone의 TransformationMatrix를 바꿔라. */
		m_Channels[i]->Invalidate_TransformationMatrix(Bones, m_fTrackPosition, &m_CurrentKeyFrameIndices[i]);
	}

}

void CAnimation::Invalidate_TransformationMatrix_Linear_Interpolation(_float fTimeDelta, const vector<class CBone*>& Bones, CAnimation* pNextAnimation)
{
	// 1. 이전 애니메이션을 순회하면서 중복되는 뼈들을 업데이트
	for (size_t i = 0; i < m_iNumChannels; ++i) {
		for(auto& pNextChannel : pNextAnimation->m_Channels)
			if (pNextChannel->Get_iBoneIndex() == m_Channels[i]->Get_iBoneIndex()) {
				m_Channels[i]->Invalidate_TransformationMatrix_Linear_Interpolation(Bones, m_fTrackPosition, &m_CurrentKeyFrameIndices[i], fTimeDelta, pNextChannel);
		}
	}
	// 2. 중복되지 않는 뼈는 어케함?

}

HRESULT CAnimation::Save_Animation(ofstream& OFS)
{
	size_t szNameLength = strlen(m_szName);
	OFS.write(reinterpret_cast<const char*>(&szNameLength), sizeof(size_t));
	OFS.write(reinterpret_cast<const char*>(&m_szName), sizeof(char) * szNameLength);
	OFS.write(reinterpret_cast<const char*>(&m_fDuration), sizeof(_float));
	OFS.write(reinterpret_cast<const char*>(&m_fTickPerSecond), sizeof(_float));

	OFS.write(reinterpret_cast<const char*>(&m_iNumChannels), sizeof(_uint));
	for (auto& pChannel : m_Channels) {
		pChannel->Save_Channel(OFS);
	}

	return S_OK;
}

HRESULT CAnimation::Load_Animation(ifstream& IFS)
{
	size_t szNameLength = 0;
	IFS.read(reinterpret_cast<char*>(&szNameLength), sizeof(size_t));
	IFS.read(reinterpret_cast<char*>(&m_szName), sizeof(char) * szNameLength);
	IFS.read(reinterpret_cast<char*>(&m_fDuration), sizeof(_float));
	IFS.read(reinterpret_cast<char*>(&m_fTickPerSecond), sizeof(_float));
	
	IFS.read(reinterpret_cast<char*>(&m_iNumChannels), sizeof(_uint));
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);	// Prototype은 실제 재생하지 않는다 가정하면, 이 코드는 필요할까?

	for (size_t i = 0; i < m_iNumChannels; ++i) {
		CChannel* pChannel = CChannel::Create(IFS);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

CAnimation * CAnimation::Create(const aiAnimation * pAIAnimation, const vector<CBone*>& Bones)
{
	CAnimation*		pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAIAnimation, Bones)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Create(ifstream& IFS)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Load_Animation(IFS)))
	{
		MSG_BOX(TEXT("Failed To Load : CAnimation"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	if (false == m_isCloned) {
		for (auto& pChannel : m_Channels)
			Safe_Release(pChannel);

		m_Channels.clear();
	}
}