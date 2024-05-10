#include "stdafx.h"
#include "HellBlast.h"
#include "Effect.h"
#include "Chr_Battle.h"
#include "Boss.h"

CHellBlast::CHellBlast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }

{
}

CHellBlast::CHellBlast(const CHellBlast& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CHellBlast::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CHellBlast::Initialize(void* pArg)
{
	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	HELLBLAST_DESC* pDesc = (HELLBLAST_DESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPosition);

	if (FAILED(CEffect::Read_File_Loop("../Bin/Resources/Effect/Hell_Blast.dat", m_pGameInstance, m_pDevice, m_pContext, &m_Effects)))
		return E_FAIL;

	for (auto& pEffect : m_Effects) {
		pEffect->Set_Position(m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION));
		pEffect->Tick(0);
	}

	return S_OK;
}

void CHellBlast::Tick(_float fTimeDelta)
{
	Move(fTimeDelta);

	_float4 vPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	for (auto& pEffect : m_Effects)
		pEffect->Set_Position(vPosition);

	if (vPosition.y <= 0.f) {
		Set_Dead(true);
		for (auto& pEffect : m_Effects)
			pEffect->Set_Dead(true);

		Set_Damage();// 이펙트, 데미지 처리 
		m_pGameInstance->PlaySoundDuplicate(TEXT("HellBlast_Explosion.wav"), CSound_Manager::EFFECT_DUPLICATE, SOUND_DEFAULT);
		CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Explosion_Start.dat", m_pGameInstance, m_pDevice, m_pContext, vPosition);

		vPosition.y += 5.f;
		CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Particle_Explosion.dat", m_pGameInstance, m_pDevice, m_pContext, vPosition);
		CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Explosion_Boss_Near.dat", m_pGameInstance, m_pDevice, m_pContext, vPosition);
	}

}

HRESULT CHellBlast::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_BLEND, this);

	return S_OK;
}

HRESULT CHellBlast::Render()
{
	return S_OK;
}

void CHellBlast::Start()
{
}

void CHellBlast::Move(_float fTimeDelta)
{
	if (!m_isMove)
		return;

	m_pTransformCom->Go_Down(fTimeDelta);

}

void CHellBlast::Set_Damage()
{
	size_t iNum = m_pGameInstance->Get_LayerCnt(g_Level, g_strChrLayerTag);
	for (size_t i = 0; i < iNum; ++i) {
		CChr_Battle* pChr_Battle = (CChr_Battle*)m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, (_uint)i);
		pChr_Battle->Set_Hit(pChr_Battle->Get_Hp()-3);
	}

}

CHellBlast* CHellBlast::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CHellBlast* pInstance = new CHellBlast(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHellBlast"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CHellBlast::Clone(void* pArg)
{
	CHellBlast* pInstance = new CHellBlast(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CHellBlast"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CHellBlast::Free()
{
	__super::Free();

	for (auto& pEffect : m_Effects)
		Safe_Release(pEffect);
	m_Effects.clear();

}
