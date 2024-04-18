#include "stdafx.h"
#include "Effect_Interface.h"
#include "Bone.h"

void Target_GameObject::Free()
{
	Safe_Release(m_pTargetObject);
}

void Target_Bone::Free()
{
	Safe_Release(m_pSocket);
}

const FRAME Interface_2D::EffectImageFrame[EFFECT_END] = {
	{25, 0.01f, 0, 0, 0}

};

void Interface_2D::Update_Frame(_float fTimeDelta)
{
	m_tFrame.fTime += fTimeDelta;

	if (m_tFrame.fTime > m_tFrame.fFrameSpeed)
	{
		m_tFrame.fTime = 0.f;
		++m_tFrame.iCurFrame;

		if (m_tFrame.iCurFrame >= m_tFrame.iMaxFrame) {
			Frame_Finish();
			m_tFrame.iCurFrame = 0;
		}
	}
}

void Interface_2D::Free()
{
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}

void Interface_3D::Free()
{
	Safe_Release(m_pModelCom);
}

void Interface_Instance::Free()
{
	Safe_Release(m_pVIBufferCom);
}
