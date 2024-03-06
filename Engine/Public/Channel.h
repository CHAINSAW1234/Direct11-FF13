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
	HRESULT Initialize(const aiNodeAnim* pAIChannel, const vector<CBone*>& Bones);
	void Invalidate_TransformationMatrix(const vector<CBone*>& Bones, _float fTrackPosition, _uint* pCurrentKeyFrameIndex);

private:
	_char				m_szName[MAX_PATH] = { "" };		// �� ü���� ����ϴ� ���� �̸�		// �ʿ� �Ѱ�?
	_uint				m_iBoneIndex = { 0 };				// �� ü���� ����ϴ� ���� �𵨿����� �ε���
	_uint				m_iNumKeyFrames = { 0 };			// �� ü�ο��� ����ϴ� Ű�������� ����
	vector<KEYFRAME>	m_KeyFrames;						// ������ Ű�������� �����͸� ���� �����̳ʿ� ����

public:
	static CChannel* Create(const aiNodeAnim* pAIChannel, const vector<CBone*>& Bones);
	virtual void Free() override;
};

END