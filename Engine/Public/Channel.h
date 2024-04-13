#pragma once

#include "Base.h"

/* Ư�� �ִϸ��̼��� ����ϰ� �ִ� ���� ��, �ϳ��� ������ ǥ���Ѵ�.. */
/* �� ���� �ð��� ���� � ����(KeyFrame)�� ���ؾ��ϴ°�? */

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
	_char				m_szName[MAX_PATH] = { "" };		// �� ü���� ����ϴ� ���� �̸�		// �ʿ� �Ѱ�?
	_uint				m_iBoneIndex = { 0 };				// �� ü���� ����ϴ� ���� �𵨿����� �ε���
	_uint				m_iNumKeyFrames = { 0 };			// �� ü�ο��� ����ϴ� Ű�������� ����
	vector<KEYFRAME>	m_KeyFrames;						// ������ Ű�������� �����͸� ���� �����̳ʿ� ����

public:

#ifdef _ASSIMP
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<CBone*>& Bones);
#endif

	static CChannel* Create(ifstream& IFS);
	virtual void Free() override;
};

END