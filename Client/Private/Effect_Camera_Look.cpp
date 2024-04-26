#include "stdafx.h"
#include "Effect_Camera_Look.h"

#include "Effect.h"

CEffect_Camera_Look::CEffect_Camera_Look(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEffect_Camera_Look::CEffect_Camera_Look(const CEffect_Camera_Look& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CEffect_Camera_Look::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Camera_Look::Initialize(void* pArg)
{
	m_eLevel = g_Level;

	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	EFFECT_CAMERA_LOOK_DESC* pDesc = (EFFECT_CAMERA_LOOK_DESC*)pArg;

	m_fEffectTime = pDesc->fEffectTime;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPosition);

	string strEffectFilePath;

	switch (pDesc->eSkill) {
	case CRole::RUIN:
		strEffectFilePath = "../Bin/Resources/Effect/Ruin_Hit_Camera_Look_Instance.dat";
		break;
	case CRole::AERO:
		strEffectFilePath = "../Bin/Resources/Effect/Aero_Hit_Camera_Look_Instance.dat";
		break;
	case CRole::WATER:
		strEffectFilePath = "../Bin/Resources/Effect/Water_Hit_Camera_Look_Instance.dat";
		break;
	case CRole::FIRE:
		strEffectFilePath = "../Bin/Resources/Effect/Fire_Hit_Camera_Look_Instance.dat";
		break;
	case CRole::CURE:
		strEffectFilePath = "../Bin/Resources/Effect/Heal_Hit_Camera_Look_Instance.dat";
		break;
	}

	if (FAILED(CEffect::Read_File_NoLoop(strEffectFilePath, m_pGameInstance, m_pDevice, m_pContext, _float4(0.f,0.f,0.f,1.f), &m_Effects)))
		return E_FAIL;

	for (auto& pEffect : m_Effects) {
		pEffect->Set_Position(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
		//pEffect->Tick(0);
	}

	return S_OK;
}

void CEffect_Camera_Look::Tick(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;
	if (m_fTimeDelta >= m_fEffectTime) {
		Set_Dead(true);
		for (auto& pEffect : m_Effects) {
			pEffect->Set_Dead(true);
		}
	}

}

HRESULT CEffect_Camera_Look::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	//_vector vCamRight = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW).r[0];
	//_vector vCamUp = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW).r[1];
	//_vector vCamLook = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW).r[2];
	//for (auto& pEffect : m_Effects) {
	//	pEffect->Get_Transform()->Set_State(CTransform::STATE_RIGHT, vCamRight);
	//	pEffect->Get_Transform()->Set_State(CTransform::STATE_UP, vCamUp);
	//	pEffect->Get_Transform()->Set_State(CTransform::STATE_LOOK, vCamLook);
	//}



	return S_OK;
}

HRESULT CEffect_Camera_Look::Render()
{
	return S_OK;
}

void CEffect_Camera_Look::Start()
{
}

CEffect_Camera_Look* CEffect_Camera_Look::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffect_Camera_Look* pInstance = new CEffect_Camera_Look(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Camera_Look"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Camera_Look::Clone(void* pArg)
{
	CEffect_Camera_Look* pInstance = new CEffect_Camera_Look(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CEffect_Camera_Look"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Camera_Look::Free()
{
	__super::Free();

	for (auto& pEffect : m_Effects)
		Safe_Release(pEffect);
	m_Effects.clear();

}
