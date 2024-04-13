#include "stdafx.h"
#include "Sphere_Heal.h"
#include "Chr_Battle.h"
#include "UI_Number.h"

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
