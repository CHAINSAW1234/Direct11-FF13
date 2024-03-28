#include "stdafx.h"
#include "UI_Chr.h"
#include "Chr_Battle.h"

CUI_Chr::CUI_Chr(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI{ pDevice, pContext }
{
}

CUI_Chr::CUI_Chr(const CUI& rhs)
	:CUI{ rhs }
{
}

HRESULT CUI_Chr::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUI_Chr::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	UI_CHR_DESC* pUI_Chr_Desc = (UI_CHR_DESC*)pArg;
	m_vStartPosition = pUI_Chr_Desc->vStartPosition;
	m_vTargetPosition = pUI_Chr_Desc->vTargetPosition;
	m_pChr_Battle = pUI_Chr_Desc->pChr_Battle;
	if (nullptr == m_pChr_Battle)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	Safe_AddRef(m_pChr_Battle);
	m_pChr_Battle->RegisterObserver(this);

	return S_OK;
}

void CUI_Chr::Tick(_float fTimeDelta)
{
	Update_Hp(fTimeDelta);	// 체력 선형 보간, 비율까지 보두 계산
	Move(fTimeDelta);
	m_fGradMovement += fTimeDelta * (20 * (1-m_fRatio));

}

HRESULT CUI_Chr::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Chr::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Bind_Buffers()))
		return E_FAIL;

	/* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이터를 다 던져야한다. */

	if (FAILED(m_pShaderCom->Begin(6)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (m_fRatio < 0.6666) {
		if (FAILED(m_pShaderCom->Begin(8)))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pShaderCom->Begin(7)))
			return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	if (FAILED(Render_Name()))
		return E_FAIL;

	if (FAILED(Render_Role()))
		return E_FAIL;

	if (FAILED(Render_Hp()))
		return E_FAIL;

	return S_OK;
}

void CUI_Chr::Start()
{
	m_iHp =  m_iStartHp = m_iCurHp = m_pChr_Battle->Get_MaxHp();
	m_iMaxHp = m_pChr_Battle->Get_MaxHp();
	m_fRatio = 1;

	m_fSizeX = 300;
	m_fSizeY = 5;
	m_fGradMovement = 0.f;
	m_fHpLerpTimeDelta = 0.f;
	m_fMoveTimeDelta = 0.f;
	m_pTransformCom->Set_Scaled(m_fSizeX, m_fSizeY, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vStartPosition), 1.f));
}

void CUI_Chr::OnNotify()
{
	m_iHp = m_pChr_Battle->Get_Hp();
	if (m_iHp != m_iCurHp) {
		m_iStartHp = m_iCurHp;
		m_fHpLerpTimeDelta = 0.f;
	}
}

