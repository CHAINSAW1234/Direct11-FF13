#include "stdafx.h"
#include "UI_Warning.h"
#include "Chr_Battle.h"

CUI_Warning::CUI_Warning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Warning::CUI_Warning(const CUI_Warning& rhs)
	: CUI{ rhs }
{
}

HRESULT CUI_Warning::Initialize_Prototype()
{
	m_isLateGroup = true;
	return S_OK;
}

HRESULT CUI_Warning::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));


	return S_OK;
}

void CUI_Warning::Tick(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta * 8;

	size_t iNum = m_pGameInstance->Get_LayerCnt(g_Level, g_strMonsterLayerTag);
	if (iNum <= 0) {
		Set_Dead(true);
		return;
	}

	Check_Hp();
}

HRESULT CUI_Warning::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Warning::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(12)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CUI_Warning::Start()
{
}

HRESULT CUI_Warning::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_Warning"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Warning::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_fColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskMovement", &m_fTimeDelta, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CUI_Warning::Check_Hp()
{
	size_t iNum = m_pGameInstance->Get_LayerCnt(g_Level, g_strChrLayerTag);
	for (size_t i = 0; i < iNum; ++i) {
		_int iHp = dynamic_cast<CChr_Battle*>(m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, (_uint)i))->Get_Hp();
		_int iMaxHp = dynamic_cast<CChr_Battle*>(m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, (_uint)i))->Get_MaxHp();
		if ((_float)iHp / (_float)iMaxHp < 0.3333f) {
			m_isRender = true;
			return;
		}
	}
	m_isRender = false;

}

CUI_Warning* CUI_Warning::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Warning* pInstance = new CUI_Warning(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Warning"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CUI* CUI_Warning::Clone(void* pArg)
{
	CUI_Warning* pInstance = new CUI_Warning(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Warning"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Warning::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
