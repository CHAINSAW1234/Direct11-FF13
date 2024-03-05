#include "stdafx.h"

#include "ImGUI_Manager.h"
#include "ImGuizmo.h"

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "Camera.h"

CImGUI_Manager* CImGUI_Manager::m_pInstance = nullptr;
CImGUI_Manager* CImGUI_Manager::Get_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (nullptr == m_pInstance)
	{
		m_pInstance = new CImGUI_Manager;
		m_pInstance->Initialize(pDevice, pContext);
	}

	return m_pInstance;
}

unsigned int  CImGUI_Manager::Destroy_Instance()
{
	unsigned int	iRefCnt = 0;
	if (nullptr != m_pInstance)
	{
		iRefCnt = m_pInstance->Release();
		if (0 == iRefCnt)
			m_pInstance = nullptr;
	}
	return iRefCnt;
}

HRESULT CImGUI_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pGameInstance = CGameInstance::Get_Instance();
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	return S_OK;
}


void CImGUI_Manager::Tick(_float fTimeDelta)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

/// <summary> ImGUI의 기능을 사용하기 전 Frame을 시작하는 단계 </summary>
/// <remarks> (23.12.18기준) Tick에서 Level생성후, RENDER로 가는 순서를 막고자 Late_Tick에서 실행 </remarks> 
void CImGUI_Manager::Late_Tick(_float fDeltaTime)
{

}

/// <summary> ImGUI의 기능을 사용하기 전 Frame을 마무리하는 단계 </summary>
HRESULT CImGUI_Manager::Render()
{
	// Rendering
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

void CImGUI_Manager::EditTransform(_float4x4& matrix)
{
#ifndef _DEBUG
	return;
#endif // !_DEBUG

	ImGui::Begin("Imguizmo");
	ImGuizmo::BeginFrame();
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	//if (ImGui::IsKeyPressed(90))
	if (CGameInstance::Get_Instance()->Get_KeyState(KEY_DOWN, DIK_Q))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	//if (ImGui::IsKeyPressed(69))
	if (CGameInstance::Get_Instance()->Get_KeyState(KEY_DOWN, DIK_E))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	//if (ImGui::IsKeyPressed(82)) // r Key
	if (CGameInstance::Get_Instance()->Get_KeyState(KEY_DOWN, DIK_R))
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&matrix.m[0][0], matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation);
	ImGui::InputFloat3("Rt", matrixRotation);
	ImGui::InputFloat3("Sc", matrixScale);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &matrix.m[0][0]);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	static bool useSnap(false);
	/*if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;*/
	ImGui::Checkbox("useSnap", &useSnap);
	ImGui::SameLine();
	//vec_t snap;
	_float3 snap;
	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		//snap = config.mSnapTranslation;
		ImGui::InputFloat3("Snap", &snap.x);
		break;
	case ImGuizmo::ROTATE:
		//snap = config.mSnapRotation;
		ImGui::InputFloat("Angle Snap", &snap.x);
		break;
	case ImGuizmo::SCALE:
		//snap = config.mSnapScale;
		ImGui::InputFloat("Scale Snap", &snap.x);
		break;
	}

	ImGuiIO& io = ImGui::GetIO();

	/*RECT rt;
	::GetClientRect(g_hWnd, &rt);
	POINT lt{ rt.left, rt.top };
	::ScreenToClient(g_hWnd, &lt);
	ImGuizmo::SetRect((_float)lt.x, (_float)lt.y, io.DisplaySize.x, io.DisplaySize.y);*/
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	_float4x4	ProjMatrix, ViewMatrix;
	
	ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
	ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
	ImGuizmo::Manipulate(&ViewMatrix.m[0][0], &ProjMatrix.m[0][0], mCurrentGizmoOperation, mCurrentGizmoMode, &matrix.m[0][0], NULL, useSnap ? &snap.x : NULL);

	ImGui::End();
}

void CImGUI_Manager::EditFilePath(_Out_ string& FilePath, _Out_ string& FilePathName)
{
	if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
		FilePathName = ImGuiFileDialog::Instance()->GetFilePathName();
		FilePath = ImGuiFileDialog::Instance()->GetCurrentPath();
		// action
	}
	else {
		FilePathName = "";
		FilePath = "";
	}
	// close
	ImGuiFileDialog::Instance()->Close();
}


void CImGUI_Manager::Free()
{
	__super::Free();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pGameInstance);

}

