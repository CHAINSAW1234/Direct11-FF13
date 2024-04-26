#include "stdafx.h"
#include "EffectTool.h"

#include "Texture.h"
#include "Model.h"
#include "Mesh.h"

#include "Effect_2D.h"
#include "Effect_3D.h"
#include "Effect_Instance.h"

#include "Camera_Free.h"

#include "ImGUI_Manager.h"
#include "ImGuiFileDialog.h"

CEffectTool::CEffectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CEffectTool::CEffectTool(const CEffectTool& rhs)
    :CGameObject{ rhs }
{
}

HRESULT CEffectTool::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffectTool::Initialize(void* pArg)
{
    m_eLevel = g_Level;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pImGUI_Manager = CImGUI_Manager::Get_Instance(m_pDevice, m_pContext);

    m_pCamera = dynamic_cast<CCamera_Free*>(m_pGameInstance->Get_GameObject(m_eLevel, TEXT("Layer_Camera"), 0));
    Safe_AddRef(m_pCamera);

    Update_Saved_Effect();

    return S_OK;
}

void CEffectTool::Tick(_float fTimeDelta)
{
    m_pImGUI_Manager->Tick(fTimeDelta);
    

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    WindowList_Window();
    Effect_Window();
    Save_Window();
}

HRESULT CEffectTool::Late_Tick(_float fTimeDelta)
{
    m_pImGUI_Manager->Late_Tick(fTimeDelta);

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);

    return S_OK;
}

HRESULT CEffectTool::Render()
{
    m_pImGUI_Manager->Render();

    return S_OK;
}

void CEffectTool::Set_PickingTarget()
{
}

void CEffectTool::Show_Picking_ImGUI()
{
    if (nullptr != m_pTargetObject) {
        m_pImGUI_Manager->EditTransform(((CTransform*)m_pTargetObject->Get_Component(g_strTransformTag))->Get_WorldFloat4x4());
    }
}

void CEffectTool::WindowList_Window()
{

    ImGui::SetWindowSize(ImVec2(200.f, 200.f));

    ImGui::Begin("Hello, World!");                          // Create a window called "Hello, world!" and append into it.

    if (ImGui::TreeNode("Window List")) {
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Effect Window", &show_Effect_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Save Window", &show_Save_window);      // Edit bools storing our window open/close state

        
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Opotion List")) {

        ImGui::TreePop();
    }

    ImGui::SameLine();
    ImGui::End();

}

