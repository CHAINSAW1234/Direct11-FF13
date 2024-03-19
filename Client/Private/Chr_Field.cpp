#include "stdafx.h"
#include "Chr_Field.h"

#include "FSM.h"
#include "Model.h"
#include "Shader.h"

#include "Chr_Field_State_Idle.h"
#include "Chr_Field_State_Move.h"
#include "Chr_Field_State_Walk.h"
#include "Chr_Field_State_Battle_Begin.h"
#include "Chr_Field_State_Item.h"

#include "ImGUI_Manager.h"

CChr_Field::CChr_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CChr_Field::CChr_Field(const CChr_Field& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CChr_Field::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CChr_Field::Initialize(void* pArg)
{
	m_eLevel = g_Level;

	GAMEOBJECT_DESC		GameObjectDesc{};

	GameObjectDesc.fSpeedPerSec = 10.f;
	GameObjectDesc.fRotationPerSec = XMConvertToRadians(360.f);

	if (FAILED(__super::Initialize(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pImGUI_Manager = CImGUI_Manager::Get_Instance(m_pDevice, m_pContext);


	m_pModelCom->Set_Animation(0, false);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(_float(rand() % 20), 2.f, _float(rand() % 20), 1.f));

	return S_OK;
}

void CChr_Field::Tick(_float fTimeDelta)
{
	m_pFSMCom->Update(fTimeDelta);
	Update_FSMState(fTimeDelta);

	m_pImGUI_Manager->Tick(fTimeDelta);
	Show_ImGUI();
}

HRESULT CChr_Field::Late_Tick(_float fTimeDelta)
{
	if (FAILED(__super::Late_Tick(fTimeDelta)))
		return E_FAIL;

	m_pModelCom->Play_Animation(fTimeDelta);

	// �ӽ�
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
	return S_OK;
}

HRESULT CChr_Field::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i) {
		if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		/* �� �Լ� ���ο��� ȣ��Ǵ� Apply�Լ� ȣ�� ������ ���̴� ������ �������� ��� �����͸� �� �������Ѵ�. */
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;

		m_pModelCom->Render(i);
	}

	m_pImGUI_Manager->Render();

	return S_OK;
}

HRESULT CChr_Field::Change_State(STATE eState)
{
	m_eState = eState;
	m_pFSMCom->Change_State(eState);

	return S_OK;
}

void CChr_Field::Change_Animation(ANIMATION_CHR_FIELD iAnimationIndex, _bool isLoop)
{
	m_pModelCom->Set_Animation(iAnimationIndex, isLoop);
}

_float4 CChr_Field::Cal_Target_Direction()
{
	_vector vTargetDir = { 0.f,0.f,0.f,0.f };

	// 1. ī�޶��� Look, Right ���� ���Ѵ�
	_vector vCamRight, vCamLook;
	vCamRight = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW).r[0];
	vCamLook = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW).r[2];

	// 2. Ű �Է��� �������� �̵� ���� ���� 
	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_W))
		vTargetDir += vCamLook;
	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_S))
		vTargetDir -= vCamLook;

	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_A))
		vTargetDir -= vCamRight;
	if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_D))
		vTargetDir += vCamRight;

	// 3. y�� �����
	vTargetDir.m128_f32[1] = 0;

	// 4. ����ȭ�� �̵� ���� ���� 
	vTargetDir = XMVector3Normalize(vTargetDir);

	_float4 vOutputDir = { 0.f,0.f,0.f,0.f };

	XMStoreFloat4(&vOutputDir, vTargetDir);

	return vOutputDir;
}

_float4 CChr_Field::Get_Look()
{
	// Player�� Look vector�� Y���� ����� ����
	_float4 vChrLook = m_pTransformCom->Get_State_Float4(CTransform::STATE_LOOK);
	vChrLook.y = 0;
	XMStoreFloat4(&vChrLook, XMVectorSetW(XMVector3Normalize(XMLoadFloat4(&vChrLook)), 0.f));

	return vChrLook;
}

HRESULT CChr_Field::Add_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Model_Light_Field"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if(FAILED(Add_Component_FSM()))
		return E_FAIL;

	return S_OK;
}

