#include "stdafx.h"
#include "ParsingTool.h"
#include "ImGUI_Manager.h"
#include "ImGuiFileDialog.h"
#include "Model.h"

CParsingTool::CParsingTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CParsingTool::CParsingTool(const CParsingTool& rhs)
    :CGameObject{ rhs }
{
}

HRESULT CParsingTool::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CParsingTool::Initialize(void* pArg)
{
    m_eLevel = g_Level;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pImGUI_Manager = CImGUI_Manager::Get_Instance(m_pDevice, m_pContext);

}

void CParsingTool::Tick(_float fTimeDelta)
{
    m_pImGUI_Manager->Tick(fTimeDelta);

    ModelList_Window();
}

HRESULT CParsingTool::Late_Tick(_float fTimeDelta)
{
    m_pImGUI_Manager->Late_Tick(fTimeDelta);

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);

    return S_OK;
}

HRESULT CParsingTool::Render()
{
    m_pImGUI_Manager->Render();

    return S_OK;
}

void CParsingTool::ModelList_Window()
{
    ImGui::SetWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetWindowSize(ImVec2(400.f, 600.f));

    if (show_ModelList_window) {
        ImGui::Begin("Model_List", &show_ModelList_window, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginListBox("MapObjects List"))
        {
            for (size_t i = 0; i < m_Models.size(); ++i)
            {
                const bool is_selected = (m_iCurrent_Model_Index == i);
                char name[26];
                sprintf_s(name, "MapObject %d", i);
                if (ImGui::Selectable(name, is_selected)) {
                    m_iCurrent_Model_Index = i;
                }

            }

            ImGui::EndListBox();
        }

        if (ImGui::Button("Add")) {
            IGFD::FileDialogConfig config;
            config.path = "../Bin/Resources/";
            config.countSelectionMax = 1;
            config.flags = ImGuiFileDialogFlags_Modal;
            ImGuiFileDialog::Instance()->OpenDialog("Choose Model", "Choose File", ".fbx,.*", config);
        }

        if (ImGui::Button("Delete")) {
            if (m_Models.size() > m_iCurrent_Model_Index) {
                Safe_Release(m_Models[m_iCurrent_Model_Index]);
                m_Models.erase(m_Models.begin() + m_iCurrent_Model_Index);
                m_iCurrent_Model_Index = INFINITE;
            }
        }

        ImGui::NewLine();
        if (ImGui::Button("Change Path")) {
            IGFD::FileDialogConfig config;
            config.path = "../Bin/Resources/";
            config.countSelectionMax = 1;
            config.flags = ImGuiFileDialogFlags_Modal;
            ImGuiFileDialog::Instance()->OpenDialog("Choose Model", "Choose FileName", ".fbx,.*", config);
        }

        if (ImGui::Button("Parse All")) {
            Parse_All();
        }


    }

}

void CParsingTool::Parse_All()
{
}

CParsingTool* CParsingTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CParsingTool* pInstance = new CParsingTool(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Initialize : CParsingTool"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParsingTool::Clone(void* pArg)
{
    CParsingTool* pInstance = new CParsingTool(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CParsingTool"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParsingTool::Free()
{
    __super::Free();
    Safe_Release(m_pImGUI_Manager);
}