HRESULT CUI_Chr::Bind_ShaderResources()
{
	if (FAILED(__super::Bind_ShaderResources()))
		return E_FAIL;

	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0);
	m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture_Mask", 0);

	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeX", &m_fSizeX, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_SizeY", &m_fSizeY, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_vColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_Ratio", &m_fRatio, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_MaskMovement", &m_fGradMovement, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Chr::Add_Components()
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
	if (FAILED(__super::Add_Component(g_Level, TEXT("Prototype_Component_Texture_UI_Grad"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Chr::Render_Name()
{
	wstring strName = m_pChr_Battle->Get_Name();

	if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, strName, { m_vFont_NamePosition.x - 1, m_vFont_NamePosition.y - 1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, strName, { m_vFont_NamePosition.x - 1, m_vFont_NamePosition.y + 1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, strName, { m_vFont_NamePosition.x + 1, m_vFont_NamePosition.y - 1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, strName, { m_vFont_NamePosition.x + 1, m_vFont_NamePosition.y + 1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Font(g_strFont14Tag, strName, m_vFont_NamePosition, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f)))
		return E_FAIL;


	return S_OK;
}

HRESULT CUI_Chr::Render_Role()
{
	CAbility::ROLE eRole = m_pChr_Battle->Get_Ability()->Get_CurrentRole();
	wstring strName = CAbility::Get_RoleName(eRole);
	_float4 vColorName = CAbility::Get_RoleColor(eRole);

	if (FAILED(m_pGameInstance->Render_Font(g_strFontAlphaTag, strName,  m_vFont_RolePosition, XMLoadFloat4(&vColorName), 0.f)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI_Chr::Render_Hp()
{

	wstring strHp = to_wstring(m_iCurHp);

	if (FAILED(m_pGameInstance->Render_Font(g_strFont10Tag, TEXT("HP"), { m_vFont_HpPosition.x - 1, m_vFont_HpPosition.y - 1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Render_Font(g_strFont10Tag, TEXT("HP"), { m_vFont_HpPosition.x - 1, m_vFont_HpPosition.y - 1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Render_Font(g_strFont10Tag, TEXT("HP"), { m_vFont_HpPosition.x + 1, m_vFont_HpPosition.y + 1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Render_Font(g_strFont10Tag, TEXT("HP"), { m_vFont_HpPosition.x + 1, m_vFont_HpPosition.y + 1 }, XMVectorSet(0.f, 0.f, 0.f, 1.f), 0.f)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Render_Font(g_strFont10Tag, TEXT("HP"), { m_vFont_HpPosition.x, m_vFont_HpPosition.y }, XMVectorSet(0.f, 1.f, 0.f, 1.f), 0.f)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Font(g_strFontNum24Tag, strHp, { m_vFont_HpPosition.x + 23, m_vFont_HpPosition.y  }, XMVectorSet(1.f, 1.f, 1.f, 1.f), 0.f)))
		return E_FAIL;

	return S_OK;
}

void CUI_Chr::Update_Hp(_float fTimeDelta)
{
	if (m_fHpLerpTimeDelta >= 1.f) {
		return;
	}

	m_fHpLerpTimeDelta += fTimeDelta * 10;
	if (m_fHpLerpTimeDelta >= 1.f) {
		m_fHpLerpTimeDelta = 1.f;
	}

	// 선형 보간 
	m_iCurHp = m_iStartHp + (_int)round(m_fHpLerpTimeDelta * _float(m_iHp - m_iStartHp));
	m_fRatio = (_float)m_iCurHp / (_float)m_iMaxHp;

	// 색깔 결정
	m_vColor = { 0.f,1.f,0.f,1.f };
	if (m_fRatio <= 0.6666)
		m_vColor = {1.f,0.83f,0.f,1.f };
	if (m_fRatio <= 0.3)
		m_vColor = { 1.f,0.f,0.f,1.f };

}

void CUI_Chr::Move(_float fTimeDelta)
{
	if (m_fMoveTimeDelta >= 1.f) {
		return;
	}

	m_fMoveTimeDelta += fTimeDelta;
	if (m_fMoveTimeDelta >= 1.0f) {
		m_fMoveTimeDelta = 1.0f;
	}

	_float4 vCurPos = {};
	XMStoreFloat4(&vCurPos, XMVectorSetW(XMVectorLerp(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION), XMLoadFloat3(&m_vTargetPosition), m_fMoveTimeDelta), 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCurPos);

	// 폰트 위치 지정
	// 1. vCurPos -> WIndow 좌표계로 변경 직교투영이므로 winsize만 뺴면됨
	// { vPosition.x + (g_iWinSizeX - m_fSizeX) * 0.5f + 20 , -vPosition.y + 2 + (g_iWinSizeY - m_fSizeY) * 0.5f };

	vCurPos.x += g_iWinSizeX * 0.5f;
	vCurPos.y = -vCurPos.y + g_iWinSizeY * 0.5f -28;
	m_vFont_NamePosition = { vCurPos.x - m_fSizeX * 0.5f , vCurPos.y };
	m_vFont_RolePosition = { vCurPos.x - 40, vCurPos.y +5 };
	m_vFont_HpPosition = { vCurPos.x + 50, vCurPos.y + 5 };

}

CUI_Chr* CUI_Chr::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI_Chr* pInstance = new CUI_Chr(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Chr"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CUI_Chr* CUI_Chr::Clone(void* pArg)
{
	CUI_Chr* pInstance = new CUI_Chr(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Chr"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Chr::Free()
{
	__super::Free();
	
	Safe_Release(m_pTextureGradCom);
	
	Safe_Release(m_pChr_Battle);
}
