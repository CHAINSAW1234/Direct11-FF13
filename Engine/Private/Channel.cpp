#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* pAIChannel, const vector<CBone*>& Bones)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	auto iter = find_if(Bones.begin(), Bones.end(), [&](CBone* pBone)-> _bool {
		return pBone->Compare_Name(m_szName);
		});

	m_iBoneIndex = iter - Bones.begin();

	m_iNumKeyFrames = max(pAIChannel->mNumPositionKeys, pAIChannel->mNumRotationKeys, pAIChannel->mNumScalingKeys);

	_float3			vScale = { 0.f, 0.f, 0.f };
	_float4			vRotation = { 0.f, 0.f, 0.f, 0.f };
	_float3			vTranslation = { 0.f, 0.f, 0.f };
	_float			fTime = { 0.f };

	for (size_t i = 0; i < m_iNumKeyFrames; ++i) {
		KEYFRAME			KeyFrame{};

		if (i < pAIChannel->mNumScalingKeys) {
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			fTime = pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			/*memcpy(&vRotation, &pAIChannel->mRotationKeys[i].mValue, sizeof(_float4));*/
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

			fTime = pAIChannel->mRotationKeys[i].mTime;
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vTranslation, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			fTime = pAIChannel->mPositionKeys[i].mTime;
		}
		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vTranslation = vTranslation;
		KeyFrame.fTime = fTime;

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Invalidate_TransformationMatrix(const vector<CBone*>& Bones, _float fTrackPosition, _uint* pCurrentKeyFrameIndex)
{
	if (0.0f == fTrackPosition)
		(*pCurrentKeyFrameIndex) = 0;

	KEYFRAME		KeyFrame = m_KeyFrames.back();

	_float3			vScale;
	_float4			vRotation;
	_float3			vTranslation;

	if (KeyFrame.fTime <= fTrackPosition) {
		vScale = KeyFrame.vScale;
		vRotation = KeyFrame.vRotation;
		vTranslation = KeyFrame.vTranslation;
	}
	else {
		while(fTrackPosition >= m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime)
			++(*pCurrentKeyFrameIndex);

		_float fRatio = (fTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTime)
			/ (m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTime);

		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale), fRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation), XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation), fRatio));
		XMStoreFloat3(&vTranslation, XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation), fRatio));
	}

	_matrix TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f));

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

void CChannel::Invalidate_TransformationMatrix_Linear_Interpolation(const vector<CBone*>& Bones, _float fTrackPosition, _uint* pCurrentKeyFrameIndex, _float fTimeDelta,  CChannel* pNextChannel)
{
	KEYFRAME		NextKeyFrame = pNextChannel->m_KeyFrames[0];

	_float3			vScale;
	_float4			vRotation;
	_float3			vTranslation;

	// 1. 현재 fTrackPosition에서의 위치를 구한다
	_float fRatio = (fTrackPosition - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTime)
		/ (m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].fTime - m_KeyFrames[(*pCurrentKeyFrameIndex)].fTime);

	XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vScale), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vScale), fRatio));
	XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vRotation), XMLoadFloat4(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vRotation), fRatio));
	XMStoreFloat3(&vTranslation, XMVectorLerp(XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex)].vTranslation), XMLoadFloat3(&m_KeyFrames[(*pCurrentKeyFrameIndex) + 1].vTranslation), fRatio));

	// 2. 현재 위치와 새 애니메이션의 시작 위치를 선형 보간한다
	fRatio = fTimeDelta / 0.2;
	XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vScale), XMLoadFloat3(&NextKeyFrame.vScale), fRatio));
	XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vRotation), XMLoadFloat4(&NextKeyFrame.vRotation), fRatio));
	XMStoreFloat3(&vTranslation, XMVectorLerp(XMLoadFloat3(&vTranslation), XMLoadFloat3(&NextKeyFrame.vTranslation), fRatio));

	_matrix TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vTranslation), 1.f));

	Bones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

HRESULT CChannel::Save_Channel(ofstream& OFS)
{
	size_t szNameLength = strlen(m_szName);
	OFS.write(reinterpret_cast<const char*>(&szNameLength), sizeof(size_t));
	OFS.write(reinterpret_cast<const char*>(&m_szName), sizeof(char) * szNameLength);

	OFS.write(reinterpret_cast<const char*>(&m_iBoneIndex), sizeof(_uint));
	OFS.write(reinterpret_cast<const char*>(&m_iNumKeyFrames), sizeof(_uint));

	for (auto& pKeyFrame : m_KeyFrames) {
		OFS.write(reinterpret_cast<const char*>(&pKeyFrame), sizeof(KEYFRAME));
	}

	return S_OK;
}

HRESULT CChannel::Load_Channel(ifstream& IFS)
{
	size_t szNameLength = 0;
	IFS.read(reinterpret_cast<char*>(&szNameLength), sizeof(size_t));
	IFS.read(reinterpret_cast<char*>(&m_szName), sizeof(char) * szNameLength);

	IFS.read(reinterpret_cast<char*>(&m_iBoneIndex), sizeof(_uint));
	IFS.read(reinterpret_cast<char*>(&m_iNumKeyFrames), sizeof(_uint));

	for (size_t i = 0; i < m_iNumKeyFrames; ++i) {
		KEYFRAME			KeyFrame{};
		IFS.read(reinterpret_cast<char*>(&KeyFrame), sizeof(KEYFRAME));

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

CChannel* CChannel::Create(const aiNodeAnim* pAIChannel, const vector<CBone*>& Bones)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, Bones)))
	{
		MSG_BOX(TEXT("Failed To Created : CChannel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CChannel* CChannel::Create(ifstream& IFS)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Load_Channel(IFS)))
	{
		MSG_BOX(TEXT("Failed To Load : CChannel"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	m_KeyFrames.clear();
}
