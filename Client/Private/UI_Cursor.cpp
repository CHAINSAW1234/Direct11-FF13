#include "stdafx.h"
#include "UI_Cursor.h"
#include "Player_Battle.h"

CUI_Cursor::CUI_Cursor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Cursor::CUI_Cursor(const CUI_Cursor& rhs)
	: CUI{rhs}
{
}

void CUI_Cursor::Set_Position(_float4 vPosition)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_vOriginPosition = vPosition;
}

HRESULT CUI_Cursor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Cursor::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	UI_DESC* pUI_Desc = (UI_DESC*)pArg;
	m_pPlayerInfo = (CPlayer_Battle*)pUI_Desc->pObserver_Hander;
	if (nullptr == m_pPlayerInfo)	// �̺κ� ����
		return E_FAIL;
	Safe_AddRef(m_pPlayerInfo);


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pPlayerInfo->RegisterObserver(this);
	
	return S_OK;
}

void CUI_Cursor::Tick(_float fTimeDelta)
{
	m_fDegree += fTimeDelta * 360;
	if (m_fDegree >= 360) {
		m_fDegree -= 360;
	}
	_float4 vPosition = m_vOriginPosition;
	vPosition.x += sin(XMConvertToRadians((_float)m_fDegree)) * 5;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

}

HRESULT CUI_Cursor::Late_Tick(_float fTimeDelta)
{
	if (FAILED(CGameObject::Late_Tick(fTimeDelta)))
		return E_FAIL;

	if (m_isRender) {
		m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}

	return S_OK;
}

HRESULT CUI_Cursor::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	/* �� �Լ� ���ο��� ȣ��Ǵ� Apply�Լ� ȣ�� ������ ���̴� ������ �������� ��� �����͸� �� �������Ѵ�. */

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_Cursor::Start()
{
	m_fSizeX = 56;
	m_fSizeY = 30;
	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_vOriginPosition = m_pTransformCom->Get_State_Float4(CTransform::STATE_POSITION);
}

void CUI_Cursor::OnNotify()
{
	if (m_pPlayerInfo->Get_Stage() == CPlayer_Battle::STAGE_WAIT ||
		m_pPlayerInfo->Get_Stage() == CPlayer_Battle::STAGE_FINISH)
		m_isRender = false;
	else
		m_isRender = true;

	_float3 vOriginPosition = m_pPlayerInfo->Get_CursorPosition();
	m_vOriginPosition = { vOriginPosition.x, vOriginPosition.y , vOriginPosition.z , 1.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vOriginPosition);
}

HRESULT CUI_Cursor::Add_Components()
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
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_Cursor"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Cursor::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);

	return S_OK;
}

CUI_Cursor* CUI_Cursor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Cursor* pInstance = new CUI_Cursor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Cursor"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CUI_Cursor* CUI_Cursor::Clone(void* pArg)
{
	CUI_Cursor* pInstance = new CUI_Cursor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Cursor"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Cursor::Free()
{
	__super::Free();
	Safe_Release(m_pPlayerInfo);
}