void CEffectTool::Effect_Window()
{
    if (show_Effect_window) {

        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoMove;

        ImGui::Begin("Effect Maker", NULL, window_flags);

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("EffectSelect", tab_bar_flags))
        {
            if (ImGui::BeginTabItem("List")) 
            {
                if (ImGui::BeginListBox(""))
                {
                    bool is_selected;    

                    for (_uint i = 0; i < m_Effects.size(); ++i)
                    {
                        is_selected = (m_iCurrent_Effect_Index == i);
                        if (ImGui::Selectable(m_Effects[i]->Get_Effect_Name().c_str(), is_selected)) {
                            m_iCurrent_Effect_Index = i;
                            m_pTargetObject = m_Effects[m_iCurrent_Effect_Index];
                        }
                    }

                    ImGui::EndListBox();
                }

                ImGui::SameLine();
                if (ImGui::Button("Add")) {
                    if (m_iCurrent_Effect_Index < m_Effects.size()) {
                        m_CandidateEffects.push_back(m_Effects[m_iCurrent_Effect_Index]);
                        m_iCurrent_Effect_Index = INFINITE;
                    }

                }

                ImGui::SameLine();
                if (ImGui::Button("Delete")) {
                    if (m_iCurrent_Effect_Index < m_Effects.size()) {
                        m_Effects[m_iCurrent_Effect_Index]->Set_Dead(true);
                        Safe_Release(m_Effects[m_iCurrent_Effect_Index]);
                        m_Effects.erase(m_Effects.begin() + m_iCurrent_Effect_Index);
                        m_iCurrent_Effect_Index = INFINITE;
                        m_pTargetObject = nullptr;
                    }
                }

                if (ImGui::Button("Reset All")) {
                    for (auto& pEffect : m_Effects) {
                        pEffect->Reset_Effect();
                        pEffect->Reset_Position();
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("3D"))
            {
                ImGui::Text("This is the 3D Effect tab!");

                static char buf[32];
                ImGui::InputText("Name", buf, IM_ARRAYSIZE(buf));

                const char* items[] = { "Capsule", "Circle", "Circle2", "Crack",
                    "Cylinder", "Cylinder1", "Cylinder2", "Electricity", "Electricity2", "Shock",
                    "Slash", "Sphere", "Thunder0", "Thunder1", "Thunder2", "Thunder3", "Thunder4", "Thunder5",
                    "Tornado", "Trail", "Wind", "Effect_1", "Effect_2", "Optima1", "Optima2" };
                static int iMeshIndex = 0;
                ImGui::Combo("Select Mesh", &iMeshIndex, items, IM_ARRAYSIZE(items));

                if (ImGui::Button("Create")) {
                    m_pTargetObject = Create_Effect_3D(iMeshIndex, buf);
                }

                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Instance"))
            {
                ImGui::Text("This is the Instance tab!");
                
                static char buf[32];
                ImGui::InputText("Effect Name", buf, IM_ARRAYSIZE(buf));

                static _int eType = 0;
                static _float vPivot[3] = { 0.f,0.f,0.f };
                static _float vCenter[3] = { 0.f,0.f,0.f };
                static _float vRange[3] = { 1.f,1.f,1.f };
                static _float vMinScale[3] = { 1.f,1.f,1.f };
                static _float vMaxScale[3] = { 1.f,1.f,1.f };
                static _float vLifeTime[2] = { 0.f,1.f };
                static _float vSpeed[2] = { 0.f,1.f };
                static _int isLoop = { true };
                static _int iNumInstance = { 100 };

                ImGui::Text("TYPE"); ImGui::SameLine();
                ImGui::RadioButton("Rect", &eType, 0); ImGui::SameLine();
                ImGui::RadioButton("Point", &eType, 1); 
                ImGui::InputFloat3("Pivot", vPivot);
                ImGui::InputFloat3("Center", vCenter);
                ImGui::InputFloat3("Range", vRange);
                ImGui::InputFloat3("MinScale", vMinScale);
                ImGui::InputFloat3("MaxScale", vMaxScale);
                ImGui::InputFloat2("LifeTime", vLifeTime);
                ImGui::InputFloat2("Speed", vSpeed);
                ImGui::Text("IsLoop"); ImGui::SameLine();
                ImGui::RadioButton("Loop_False", &isLoop, 0);   ImGui::SameLine();
                ImGui::RadioButton("Loop_True", &isLoop, 1); 
                ImGui::InputInt("NumInstance", &iNumInstance);

                if (ImGui::Button("Create")) {
                    CVIBuffer_Instance::INSTANCE_DESC pDesc = {};
                    pDesc.vPivot = { vPivot[0], vPivot[1], vPivot[2] };
                    pDesc.vCenter = { vCenter[0], vCenter[1], vCenter[2] };
                    pDesc.vRange = { vRange[0], vRange[1], vRange[2] };
                    pDesc.vMinScale = { vMinScale[0], vMinScale[1], vMinScale[2] };
                    pDesc.vMaxScale = { vMaxScale[0], vMaxScale[1], vMaxScale[2] };
                    pDesc.vLifeTime = { vLifeTime[0], vLifeTime[1] };
                    pDesc.vSpeed = { vSpeed[0], vSpeed[1] };
                    pDesc.isLoop = isLoop;
                    pDesc.iNumInstance = (_uint)iNumInstance;

                    m_pTargetObject = Create_Effect_Instance(pDesc, eType, buf);
                }

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::SeparatorText("Target Effect");
        CEffect_Instance* pTargetInstance = dynamic_cast<CEffect_Instance*>(m_pTargetObject);
        if (nullptr != pTargetInstance) {
            if (ImGui::TreeNode("Effect Instance")) {
                _int iMovement = pTargetInstance->Get_Movement();
                ImGui::RadioButton("Spread", &iMovement, 0); ImGui::SameLine();
                ImGui::RadioButton("Gather", &iMovement, 1); ImGui::SameLine();
                ImGui::RadioButton("Direction", &iMovement, 2);
                pTargetInstance->Set_Movement((Interface_Instance::MOVEMENT)iMovement);

                _float fDirection[3] = { pTargetInstance->Get_Direction().x, pTargetInstance->Get_Direction().y, pTargetInstance->Get_Direction().z } ;
                ImGui::InputFloat3("Movement Direction", fDirection);

                pTargetInstance->Set_Direction(_float4({ fDirection[0], fDirection[1], fDirection[2], 0}));


                _int iPivotLook = pTargetInstance->Get_Pivot_Look();
                ImGui::Text("Pivot Look");
                ImGui::RadioButton("Up", &iPivotLook, 0); ImGui::SameLine();
                ImGui::RadioButton("Up_Reverse", &iPivotLook, 1);
                ImGui::RadioButton("Right", &iPivotLook, 2); ImGui::SameLine();
                ImGui::RadioButton("Right_Reverse", &iPivotLook, 3); ImGui::SameLine();
                ImGui::RadioButton("None", &iPivotLook, 4);
                pTargetInstance->Set_Pivot_Look((Interface_Instance::PIVOT_LOOK)iPivotLook);

                _int iCamLook = pTargetInstance->Get_CamLook();
                ImGui::Text("Cam Look");
                ImGui::RadioButton("Look", &iCamLook, 1); ImGui::SameLine();
                ImGui::RadioButton("No Look", &iCamLook, 0);
                pTargetInstance->Se_CamLook(iCamLook);

                ImGui::TreePop();
            }
        }

        if (nullptr != m_pTargetObject) {


            if (ImGui::BeginTabBar("Target Effect", tab_bar_flags))
            {
                if (ImGui::BeginTabItem("Texture"))
                {
                    _int iDiffuseTextureIndex = m_pTargetObject->Get_DiffuseTexture();
                    ImGui::Text("DiffuseTexture");
                    ImGui::InputInt("DiffuseTexture", &iDiffuseTextureIndex);
                    if (ImGui::Button("Reset_Diffuse")) {
                        iDiffuseTextureIndex = 999;
                    }
                    if (iDiffuseTextureIndex != 999) {
                        if (iDiffuseTextureIndex <= 0)
                            iDiffuseTextureIndex = 0;
                        if (iDiffuseTextureIndex >= 172)
                            iDiffuseTextureIndex = 172;
                    }
                    m_pTargetObject->Set_DiffuseTexture(iDiffuseTextureIndex);

                    _int iMaskTextureIndex = m_pTargetObject->Get_MaskTexture();
                    ImGui::Text("MaskTexture");
                    ImGui::InputInt("MaskTexture", &iMaskTextureIndex);
                    if (ImGui::Button("Reset_Material")) {
                        iMaskTextureIndex = 999;
                    }
                    if (iMaskTextureIndex != 999) {
                        if (iMaskTextureIndex <= 0)
                            iMaskTextureIndex = 0;
                        if (iMaskTextureIndex >= 172)
                            iMaskTextureIndex = 172;
                    }
                    m_pTargetObject->Set_MaskTexture(iMaskTextureIndex);

                    _int iDissolveTextureIndex = m_pTargetObject->Get_DissolveTexture();
                    ImGui::Text("DissolveTexture");
                    ImGui::InputInt("DissolveTexture", &iDissolveTextureIndex);
                    if (ImGui::Button("Reset_Dissolve")) {
                        iDissolveTextureIndex = 999;
                    }
                    if (iDissolveTextureIndex != 999) {
                        if (iDissolveTextureIndex <= 0)
                            iDissolveTextureIndex = 0;
                        if (iDissolveTextureIndex >= 172)
                            iDissolveTextureIndex = 172;
                    }
                    m_pTargetObject->Set_DissolveTexture(iDissolveTextureIndex);

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Color"))
                {
                    _float fColorMagnification = m_pTargetObject->Get_Color_Magnification();
                    ImGui::InputFloat("Color Magnification", &fColorMagnification);
                    m_pTargetObject->Set_Color_Magnification(fColorMagnification);

                    static  ImGuiColorEditFlags iFlag = ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB;
                    iFlag |= ImGuiColorEditFlags_AlphaBar;

                    _float vColor[4] = { m_pTargetObject->Get_Color().x,m_pTargetObject->Get_Color().y ,
                    m_pTargetObject->Get_Color().z, m_pTargetObject->Get_Color().w };
                    ImGui::ColorPicker4("Color", vColor, iFlag);
                    ImGui::InputFloat4("vColor", vColor);
                    m_pTargetObject->Set_Color(_float4(vColor[0], vColor[1], vColor[2], vColor[3]));

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Mask")) {
                    _float vMaskValueStart[4] = { m_pTargetObject->Get_MaskValue()[0].x, m_pTargetObject->Get_MaskValue()[0].y, m_pTargetObject->Get_MaskValue()[0].z , m_pTargetObject->Get_MaskValue()[0].w };
                    _float vMaskValueEnd[4] = { m_pTargetObject->Get_MaskValue()[1].x, m_pTargetObject->Get_MaskValue()[1].y, m_pTargetObject->Get_MaskValue()[1].z, m_pTargetObject->Get_MaskValue()[1].w };
                    ImGui::InputFloat4("Mask_Start", vMaskValueStart);
                    ImGui::InputFloat4("Mask_End", vMaskValueEnd);

                    _float4 vMaskValue[2] = {
                        { vMaskValueStart[0], vMaskValueStart[1], vMaskValueStart[2], vMaskValueStart[3]},
                        { vMaskValueEnd[0], vMaskValueEnd[1], vMaskValueEnd[2], vMaskValueEnd[3] } };

                    m_pTargetObject->Set_MaskValue(vMaskValue);
                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Movement")) {
                    _float fTextureMovement[2] = { m_pTargetObject->Get_TextureTimeDelta().x, m_pTargetObject->Get_TextureTimeDelta().y };
                    ImGui::InputFloat2("Texture Movement", fTextureMovement);
                    m_pTargetObject->Set_TextureMovement({ fTextureMovement[0],fTextureMovement[1] });

                    _float fMove[3] = { m_pTargetObject->Get_Move().x, m_pTargetObject->Get_Move().y, m_pTargetObject->Get_Move().z };
                    ImGui::InputFloat3("Move", fMove);
                    m_pTargetObject->Set_Move({ fMove[0],fMove[1], fMove[2], 0.f });

                    _float fMoveSpeed = m_pTargetObject->Get_MoveSpeed();
                    ImGui::InputFloat("MoveSpeed", &fMoveSpeed);
                    m_pTargetObject->Set_MoveSpeed(fMoveSpeed);

                    _float fTurn[3] = { m_pTargetObject->Get_Turn().x, m_pTargetObject->Get_Turn().y, m_pTargetObject->Get_Turn().z };
                    ImGui::InputFloat3("Turn", fTurn);
                    m_pTargetObject->Set_Turn({ fTurn[0],fTurn[1], fTurn[2], 0.f });

                    _float fTurnSpeed = m_pTargetObject->Get_TurnSpeed();
                    ImGui::InputFloat("TurnSpeed", &fTurnSpeed);
                    m_pTargetObject->Set_TurnSpeed(fTurnSpeed);

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Lerp")) {
                    _float fLerpScaleStart[3] = { m_pTargetObject->Get_LerpScaleStart().x, m_pTargetObject->Get_LerpScaleStart().y, m_pTargetObject->Get_LerpScaleStart().z };
                    _float fLerpScaleEnd[3] = { m_pTargetObject->Get_LerpScaleEnd().x, m_pTargetObject->Get_LerpScaleEnd().y, m_pTargetObject->Get_LerpScaleEnd().z };

                    ImGui::InputFloat3("LerpStart", fLerpScaleStart);
                    ImGui::InputFloat3("LerpEnd", fLerpScaleEnd);
                    m_pTargetObject->Set_LerpScaleStart({ fLerpScaleStart[0], fLerpScaleStart[1], fLerpScaleStart[2] });
                    m_pTargetObject->Set_LerpScaleEnd({ fLerpScaleEnd[0], fLerpScaleEnd[1], fLerpScaleEnd[2] });

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

            ImGui::SeparatorText("Effect Time");
            _float fEffectTime = m_pTargetObject->Get_EffectTime();
            ImGui::InputFloat("Effect Time", &fEffectTime);
            m_pTargetObject->Set_EffectTime(fEffectTime);

            if (ImGui::Button("Reset Time")) {
                m_pTargetObject->Reset_Effect();
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset Position")) {
                m_pTargetObject->Reset_Position();
            }

            Show_Picking_ImGUI();
        }

        ImGui::End();
    }
}

void CEffectTool::Save_Window()
{
    static int iCurrent_Saved_Effect_Index = INFINITE;
    string filepath;
    string filepathName;
    
    if (show_Save_window) {
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoMove;

        ImGui::Begin("Save Effect Window", NULL, window_flags);                          // Create a window called "Hello, world!" and append into it.

        ImGui::SeparatorText("Candidate  Effects");
        if (ImGui::BeginListBox("Candidate \nEffect"))
        {
            bool is_selected;

            for (_uint i = 0; i < m_CandidateEffects.size(); ++i)
            {
                is_selected = (m_iCandidate_Effect_Index == i);
                if (ImGui::Selectable(m_CandidateEffects[i]->Get_Effect_Name().c_str(), is_selected)) {
                    m_iCandidate_Effect_Index = i;
                    m_pTargetObject = m_CandidateEffects[m_iCandidate_Effect_Index];
                }
            }

            ImGui::EndListBox();
        }
        if (ImGui::Button("Reset")) {
            for (auto& pEffect : m_CandidateEffects) {
                pEffect->Reset_Effect();
                pEffect->Reset_Position();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete")) {
            if (m_iCandidate_Effect_Index < m_CandidateEffects.size()) {
                m_CandidateEffects.erase(m_CandidateEffects.begin() + m_iCandidate_Effect_Index);
                m_iCandidate_Effect_Index = INFINITE;
                m_pTargetObject = nullptr;
            }
        }

        // 저장된 파일 리스트 보이기

        static _int iCurrent_Final_Effect_Index = INFINITE;
        ImGui::SeparatorText("Saved Effects");

        if (ImGui::BeginListBox("Final \nEffect"))
        {

            for (_uint i = 0; i < m_FinalEffects.size(); ++i)
            {
                const bool is_selected = (iCurrent_Final_Effect_Index == i);
                char name[60];
                sprintf_s(name, m_FinalEffects[i].c_str());
                if (ImGui::Selectable(name, is_selected)) {
                    iCurrent_Final_Effect_Index = (_int)i;
                }
            }
            ImGui::EndListBox();
        }

        if (ImGui::Button("Save Effect")) {
            IGFD::FileDialogConfig config;
            config.path = "../Bin/Resources/Effect/";
            config.countSelectionMax = 1;
            config.flags = ImGuiFileDialogFlags_Modal;
            ImGuiFileDialog::Instance()->OpenDialog("Choose Directory And FileName", "Choose File", ".*", config);
        }

        ImGui::SameLine();
        if (ImGui::Button("Load Effect")) {
            if (iCurrent_Final_Effect_Index <= m_FinalEffects.size()) {
                Load_Effect(m_FinalEffects[iCurrent_Final_Effect_Index]);
            }

        }

        ImGui::SameLine();
        if (ImGui::Button("Delete Effect")) {
            ImGui::OpenPopup("Delete?");

        }
        // About Popup
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopupModal("Delete?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Are you sure to Delete?");
            if (ImGui::Button("OK", ImVec2(60, 0))) {
                if (!FAILED(Delete_Effect(m_FinalEffects[iCurrent_Final_Effect_Index]))) {
                    Update_Saved_Effect();
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
            Save_Effect(filepath, filepathName);
        }
        Update_Saved_Effect();
    }



}

HRESULT CEffectTool::Add_Components()
{
    return S_OK;
}

void CEffectTool::Update_KeyInput()
{
    if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_ESCAPE)) {
        m_pTargetObject = nullptr;
    }
}

HRESULT CEffectTool::Update_Saved_Effect()
{
    m_FinalEffects.clear();

    WIN32_FIND_DATAA data;
    string		szFilePath = "../Bin/Resources/Effect/*";
    HANDLE hFind = FindFirstFileA(szFilePath.c_str(), &data);

    if (hFind == INVALID_HANDLE_VALUE)
        return E_FAIL;

    while (FindNextFileA(hFind, &data)) {
        if ((data.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE &&
            !(data.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)))
        {
            m_FinalEffects.push_back(data.cFileName);
        }
    }

    return S_OK;
}

HRESULT CEffectTool::Save_Effect(string filepath, string filepathName)
{
    ofstream OFS{ filepath + "\\" + filepathName, ios::out | ios::binary };

    size_t iEffectNum = m_CandidateEffects.size();
    OFS.write(reinterpret_cast<const char*>(&iEffectNum), sizeof(size_t));

    for (auto& pEffect : m_CandidateEffects)
    {
        pEffect->Save_Effect(OFS);
    }
    OFS.close();
    return S_OK;
}

HRESULT CEffectTool::Delete_Effect(string strEffectTag)
{
    wstring wstrMapTag = L"";
    wstrMapTag.assign(strEffectTag.begin(), strEffectTag.end());
    wstring filePath = L"../Bin/Resources/Effect/" + wstrMapTag;
    if (DeleteFile(filePath.c_str()))
        return S_OK;
    return E_FAIL;
}

HRESULT CEffectTool::Load_Effect(string strEffectTag)
{
    ifstream IFS{ "../Bin/Resources/Effect/" + strEffectTag, ios::in | ios::binary };

    size_t iEffectNum = 0;
    IFS.read(reinterpret_cast<char*>(&iEffectNum), sizeof(size_t));

    for (size_t i = 0; i < iEffectNum; ++i) {
        CEffect::TYPE eType = CEffect::EFFECT_END;
        IFS.read(reinterpret_cast<char*>(&eType), sizeof(CEffect::TYPE));

        if (eType == CEffect::EFFECT_END)
            return E_FAIL;

        CEffect* pEffect = { nullptr };

        switch (eType) {
        case CEffect::EFFECT_3D:
            pEffect = CEffect_3D::Clone(m_pDevice, m_pContext, IFS);
            break;
        case CEffect::EFFECT_INSTANCE:
            pEffect = CEffect_Instance::Clone(m_pDevice, m_pContext, IFS);
            break;
        }

        if (nullptr == pEffect)
            return E_FAIL;

        m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), pEffect);
        m_Effects.push_back(pEffect);
        Safe_AddRef(pEffect);

    }

    return S_OK;
}

CEffect_3D* CEffectTool::Create_Effect_3D(_int iMeshIndex, const _char* EffectName)
{
    string str = string(EffectName);
    if (str.empty())
        return nullptr;

    CEffect_3D::EFFECT_3D_DESC pDesc = {};
    pDesc.strEffectName = str;
    pDesc.vColor = { 1.f,1.f,1.f,1.f };
    
    wstring strModelTag;
    switch (iMeshIndex) {
    case 0:
        strModelTag = TEXT("Prototype_Component_Model_Capsule");
        break;
    case 1:
        strModelTag = TEXT("Prototype_Component_Model_Circle");
        break;
    case 2:
        strModelTag = TEXT("Prototype_Component_Model_Circle2");
        break;
    case 3:
        strModelTag = TEXT("Prototype_Component_Model_Crack");
        break;
    case 4:
        strModelTag = TEXT("Prototype_Component_Model_Cylinder");
        break;
    case 5:
        strModelTag = TEXT("Prototype_Component_Model_Cylinder1");
        break;
    case 6:
        strModelTag = TEXT("Prototype_Component_Model_Cylinder2");
        break;
    case 7:
        strModelTag = TEXT("Prototype_Component_Model_Electricity");
        break;
    case 8:
        strModelTag = TEXT("Prototype_Component_Model_Electricity2");
        break;
    case 9:
        strModelTag = TEXT("Prototype_Component_Model_Shock");
        break;
    case 10:
        strModelTag = TEXT("Prototype_Component_Model_Slash");
        break;
    case 11:
        strModelTag = TEXT("Prototype_Component_Model_Sphere");
        break;
    case 12:
        strModelTag = TEXT("Prototype_Component_Model_Thunder0");
        break;
    case 13:
        strModelTag = TEXT("Prototype_Component_Model_Thunder1");
        break;
    case 14:
        strModelTag = TEXT("Prototype_Component_Model_Thunder2");
        break;
    case 15:
        strModelTag = TEXT("Prototype_Component_Model_Thunder3");
        break;
    case 16:
        strModelTag = TEXT("Prototype_Component_Model_Thunder4");
        break;
    case 17:
        strModelTag = TEXT("Prototype_Component_Model_Thunder5");
        break;
    case 18:
        strModelTag = TEXT("Prototype_Component_Model_Tornado");
        break;
    case 19:
        strModelTag = TEXT("Prototype_Component_Model_Trail");
        break;
    case 20:
        strModelTag = TEXT("Prototype_Component_Model_Wind");
        break;
    case 21:
        strModelTag = TEXT("Prototype_Component_Model_Effect_1");
        break;
    case 22:
        strModelTag = TEXT("Prototype_Component_Model_Effect_2");
        break;
    case 23:
        strModelTag = TEXT("Prototype_Component_Model_Optima1");
        break;
    case 24:
        strModelTag = TEXT("Prototype_Component_Model_Optima2");
        break;
    }
    pDesc.strModelTag = strModelTag;

    CEffect_3D* pInstance = (CEffect_3D*)m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_3D"), &pDesc);

    if (nullptr == pInstance)
        return nullptr;

    m_Effects.push_back(pInstance);
    Safe_AddRef(pInstance);

    return pInstance;
}

CEffect_Instance* CEffectTool::Create_Effect_Instance(CVIBuffer_Instance::INSTANCE_DESC InstanceDesc, _int eType, const _char* EffectName)
{
    string str = string(EffectName);
    if (str.empty())
        return nullptr;
    wstring strName;
    strName.assign(str.begin(), str.end());
    wstring strPrototypeTag = L"Prototype_Component_VIBuffer_Instance_" + strName;
    //Prototype_Component_VIBuffer_Instance_Rect
    if (eType == 0) {
        if (FAILED(m_pGameInstance->Add_Prototype(g_Level, strPrototypeTag,
            CVIBuffer_Instance_Rect::Create(m_pDevice, m_pContext, InstanceDesc))))
            return nullptr;
    }
    else {
        if (FAILED(m_pGameInstance->Add_Prototype(g_Level, strPrototypeTag,
            CVIBuffer_Instance_Point::Create(m_pDevice, m_pContext, InstanceDesc))))
            return nullptr;
    }


    CEffect_Instance::EFFECT_INSTANCE_DESC pDesc = {};
    pDesc.eInstanceType = Interface_Instance::TYPE(eType);
    pDesc.strEffectName = str;
    pDesc.strBufferTag = strPrototypeTag;
    pDesc.vColor = { 1.f,1.f,1.f,1.f };
    pDesc.eInstance_Desc = InstanceDesc;

    CEffect_Instance* pInstance = (CEffect_Instance*)m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Effect_Instance"), &pDesc);

    if (nullptr == pInstance)
        return nullptr;

    m_Effects.push_back(pInstance);
    Safe_AddRef(pInstance);

    return pInstance;
}

CEffectTool* CEffectTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEffectTool* pInstance = new CEffectTool(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Initialize : CEffectTool"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CEffectTool* CEffectTool::Clone(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ifstream& IFS)
{
    return nullptr;
}

CGameObject* CEffectTool::Clone(void* pArg)
{
    CEffectTool* pInstance = new CEffectTool(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CEffectTool"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffectTool::Free()
{
    __super::Free();

    for (auto& pEffect : m_Effects)
        Safe_Release(pEffect);
    m_Effects.clear();

    Safe_Release(m_pCamera);
    Safe_Release(m_pImGUI_Manager);
}
