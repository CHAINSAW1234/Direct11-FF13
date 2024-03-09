#include "stdafx.h"

#include "Texture.h"

#include "MapTool.h"
#include "ImGUI_Manager.h"

#include "Camera_Free.h"

#include "MapObject.h"
#include "Model.h"
#include "Mesh.h"

#include "ImGuiFileDialog.h"

CMapTool::CMapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMapTool::CMapTool(const CMapTool& rhs)
    :CGameObject{ rhs }
{
}

HRESULT CMapTool::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMapTool::Initialize(void* pArg)
{
    m_eLevel = g_Level;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pImGUI_Manager = CImGUI_Manager::Get_Instance(m_pDevice, m_pContext);

    m_pCamera = dynamic_cast<CCamera_Free*>(m_pGameInstance->Get_GameObject(m_eLevel, TEXT("Layer_Camera"), 0));
    Safe_AddRef(m_pCamera);

    Update_MapList();

    return S_OK;
}

void CMapTool::Tick(_float fTimeDelta)
{
    Update_KeyInput();

    m_pImGUI_Manager->Tick(fTimeDelta);

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);


    WindowList_Window(fTimeDelta);

    ModelCreate_Window();

    ModelList_Window();

    MapList_Window();

    Show_Picking_ImGUI();


}

HRESULT CMapTool::Late_Tick(_float fTimeDelta)
{
    m_pImGUI_Manager->Late_Tick(fTimeDelta);

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);

    return S_OK;
}

HRESULT CMapTool::Render()
{
    m_pImGUI_Manager->Render();

    return S_OK;
}

void CMapTool::Set_PickingTarget()
{
    for (auto& pMapObject : m_MapObjects) {
        if (pMapObject->Compute_Picking())
            m_pTargetObject = pMapObject;
    }
}

void CMapTool::Show_Picking_ImGUI()
{
    if (nullptr != m_pTargetObject) {
        m_pImGUI_Manager->EditTransform(((CTransform*)m_pTargetObject->Get_Component(g_strTransformTag))->Get_WorldFloat4x4());
    }

}

HRESULT CMapTool::Add_Components()
{
    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, TEXT("Prototype_Component_Texture_PrevMapObject"),
        TEXT("Com_Texture"), (CComponent**)&m_PrevTextures)))
        return E_FAIL;

    m_strModelTags.push_back(TEXT("Prototype_Component_Model_Map_Field"));
    m_strModelTags.push_back(TEXT("Prototype_Component_Model_Map_Battle"));
    m_strModelTags.push_back(TEXT("Prototype_Component_Model_Map_BossBattle_1"));
    m_strModelTags.push_back(TEXT("Prototype_Component_Model_Map_BossBattle_2"));

    return S_OK;
}


void CMapTool::Update_KeyInput()
{
    if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_ESCAPE)) {
        m_pTargetObject = nullptr;
    }

    if (m_isEnablePicking && m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON)) {
        Set_PickingTarget();
    }
}

HRESULT CMapTool::Update_MapList()
{
    m_MapList.clear();

    WIN32_FIND_DATAA data;
    string		szFilePath  = "../Bin/Resources/Map/*";
    HANDLE hFind = FindFirstFileA(szFilePath.c_str(), &data);
    
    if (hFind == INVALID_HANDLE_VALUE)
        return E_FAIL;

    while (FindNextFileA(hFind, &data)) {
        if ((data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE &&
            !(data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)))
        {
            m_MapList.push_back(data.cFileName);
        }
    }

    return S_OK;
}

void CMapTool::WindowList_Window(_float fTimeDelta)
{
    static _bool is_Camera_Move = true;

    ImGui::SetWindowSize(ImVec2(200.f, 200.f));

    ImGui::Begin("Hello, World!");                          // Create a window called "Hello, world!" and append into it.
    
    if (ImGui::TreeNode("Window List")) {
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("ModelCreate Window", &show_ModelCreate_window);
        ImGui::Checkbox("ModelList Window", &show_ModelList_window);
        ImGui::Checkbox("MapList Window", &show_MapList_window);

        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Opotion List")) {
        ImGui::Checkbox("Camera Move", &is_Camera_Move);
        ImGui::Checkbox("Enable Model Picking", &m_isEnablePicking);

        ImGui::TreePop();
    }

    if (is_Camera_Move) {
        m_pCamera->Move_Camera(fTimeDelta);
    }

    //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
    //if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    //    counter++;

    ImGui::SameLine();
    ImGui::End();
}

void CMapTool::ModelCreate_Window()
{
    ImGui::SetWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetWindowSize(ImVec2(400.f, 600.f));

    if (show_ModelCreate_window) {

        ImGui::Begin("ModelCreate", &show_ModelCreate_window, ImGuiWindowFlags_MenuBar);
        ImVec2 size = ImVec2(80.f, 80.f);

        for (size_t i = 0; i < m_PrevTextures->Get_NumTextures(); ++i) {
            ID3D11ShaderResourceView* pTexture = { nullptr };
            pTexture = m_PrevTextures->Get_DXTexture(i);

            if (ImGui::ImageButton((void*)pTexture, size))
            {
                Create_MapObject(m_strModelTags[i]);
            }
            ImGui::SameLine(); // 앞뒤에 있는 줄이 같은 라인이게 만드는 함수

            if (2 == i % 3)
                ImGui::NewLine();
        }
        ImGui::End();
    }
}

