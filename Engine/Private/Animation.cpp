#include "Animation.h"
#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(CAnimation& rhs)
	: m_isCloned{ true }
	, m_fDuration{rhs.m_fDuration}
	, m_fTickPerSecond{rhs.m_fTickPerSecond}
	, m_fTrackPosition{rhs.m_fTrackPosition}
	, m_iNumChannels{rhs.m_iNumChannels}
	, m_Channels{rhs.m_Channels}
	, m_CurrentKeyFrameIndices{rhs.m_CurrentKeyFrameIndices}
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

	if (m_fDuration <= m_fTrackPosition) {	// ���� ��� �ð��� ��ü ��� �ð����� ��� -> �ִϸ��̼� ���� ��
		if (false == isLoop) {	
			m_isFinished = true;	// �ִϸ��̼��� �����ٰ� ����
			return;					// ���� �ִϸ��̼��� �ƴ� ��� ��� ������Ʈ ����
		}
		// ���� �ִϸ��̼��� ���
		m_fTrackPosition = 0.f;		// ���� ��� �ð��� �ʱ�ȭ
	}

	for (size_t i = 0; i < m_iNumChannels; ++i) {
		/* �� ���� ��������� ���� CBone�� TransformationMatrix�� �ٲ��. */
		m_Channels[i]->Invalidate_TransformationMatrix(Bones, m_fTrackPosition, &m_CurrentKeyFrameIndices[i]);
	}

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