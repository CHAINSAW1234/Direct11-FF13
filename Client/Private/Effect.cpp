#include "stdafx.h"
#include "Effect.h"

CEffect::CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEffect::CEffect(const CEffect& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CEffect::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffect::Initialize(void* pArg)
{
	m_eLevel = g_Level;

	Effect_Desc* pDesc = (Effect_Desc*) pArg;

	m_vColor = pDesc->vColor;
	m_strEffectName = pDesc->strEffectName;

	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CEffect::Tick(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;
	if (m_fTimeDelta >= m_fEffectTimeDelta) {
		m_fTimeDelta = m_fEffectTimeDelta;
		Reset_Effect();
	}

	Update_Texture_Movement(fTimeDelta);
	Turn(fTimeDelta);
	Lerp();
}

HRESULT CEffect::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect::Render()
{
	return S_OK;
}

void CEffect::Start()
{
}

void CEffect::Set_Turn(_float4 vTurnDir)
{
	XMStoreFloat4(&m_vTurnDir, XMVectorSetW(XMVector3Normalize(XMLoadFloat4(&vTurnDir)), 0.f));
}

void CEffect::Reset_Effect()
{
	m_fTimeDelta = 0.f;
	m_fTextureTimeDelta = { 0.f, 0.f };
	m_fDissolveTimeDelta = { 0.f };
	m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);

}

void CEffect::Update_Texture_Movement(_float fTimeDelta)
{
	m_fTextureTimeDelta = { m_fTextureTimeDelta.x + m_fTextureMovementTimeDelta.x * fTimeDelta / m_fEffectTimeDelta,
							m_fTextureTimeDelta.y + m_fTextureMovementTimeDelta.y * fTimeDelta / m_fEffectTimeDelta };

	m_fDissolveTimeDelta += fTimeDelta / m_fEffectTimeDelta;
	if (m_fDissolveTimeDelta >= 1.f)
		m_fDissolveTimeDelta = 1.1f;
}

void CEffect::Turn(_float fTimeDelta)
{
	if (XMVector3Equal(XMLoadFloat4(&m_vTurnDir), XMVectorSet(0.f, 0.f, 0.f, 0.f)))
		return;

	m_pTransformCom->Turn(XMLoadFloat4(&m_vTurnDir), fTimeDelta * m_fTurnSpeed);
}

void CEffect::Lerp()
{
	_vector vScale = XMVectorLerp(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMLoadFloat3(&m_vLerpScale), m_fTimeDelta / m_fEffectTimeDelta);
	m_pTransformCom->Set_Scaled(vScale.m128_f32[0], vScale.m128_f32[1], vScale.m128_f32[2] );
}

void CEffect::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDiffuseTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pDissolveTextureCom);
}
