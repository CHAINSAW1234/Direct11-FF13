#include "stdafx.h"
#include "Effectt_2D_Bone.h"
#include "Bone.h"

CEffect_2D_Bone::CEffect_2D_Bone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect{ pDevice, pContext }
{
}

CEffect_2D_Bone::CEffect_2D_Bone(const CEffect& rhs)
	: CEffect{ rhs }
{
}

HRESULT CEffect_2D_Bone::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect_2D_Bone::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;


	EFFECT_2D_DESC* pDesc = (EFFECT_2D_DESC*)pArg;
	m_eEffect = pDesc->eEffect;
	m_tFrame = EffectImageFrame[m_eEffect];

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CEffect_2D_Bone::Tick(_float fTimeDelta)
{
}

HRESULT CEffect_2D_Bone::Late_Tick(_float fTimeDelta)
{
	return E_NOTIMPL;
}

HRESULT CEffect_2D_Bone::Render()
{
	return E_NOTIMPL;
}

void CEffect_2D_Bone::Start()
{
}

HRESULT CEffect_2D_Bone::Add_Components()
{
	return E_NOTIMPL;
}

CEffect_2D_Bone* CEffect_2D_Bone::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CEffect_2D_Bone::Clone(void* pArg)
{
	return nullptr;
}

void CEffect_2D_Bone::Free()
{
}
