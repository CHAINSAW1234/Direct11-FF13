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


	/* �� �ִϸ��̼��� ��� ���� ��Ʈ���ؾ��ϴ°�? */
	m_iNumChannels = pAIAnimation->mNumChannels;
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);	// Prototype�� ���� ������� �ʴ´� �����ϸ�, �� �ڵ�� �ʿ��ұ�?

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
	m_isFinished = false;	// �������� ���θ� �� ƽ �ʱ�ȭ

	m_fTrackPosition += m_fTickPerSecond * fTimeDelta;

	if (false == isLoop) {	// ���� �ִϸ��̼��� �ƴ� ���
		if (m_fDuration <= m_fTrackPosition) {	// ���� ��� �ð��� ��ü ��� �ð����� ��� -> �ִϸ��̼� ���� ��
			m_isFinished = true;	// �ִϸ��̼��� �����ٰ� ����
			return;					// ��� ������Ʈ ����
		}
	}
	else {	// ���� �ִϸ��̼��� ���
		if (m_fDuration + 1 <= m_fTrackPosition) {
			m_fTrackPosition = 0.f; // ���� ��� �ð��� �ʱ�ȭ
		}
	}


	for (size_t i = 0; i < m_iNumChannels; ++i) {
		/* �� ���� ��������� ���� CBone�� TransformationMatrix�� �ٲ��. */
		m_Channels[i]->Invalidate_TransformationMatrix(Bones, m_fTrackPosition, m_fTickPerSecond, &m_CurrentKeyFrameIndices[i]);
	}

}

void CAnimation::Invalidate_TransformationMatrix_Linear_Interpolation(_float fTimeDelta, const vector<class CBone*>& Bones, CAnimation* pNextAnimation)
{
	// 1. ���� �ִϸ��̼��� ��ȸ�ϸ鼭 �ߺ��Ǵ� ������ ������Ʈ
	for (_uint i = 0; i < m_iNumChannels; ++i) {
		for (_uint j = 0; j < pNextAnimation->m_iNumChannels; ++j) {
			if (pNextAnimation->m_Channels[j]->Get_iBoneIndex() == m_Channels[i]->Get_iBoneIndex()) {
					m_Channels[i]->Invalidate_TransformationMatrix_Linear_Interpolation(Bones, m_fTrackPosition, &m_CurrentKeyFrameIndices[i], fTimeDelta, pNextAnimation->m_Channels[j], pNextAnimation->m_CurrentKeyFrameIndices[j]);
			}
			
		}
	}
	// 2. �ߺ����� �ʴ� ���� ������?

}

void CAnimation::Reset_Animation()
{
	m_isFinished = false;
	m_fTrackPosition = 0.f;
	for (auto& CurrentKeyFrameIndex : m_CurrentKeyFrameIndices) {
		CurrentKeyFrameIndex = 0;
	}
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

void CAnimation::Set_TrackPosition(_float fTrackPosition)
{
	m_fTrackPosition = fTrackPosition;
	m_isFinished = false;
	for (size_t i = 0; i < m_Channels.size(); ++i) {
		m_Channels[i]->Update_KeyFrame(m_fTrackPosition, &m_CurrentKeyFrameIndices[i]);
	}
}

HRESULT CAnimation::Load_Animation(ifstream& IFS)
{
	size_t szNameLength = 0;
	IFS.read(reinterpret_cast<char*>(&szNameLength), sizeof(size_t));
	IFS.read(reinterpret_cast<char*>(&m_szName), sizeof(char) * szNameLength);
	IFS.read(reinterpret_cast<char*>(&m_fDuration), sizeof(_float));
	IFS.read(reinterpret_cast<char*>(&m_fTickPerSecond), sizeof(_float));
	
	IFS.read(reinterpret_cast<char*>(&m_iNumChannels), sizeof(_uint));
	m_CurrentKeyFrameIndices.resize(m_iNumChannels);	// Prototype�� ���� ������� �ʴ´� �����ϸ�, �� �ڵ�� �ʿ��ұ�?

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