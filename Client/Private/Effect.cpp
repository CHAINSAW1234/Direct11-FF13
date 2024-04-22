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
	GameObjectDesc.fSpeedPerSec = 1.f;
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
	Move(fTimeDelta);
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

void CEffect::Set_Move(_float4 vMoveDir)
{
	XMStoreFloat4(&m_vMoveDir, XMLoadFloat4(&vMoveDir));
}

void CEffect::Set_Turn(_float4 vTurnDir)
{
	XMStoreFloat4(&m_vTurnDir, XMLoadFloat4(&vTurnDir));
}

HRESULT CEffect::Save_Effect(ofstream& OFS)
{
	// 0. EFfect Type
	OFS.write(reinterpret_cast<const char*>(&m_eType), sizeof(TYPE));

	//1. EffectName
	size_t istrEffectNameSize = m_strEffectName.size();
	OFS.write(reinterpret_cast<const char*>(&istrEffectNameSize), sizeof(size_t));
	OFS.write(reinterpret_cast<const char*>(m_strEffectName.c_str()), sizeof(_char) * istrEffectNameSize);

	// 2. EffectTime
	OFS.write(reinterpret_cast<const char*>(&m_fEffectTimeDelta), sizeof(_float));

	// 3. Color
	OFS.write(reinterpret_cast<const char*>(&m_fColorMagnification), sizeof(_float));
	OFS.write(reinterpret_cast<const char*>(&m_vColor), sizeof(_float4));

	// 4. Texture
	OFS.write(reinterpret_cast<const char*>(&m_iDiffuseTextureIndex), sizeof(_float));
	OFS.write(reinterpret_cast<const char*>(&m_iMaskTextureIndex), sizeof(_float));
	OFS.write(reinterpret_cast<const char*>(&m_iDissolveTextureIndex), sizeof(_float));

	OFS.write(reinterpret_cast<const char*>(&m_fTextureMovementTimeDelta), sizeof(_float2));

	// 5. MaskValue
	OFS.write(reinterpret_cast<const char*>(&m_vMaskValue[0]), sizeof(_float4));
	OFS.write(reinterpret_cast<const char*>(&m_vMaskValue[1]), sizeof(_float4));

	// 6. Move
	OFS.write(reinterpret_cast<const char*>(&m_fMoveSpeed), sizeof(_float));
	OFS.write(reinterpret_cast<const char*>(&m_vMoveDir), sizeof(_float4));

	// 7. Turn
	OFS.write(reinterpret_cast<const char*>(&m_fTurnSpeed), sizeof(_float));
	OFS.write(reinterpret_cast<const char*>(&m_vTurnDir), sizeof(_float4));

	// 8. Lerp
	OFS.write(reinterpret_cast<const char*>(&m_vLerpScaleStart), sizeof(_float3));
	OFS.write(reinterpret_cast<const char*>(&m_vLerpScaleEnd), sizeof(_float3));

	return S_OK;
}

HRESULT CEffect::Load_Effect(ifstream& IFS)
{
	//1. EffectName
	size_t strEffectNameSize = 0;
	IFS.read(reinterpret_cast<char*>(&strEffectNameSize), sizeof(size_t));
	vector<_char> vecBuffer(strEffectNameSize);

	IFS.read(reinterpret_cast<char*>(vecBuffer.data()), sizeof(_char) * strEffectNameSize);
	m_strEffectName = string(vecBuffer.begin(), vecBuffer.end());

	// 2. EffectTime
	IFS.read(reinterpret_cast<char*>(&m_fEffectTimeDelta), sizeof(_float));

	// 3. Color
	IFS.read(reinterpret_cast<char*>(&m_fColorMagnification), sizeof(_float));
	IFS.read(reinterpret_cast<char*>(&m_vColor), sizeof(_float4));

	// 4. Texture
	IFS.read(reinterpret_cast<char*>(&m_iDiffuseTextureIndex), sizeof(_float));
	IFS.read(reinterpret_cast<char*>(&m_iMaskTextureIndex), sizeof(_float));
	IFS.read(reinterpret_cast<char*>(&m_iDissolveTextureIndex), sizeof(_float));

	IFS.read(reinterpret_cast<char*>(&m_fTextureMovementTimeDelta), sizeof(_float2));

	// 5. MaskValue
	IFS.read(reinterpret_cast<char*>(&m_vMaskValue[0]), sizeof(_float4));
	IFS.read(reinterpret_cast<char*>(&m_vMaskValue[1]), sizeof(_float4));

	//// 6. Move
	IFS.read(reinterpret_cast<char*>(&m_fMoveSpeed), sizeof(_float));
	IFS.read(reinterpret_cast<char*>(&m_vMoveDir), sizeof(_float4));

	// 7. Turn
	IFS.read(reinterpret_cast<char*>(&m_fTurnSpeed), sizeof(_float));
	IFS.read(reinterpret_cast<char*>(&m_vTurnDir), sizeof(_float4));

	// 8. Lerp
	IFS.read(reinterpret_cast<char*>(&m_vLerpScaleStart), sizeof(_float3));
	IFS.read(reinterpret_cast<char*>(&m_vLerpScaleEnd), sizeof(_float3));

	return S_OK;
}

void CEffect::Reset_Effect()
{
	m_fTimeDelta = 0.f;
	m_fTextureTimeDelta = { 0.f, 0.f };
	m_fDissolveTimeDelta = { 0.f };
	m_pTransformCom->Set_Scaled(1.f, 1.f, 1.f);

}

void CEffect::Reset_Position()
{
	//_float4 vPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
	m_pTransformCom->Set_WorldMatrix(XMMatrixIdentity());
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
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
	_vector vTurnDir = XMVectorSetW(XMVector3Normalize(XMLoadFloat4(&m_vTurnDir)), 0.f);
	if (XMVector3Equal(vTurnDir, XMVectorSet(0.f, 0.f, 0.f, 0.f)))
		return;

	m_pTransformCom->Turn(vTurnDir, fTimeDelta * m_fTurnSpeed);
}

void CEffect::Move(_float fTimeDelta)
{
	_vector vMoveDir = XMVectorSetW(XMVector3Normalize(XMLoadFloat4(&m_vMoveDir)), 0.f);
	if (XMVector3Equal(vMoveDir, XMVectorSet(0.f, 0.f, 0.f, 0.f)))
		return;

	m_pTransformCom->Move_To_Direction(vMoveDir, fTimeDelta * m_fMoveSpeed);

}

void CEffect::Lerp()
{
	_vector vScale = XMVectorLerp(XMLoadFloat3(&m_vLerpScaleStart), XMLoadFloat3(&m_vLerpScaleEnd), m_fTimeDelta / m_fEffectTimeDelta);
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