void CMapTool::ModelList_Window()
{
    ImGui::SetWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetWindowSize(ImVec2(400.f, 600.f));

    if (show_ModelList_window) {
        ImGui::Begin("Model_List", &show_ModelList_window, ImGuiWindowFlags_MenuBar);
        ImVec2 size = ImVec2(80.f, 80.f);


        if (ImGui::BeginListBox("MapObjects List"))
        {

            for (size_t  i= 0; i < m_MapObjects.size(); ++i)
            {
                const bool is_selected = (m_iCurrent_MapObject_Index == i);
                char name[26];
                wstring wstrModelTag = m_MapObjects[i]->Get_ModelTag();
                string strModelTag;
                strModelTag.assign(wstrModelTag.begin(), wstrModelTag.end());
                strModelTag = strModelTag.substr(26);
                sprintf_s(name, sizeof(name), strModelTag.c_str());
                if (ImGui::Selectable(name, is_selected)) {
                    m_iCurrent_MapObject_Index = i;
                    m_pTargetObject = m_MapObjects[m_iCurrent_MapObject_Index];
                }

                //if (is_selected)
                //{
                //    ImGui::SetItemDefaultFocus();
                //}

            }
            ImGui::EndListBox();

            if (ImGui::Button("Delete")) {
                if (m_MapObjects.size() > m_iCurrent_MapObject_Index) {
                    m_MapObjects[m_iCurrent_MapObject_Index]->Set_Dead(true);
                    Safe_Release(m_MapObjects[m_iCurrent_MapObject_Index]);
                    m_MapObjects.erase(m_MapObjects.begin() + m_iCurrent_MapObject_Index);
                    m_pTargetObject = nullptr;
                    m_iCurrent_MapObject_Index = INFINITE;
                    m_iCurrent_Mesh_Index = INFINITE;
                }
            }
        }

        CMapObject* pTargetMapObject = dynamic_cast<CMapObject*>(m_pTargetObject);
        if (nullptr != pTargetMapObject) {

            if (ImGui::BeginListBox("Selected MapObject Meshes"))
            {

                CModel* pModel = dynamic_cast<CModel*>(pTargetMapObject->Get_Component(g_strModelTag));
                for (size_t i = 0; i < pModel->Get_NumMeshes(); ++i)
                {
                    const bool is_selected = (m_iCurrent_Mesh_Index == i);
                    char name[56];
                    sprintf_s(name, pModel->Get_Meshes()[i]->Get_Name());
                    if (ImGui::Selectable(name, is_selected))
                        m_iCurrent_Mesh_Index = i;

                    //if (is_selected)
                    //{
                    //    ImGui::SetItemDefaultFocus();
                    //    m_pTargetObject = m_MapObjects[i];
                    //}

                }
                ImGui::EndListBox();
            }
            //// temp static
            //static int	iCurrency = CMapObject::TYPE_END;
            //const char* Mesh_Type_Name[CMapObject::TYPE_END] = { "WALL", "ROAD", "OBJECT" };
            //const char* Current_Mesh_Type_Name = (iCurrency >= 0 && iCurrency < CMapObject::TYPE_END) ? Mesh_Type_Name[iCurrency] : "Unknown";
            //ImGui::SliderInt("Mesh Type", &iCurrency, 0, CMapObject::TYPE_END-1, Current_Mesh_Type_Name);
            // ImGui::InputFloat("red", &foo, 0.05f, 0, "%.3f");
        }

        ImGui::End();
    }

}

void CMapTool::MapList_Window()
{
    static int iCurrent_Map_Index = INFINITE;
    string filepath;
    string filepathName;

    ImGui::SetWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetWindowSize(ImVec2(400.f, 600.f));

    if (show_MapList_window) {
        ImGui::Begin("Map_List", &show_MapList_window, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginListBox("MapList"))
        {
            for (size_t i = 0; i < m_MapList.size(); ++i)
            {
                const bool is_selected = (iCurrent_Map_Index == i);
                char name[26];
                sprintf_s(name, m_MapList[i].c_str());
                if (ImGui::Selectable(name, is_selected)) {
                    iCurrent_Map_Index = i;
                }

            }


            ImGui::EndListBox();
        }
        if (ImGui::Button("Reset")) {
            iCurrent_Map_Index = INFINITE;
        }

        if (ImGui::Button("Save")) {
            IGFD::FileDialogConfig config;
            config.path = "../Bin/Resources/Map/";
            config.countSelectionMax = 1;
            config.flags = ImGuiFileDialogFlags_Modal;
            ImGuiFileDialog::Instance()->OpenDialog("Choose Directory And FileName", "Choose File", ".*", config);
        }

        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            if (iCurrent_Map_Index <= m_MapList.size()) {
                Load_Map(m_MapList[iCurrent_Map_Index]);
            }

        }

        ImGui::SameLine();
        if (ImGui::Button("Delete")) {
            ImGui::OpenPopup("Delete?");

        }
        // About Popup
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure to Delete?");
            if (ImGui::Button("OK", ImVec2(60, 0))) {
                if(!FAILED(Delete_Map(m_MapList[iCurrent_Map_Index]))) {
                    Update_MapList();
                }
                
                ImGui::CloseCurrentPopup();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(60, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }


        ImGui::End();
    }

    if (ImGuiFileDialog::Instance()->Display("Choose Directory And FileName")) {
        m_pImGUI_Manager->EditFilePath(filepath, filepathName);

        if (filepath != "" || filepathName != "") {
            Save_Map(filepath, filepathName);
        }
        Update_MapList();
        iCurrent_Map_Index = INFINITE;
    }

}

