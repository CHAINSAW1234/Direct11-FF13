#include "stdafx.h"
#include "Sphere_Heal.h"
#include "Chr_Battle.h"
#include "UI_Number.h"
#include "Effect.h"
#include "Effect_Camera_Look.h"

CSphere_Heal::CSphere_Heal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSphere{ pDevice, pContext }
{
}

CSphere_Heal::CSphere_Heal(const CSphere_Heal& rhs)
	: CSphere{ rhs }
{
}

void CSphere_Heal::Check_Collision()
{
	CCollider* pColliderCom = (CCollider*)m_pTargetObject->Get_Component(TEXT("Com_Collider"));
	if (nullptr == pColliderCom)
		return;

	if (m_pColliderCom->Intersect(pColliderCom)) {
		((CChr_Battle*)m_pTargetObject)->Add_Hp(m_iDamage);
		Set_Dead(true);
		for (auto& pEffect : m_Effects)
			pEffect->Set_Dead(true);

		string strEffectFilePath;
		wstring strSoundTag;
		switch (m_eSkill) {
		case CRole::RUIN:
			strEffectFilePath = "../Bin/Resources/Effect/Ruin_Hit_Camera_Look_Instance.dat";
			strSoundTag = TEXT("Ruin_Hit.wav");
			break;
		case CRole::AERO:
			strEffectFilePath = "../Bin/Resources/Effect/Aero_Hit_Camera_Look_Instance.dat";
			strSoundTag = TEXT("Aero_Hit.wav");
			break;
		case CRole::WATER:
			strEffectFilePath = "../Bin/Resources/Effect/Water_Hit_Camera_Look_Instance.dat";
			strSoundTag = TEXT("Water_Hit.wav");
			break;
		case CRole::FIRE:
			strEffectFilePath = "../Bin/Resources/Effect/Fire_Hit_Camera_Look_Instance.dat";
			strSoundTag = TEXT("Fire_Hit.wav");
			break;
		case CRole::CURE:
			strEffectFilePath = "../Bin/Resources/Effect/Heal_Camera_Look_Instance.dat";
			strSoundTag = TEXT("Cure_Hit.wav");
			break;
		}

		if (FAILED(CEffect::Read_File_NoLoop(strEffectFilePath, m_pGameInstance, m_pDevice, m_pContext, m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION))))
			return;
		m_pGameInstance->PlaySoundDuplicate(const_cast<TCHAR*>(strSoundTag.c_str()), CSound_Manager::EFFECT_DUPLICATE, SOUND_DEFAULT);
	}
}

CSphere_Heal* CSphere_Heal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSphere_Heal* pInstance = new CSphere_Heal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSphere_Heal"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSphere_Heal::Clone(void* pArg)
{
	CSphere_Heal* pInstance = new CSphere_Heal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CSphere_Heal"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSphere_Heal::Free()
{
	__super::Free();
}
