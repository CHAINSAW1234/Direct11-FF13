#include "stdafx.h"
#include "UI_Pnal.h"
#include "Player_Battle.h"

CUI_Pnal::CUI_Pnal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI{ pDevice, pContext }
{
}

CUI_Pnal::CUI_Pnal(const CUI_Pnal& rhs)
	: CUI{ rhs }
	, m_tFrame{rhs.m_tFrame }
{
}

void CUI_Pnal::Set_TargetPosition(_bool isAnimated, _float3 vTargetPosition)
{
	m_vTargetPosition = vTargetPosition;

	m_isAnimated = isAnimated;
	if (m_isAnimated) {
		m_fMoveTimeDelta = 0.f;
		m_tFrame.iCurFrame = 0;
	}

}

void CUI_Pnal::Reset_Position()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vStartPosition), 1.f));
	m_isAnimated = true;
	m_fMoveTimeDelta = 0.f;
	m_tFrame.iCurFrame = 0;
}

HRESULT CUI_Pnal::Initialize_Prototype()
{
	m_tFrame = { 16, 0.05f, 0, 0.f, 0 };

	return S_OK;
}

HRESULT CUI_Pnal::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	UI_PNAL_DESC* UI_Pnal_desc = (UI_PNAL_DESC*)pArg;

	m_strName = UI_Pnal_desc->strName;
	m_vStartPosition = UI_Pnal_desc->vStartPosition;
	m_vTargetPosition = UI_Pnal_desc->vTargetPosition;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	//m_pPlayerInfo->RegisterObserver(this); // 이거 사실상 UI 아님 

	return S_OK;
}

void CUI_Pnal::Tick(_float fTimeDelta)
{
	m_fMaskMovement += fTimeDelta / 10;

	Update_Frame(fTimeDelta);
	Move(fTimeDelta);
}

HRESULT CUI_Pnal::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Pnal::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이터를 다 던져야한다. */

	if (m_isAnimated) {
		if (FAILED(m_pShaderCom->Begin(0)))		//default
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pShaderCom->Begin(1)))		// inner
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(3)))		// border
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	return S_OK;
}

void CUI_Pnal::Start()
{
	// 이 부분 수정 들어가야됨
	m_fSizeX = 128 * 1.3f;
	m_fSizeY = 22;
	m_fMaskMovement = Random_Float(10);
	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vStartPosition), 1.f));
}

HRESULT CUI_Pnal::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture_Inner */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_Pnal_Inner"),
		TEXT("Com_Texture_Inner"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Texture_Border */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_Pnal_Border"),
		TEXT("Com_Texture_Border"), (CComponent**)&m_pTextureBorderCom)))
		return E_FAIL;

	/* For.Com_Texture_UV */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_ATB_Mask"),
		TEXT("Com_Texture_UV"), (CComponent**)&m_pTextureMaskCom)))
		return E_FAIL;

	/* For.Com_Texture_Inner */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_Pnal_Anim"),
		TEXT("Com_Texture_Anim"), (CComponent**)&m_pTextureAnimationCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Pnal::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeX", &m_fSizeX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeY", &m_fSizeY, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskMovement", &m_fMaskMovement, sizeof(_float))))
		return E_FAIL;

	if (m_isAnimated) {
		m_pTextureAnimationCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_tFrame.iCurFrame);
	}
	else {
		m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);
		m_pTextureBorderCom->Bind_ShaderResource(m_pShaderCom, "g_Texture_Border", 0);
		m_pTextureMaskCom->Bind_ShaderResource(m_pShaderCom, "g_Texture_Mask", 0);
	}
	
	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	return S_OK;
}

void CUI_Pnal::Update_Frame(_float fTimeDelta)
{
	if (!m_isAnimated)
		return;
	
	m_tFrame.fTime += fTimeDelta;

	if (m_tFrame.fTime > m_tFrame.fFrameSpeed)
	{
		m_tFrame.fTime = 0.f;
		++m_tFrame.iCurFrame;

		if (m_tFrame.iCurFrame >= m_tFrame.iMaxFrame) {
			m_isAnimated = false;
			m_tFrame.iCurFrame = 0;
		}
	}
}

void CUI_Pnal::Move(_float fTimeDelta)
{
	if (!m_isAnimated)
		return;

	m_fMoveTimeDelta += fTimeDelta;
	if (m_fMoveTimeDelta >= 1.0f) {
		m_fMoveTimeDelta = 1.0f;
	}

	_float4 vCurPos = {};
	XMStoreFloat4(&vCurPos, XMVectorSetW(XMVectorLerp(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION), XMLoadFloat3(&m_vTargetPosition), m_fMoveTimeDelta), 1.f));
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurPos);
}

CUI_Pnal* CUI_Pnal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Pnal* pInstance = new CUI_Pnal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Pnal"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CUI_Pnal* CUI_Pnal::Clone(void* pArg)
{
	CUI_Pnal* pInstance = new CUI_Pnal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Pnal"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Pnal::Free()
{
	__super::Free();
	Safe_Release(m_pTextureAnimationCom);
	Safe_Release(m_pTextureBorderCom);
	Safe_Release(m_pTextureMaskCom);
}