HRESULT CMapTool::Create_MapObject(const wstring& m_strModelTag)
{
    CMapObject::MAPOBJECT_DESC MapObjectDesc = {};
    MapObjectDesc.strModelTag = m_strModelTag;

    CMapObject* pObject = { nullptr };
    if(nullptr == (pObject = dynamic_cast<CMapObject*>(m_pGameInstance->Add_Clone_With_Object(m_eLevel, TEXT("Layer_MapObject"), TEXT("Prototype_GameObject_MapObject"), &MapObjectDesc))))
        return E_FAIL;

    m_MapObjects.push_back(pObject);
    Safe_AddRef(pObject);
    m_pTargetObject = pObject;

    return S_OK;
}

HRESULT CMapTool::Save_Map(string filepath, string filepathName)
{
    ofstream OFS{ filepath + "\\" + filepathName, ios::out | ios::binary};

    size_t MapObjects = m_MapObjects.size();
    OFS.write(reinterpret_cast<const char*>(&MapObjects), sizeof(size_t));

    for (auto pMapObjects : m_MapObjects)
    {
        wstring strModelTag = pMapObjects->Get_ModelTag();
        size_t strModelTagLength = strModelTag.size();
        OFS.write(reinterpret_cast<const char*>(&strModelTagLength), sizeof(size_t));
        OFS.write(reinterpret_cast<const char*>(strModelTag.c_str()), sizeof(_tchar) * strModelTagLength);

        _float4x4 WorldMarix = dynamic_cast<CTransform*>(pMapObjects->Get_Component(g_strTransformTag))->Get_WorldFloat4x4();

        OFS.write(reinterpret_cast<const char*>(&WorldMarix), sizeof(_float4x4));

    }
    OFS.close();
    return S_OK;
}

HRESULT CMapTool::Delete_Map(string strMapTag)
{
    wstring wstrMapTag = L"";
    wstrMapTag.assign(strMapTag.begin(), strMapTag.end());
    wstring filePath = L"../Bin/Resources/Map/" + wstrMapTag;
    if (DeleteFile(filePath.c_str()))
        return S_OK;
    return E_FAIL;
}

HRESULT CMapTool::Load_Map(string strMapTag)
{
    ifstream IFS{ "../Bin/Resources/Map/" + strMapTag, ios::in | ios::binary};

    size_t iModelNum = 0;
    IFS.read(reinterpret_cast<char*>(&iModelNum), sizeof(size_t));

    for (size_t i = 0; i < iModelNum; ++i) {
        size_t strModelTagLength;
        wstring strModelTag;

        IFS.read(reinterpret_cast<char*>(&strModelTagLength), sizeof(size_t));
        vector<_tchar> vecBuffer(strModelTagLength);

        IFS.read(reinterpret_cast<char*>(vecBuffer.data()), sizeof(_tchar) * strModelTagLength);
        strModelTag = wstring(vecBuffer.begin(), vecBuffer.end());

        _float4x4 WorldMatrix;
        IFS.read(reinterpret_cast<char*>(&WorldMatrix), sizeof(_float4x4));
        if (FAILED(Create_MapObject(strModelTag))) {
            return E_FAIL;
        }
        dynamic_cast<CTransform*>(m_MapObjects[m_MapObjects.size() - 1]->Get_Component(g_strTransformTag))->Set_WorldMatrix(WorldMatrix);
    }
    IFS.close();
    return S_OK;

}

CMapTool* CMapTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMapTool* pInstance = new CMapTool(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Initialize : CMapTool"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMapTool::Clone(void* pArg)
{
    CMapTool* pInstance = new CMapTool(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CMapTool"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapTool::Free()
{
    __super::Free();

    for (auto& pMapObject : m_MapObjects)
        Safe_Release(pMapObject);

    Safe_Release(m_pCamera);
    Safe_Release(m_PrevTextures);
    Safe_Release(m_pImGUI_Manager);
}