HRESULT CChr_Field::Add_Component_FSM()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		TEXT("Com_FSM"), (CComponent**)&m_pFSMCom)))
		return E_FAIL;

	m_pFSMCom->Add_State(IDLE, CChr_Field_State_Idle::Create(this));
	m_pFSMCom->Add_State(MOVE, CChr_Field_State_Move::Create(this));
	m_pFSMCom->Add_State(WALK, CChr_Field_State_Walk::Create(this));
	m_pFSMCom->Add_State(ITEM, CChr_Field_State_Item::Create(this));
	m_pFSMCom->Add_State(BATTLE_BEGIN, CChr_Field_State_Battle_Begin::Create(this));
	Change_State(IDLE);

	return S_OK;
}

HRESULT CChr_Field::Bind_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CamPosition_Float4(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

void CChr_Field::Update_FSMState(_float fTimeDelta)
{

	// ������ ���ɼ��� �ſ� ���� �ڵ�?
	if (!m_isControl)
		return;

	switch (m_eState) {
	case IDLE:
		// 1. IDLE to WALK
		if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_W) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_A) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_S) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_D))
			Change_State(WALK);
		// 2. IDLE to MOVE
		if (m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_LSHIFT) && (
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_W) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_A) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_S) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_D)))
			Change_State(MOVE);
		break;
	case WALK:
		// 1. WALK to MOVE
		if(m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_LSHIFT))
			Change_State(MOVE);
		// 2. WALK to IDLE
		if (!(m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_W) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_A) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_S) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_D)))
			Change_State(IDLE);
		break;
	case MOVE:
		// 1. MOVE to IDLE
		if (!m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_LSHIFT))
			Change_State(WALK);
		// 2. MOVE to IDLE
		if (!(m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_W) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_A) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_S) ||
			m_pGameInstance->Get_KeyState(KEY_PRESS, DIK_D)))
			Change_State(IDLE);
		break;
	}
}

void CChr_Field::Show_ImGUI()
{
	_float4x4 worldmatrix = m_pTransformCom->Get_WorldFloat4x4();
	_float vRight[4] = { worldmatrix.m[0][0], worldmatrix.m[0][1] , worldmatrix.m[0][2] , worldmatrix.m[0][3] };
	_float vUp[4] = { worldmatrix.m[1][0], worldmatrix.m[1][1] , worldmatrix.m[1][2] , worldmatrix.m[1][3] };
	_float vLook[4] = { worldmatrix.m[2][0], worldmatrix.m[2][1] , worldmatrix.m[2][2] , worldmatrix.m[2][3] };
	_float vPos[4] = { worldmatrix.m[3][0], worldmatrix.m[3][1] , worldmatrix.m[3][2] , worldmatrix.m[3][3] };

	string str;
	switch (m_eState) {
	case IDLE:
		str = "IDLE";
		break;
	case WALK:
		str = "WALK";
		break;
	case MOVE:
		str = "MOVE";
		break;
	case ITEM:
		str = "ITEM";
		break;
	case BATTLE_BEGIN:
		str = "BATTLE_BEGIN";
		break;
	}

	ImGui::Begin("Chr_Field Tool");
	if (ImGui::TreeNode("Transform")) {
		ImGui::InputFloat4("Right", vRight);
		ImGui::InputFloat4("Up", vUp);
		ImGui::InputFloat4("Look", vLook);
		ImGui::InputFloat4("Pos", vPos);

		ImGui::TreePop();
	}
	ImGui::Text("Current State : ");
	ImGui::SameLine();
	ImGui::Text(str.c_str());
	ImGui::Text("Animation Index : %d", m_pModelCom->Get_CurrentAnimationIndex());
	ImGui::Text("Animation Frame : %f", m_pModelCom->Get_CurrentTrackPosition());

	ImGui::End();
}

CChr_Field* CChr_Field::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CChr_Field* pInstance = new CChr_Field(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CChr_Field"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CChr_Field::Clone(void* pArg)
{
	CChr_Field* pInstance = new CChr_Field(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CChr_Field"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChr_Field::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pFSMCom);

	CImGUI_Manager::Destroy_Instance();
}
