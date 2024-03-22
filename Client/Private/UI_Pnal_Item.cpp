#include "stdafx.h"
#include "UI_Pnal_Item.h"
#include "Player_Battle.h"

CUI_Pnal_Item::CUI_Pnal_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI_Pnal{ pDevice, pContext }
{
}

CUI_Pnal_Item::CUI_Pnal_Item(const CUI_Pnal_Item& rhs)
	: CUI_Pnal{ rhs }
{
}

HRESULT CUI_Pnal_Item::Initialize_Prototype()
{
	m_tFrame = { 16, 0.05f, 0, 0.f, 0 };
	m_vColor = { 0.f,1.f,0.f,1.f };
	return S_OK;
}

HRESULT CUI_Pnal_Item::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	UI_PNAL_ITEM_DESC* UI_Pnal_Attack_desc = (UI_PNAL_ITEM_DESC*)pArg;

	m_eItem = UI_Pnal_Attack_desc->eItem;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	//m_pPlayerInfo->RegisterObserver(this); // 이거 사실상 UI 아님 

	return S_OK;
}

void CUI_Pnal_Item::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CUI_Pnal_Item::Late_Tick(_float fTimeDelta)
{
	if (m_isDead && !m_isAnimated)
		return E_FAIL;

	if (m_isRender) {
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}

	return S_OK;
}

HRESULT CUI_Pnal_Item::Render()
{
	if (m_isDead) {
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(4)))		//Disapper
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
		return S_OK;
	}
	else {
		__super::Render();
	}

	return S_OK;
}

void CUI_Pnal_Item::Start()
{
	// 이 부분 수정 들어가야됨
	m_fSizeX = 128 * 2;
	m_fSizeY = 30;
	m_fMaskMovement = Random_Float(10);
	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vStartPosition), 1.f));

}

HRESULT CUI_Pnal_Item::Add_Components()
{
	if (FAILED(__super::Add_Components()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Pnal_Item::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fMoveTimeDelta, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CUI_Pnal_Item::Move(_float fTimeDelta)
{
	m_fMoveTimeDelta += fTimeDelta;
	if (m_fMoveTimeDelta >= 1.0f) {
		m_fMoveTimeDelta = 1.0f;
	}

	_float4 vCurPos = {};
	XMStoreFloat4(&vCurPos, XMVectorSetW(XMVectorLerp(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION), XMLoadFloat3(&m_vTargetPosition), m_fMoveTimeDelta), 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurPos);

}

CUI_Pnal_Item* CUI_Pnal_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Pnal_Item* pInstance = new CUI_Pnal_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Pnal_Item"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CUI_Pnal_Item* CUI_Pnal_Item::Clone(void* pArg)
{
	CUI_Pnal_Item* pInstance = new CUI_Pnal_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Pnal_Item"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Pnal_Item::Free()
{
	__super::Free();
}
