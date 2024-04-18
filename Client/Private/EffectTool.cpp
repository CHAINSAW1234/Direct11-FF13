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


    return S_OK;
}

void CEffectTool::Tick(_float fTimeDelta)
{
    m_pImGUI_Manager->Tick(fTimeDelta);
    

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    WindowList_Window();
    Effect_Window();

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

        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Opotion List")) {

        ImGui::TreePop();
    }
    static float f = 0.f;
    //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
    //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
    //if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
    //    counter++;

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
                if (ImGui::Button("Delete")) {
                    if (m_iCurrent_Effect_Index < m_Effects.size()) {
                        m_Effects[m_iCurrent_Effect_Index]->Set_Dead(true);
                        Safe_Release(m_Effects[m_iCurrent_Effect_Index]);
                        m_Effects.erase(m_Effects.begin() + m_iCurrent_Effect_Index);
                        m_iCurrent_Effect_Index = INFINITE;
                        m_pTargetObject = nullptr;
                    }

                }


                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("3D"))
            {
                ImGui::Text("This is the 3D Effect tab!");

                static char buf[32];
                ImGui::InputText("Name", buf, IM_ARRAYSIZE(buf));

                const char* items[] = { "Cone", "Cube", "Cylinder", "Donut", "Sphere",
                    "around_0", "around_1", "Empty_Cylinder", "Effect_1", "Effect_2",
                    "Hit", "Optima_1", "Optima_2", "Optima_Cylinder", "Optima_Cylinder1", "Optima_Cylinder2"
                };
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

                static _float vPivot[3] = { 0.f,0.f,0.f };
                static _float vCenter[3] = { 0.f,0.f,0.f };
                static _float vRange[3] = { 0.f,0.f,0.f };
                static _float vMinScale[3] = { 0.f,0.f,0.f };
                static _float vMaxScale[3] = { 0.f,0.f,0.f };
                static _float vLifeTime[2] = { 0.f,0.f };
                static _float vSpeed[2] = { 0.f,0.f };
                static _int isLoop = { true };
                static _int iNumInstance = { 100 };

                ImGui::InputFloat3("Pivot", vPivot);
                ImGui::InputFloat3("Center", vCenter);
                ImGui::InputFloat3("Range", vRange);
                ImGui::InputFloat3("MinScale", vMinScale);
                ImGui::InputFloat3("MaxScale", vMaxScale);
                ImGui::InputFloat2("LifeTime", vLifeTime);
                ImGui::InputFloat2("Speed", vSpeed);
                ImGui::Text("IsLoop");
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

                    m_pTargetObject = Create_Effect_Instance(pDesc, buf);
                }

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::SeparatorText("Target Effect");
        if (nullptr != m_pTargetObject) {

            if (ImGui::TreeNode("Select Texture"))
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
                    if (iDiffuseTextureIndex >= 101)
                        iDiffuseTextureIndex = 101;
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
                    if (iMaskTextureIndex >= 101)
                        iMaskTextureIndex = 101;
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
                    if (iDissolveTextureIndex >= 101)
                        iDissolveTextureIndex = 101;
                }
                m_pTargetObject->Set_DissolveTexture(iDissolveTextureIndex);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Color"))
            {
                _float fColorMagnification = m_pTargetObject->Get_Color_Magnification();
                ImGui::InputFloat("Color Magnification", &fColorMagnification);
                m_pTargetObject->Set_Color_Magnification(fColorMagnification);

                _float vColor[4] = { m_pTargetObject->Get_Color().x,m_pTargetObject->Get_Color().y ,
                m_pTargetObject->Get_Color().z, m_pTargetObject->Get_Color().w };
                ImGui::InputFloat4("Color", vColor);
                m_pTargetObject->Set_Color(_float4(vColor[0], vColor[1], vColor[2], vColor[3]));

                ImGui::TreePop();
            }

            ImGui::SeparatorText("Mask Value");
            _float vMaskValueStart[4] = { m_pTargetObject->Get_MaskValue()[0].x, m_pTargetObject->Get_MaskValue()[0].y, m_pTargetObject->Get_MaskValue()[0].z , m_pTargetObject->Get_MaskValue()[0].w };
            _float vMaskValueEnd[4] = { m_pTargetObject->Get_MaskValue()[1].x, m_pTargetObject->Get_MaskValue()[1].y, m_pTargetObject->Get_MaskValue()[1].z, m_pTargetObject->Get_MaskValue()[1].w };
            ImGui::InputFloat4("Mask_Start", vMaskValueStart);
            ImGui::InputFloat4("Mask_End", vMaskValueEnd);

            _float4 vMaskValue[2] = {
                { vMaskValueStart[0], vMaskValueStart[1], vMaskValueStart[2], vMaskValueStart[3]},
                { vMaskValueEnd[0], vMaskValueEnd[1], vMaskValueEnd[2], vMaskValueEnd[3] } };

            m_pTargetObject->Set_MaskValue(vMaskValue);

            if (ImGui::TreeNode("Movement")) {
                _float fTextureMovement[2] = { m_pTargetObject->Get_TextureTimeDelta().x, m_pTargetObject->Get_TextureTimeDelta().y };
                ImGui::InputFloat2("Texture Movement", fTextureMovement);
                m_pTargetObject->Set_TextureMovement({ fTextureMovement[0],fTextureMovement[1] });

                _float fTurn[3] = { m_pTargetObject->Get_Turn().x, m_pTargetObject->Get_Turn().y, m_pTargetObject->Get_Turn().z };
                ImGui::InputFloat3("Turn", fTurn);
                m_pTargetObject->Set_Turn({ fTurn[0],fTurn[1], fTurn[2], 0.f });

                _float fTurnSpeed = m_pTargetObject->Get_TurnSpeed();
                ImGui::InputFloat("TurnSpeed", &fTurnSpeed);
                m_pTargetObject->Set_TurnSpeed(fTurnSpeed);

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Lerp")) {
                _float fLerpScale[3] = { m_pTargetObject->Get_LerpScale().x, m_pTargetObject->Get_LerpScale().y, m_pTargetObject->Get_LerpScale().z };
                ImGui::InputFloat3("Lerp", fLerpScale);
                m_pTargetObject->Set_LerpScale({ fLerpScale[0], fLerpScale[1], fLerpScale[2] });

                ImGui::TreePop();
            }

            _float fEffectTime = m_pTargetObject->Get_EffectTime();
            ImGui::InputFloat("Effect Time", &fEffectTime);
            m_pTargetObject->Set_EffectTime(fEffectTime);

            if (ImGui::Button("Reset Time")) {
                m_pTargetObject->Reset_Effect();
            }

            Show_Picking_ImGUI();
        }

        ImGui::End();
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

CEffect_3D* CEffectTool::Create_Effect_3D(_int iMeshIndex, const _char* EffectName)
{
    string str = string(EffectName);
    if (str.empty())
        return nullptr;

    CEffect_3D::EFFECT_3D_DESC pDesc = {};
    pDesc.strEffectName = str;
    pDesc.vPosition = { 0.f,0.f,0.f,1.f };
    pDesc.vColor = { 1.f,1.f,1.f,1.f };
    
    wstring strModelTag;
    switch (iMeshIndex) {
    case 0:
        strModelTag = TEXT("Prototype_Component_Model_Cone");
        break;
    case 1:
        strModelTag = TEXT("Prototype_Component_Model_Cube");
        break;
    case 2:
        strModelTag = TEXT("Prototype_Component_Model_Cylinder");
        break;
    case 3:
        strModelTag = TEXT("Prototype_Component_Model_Donut");
        break;
    case 4:
        strModelTag = TEXT("Prototype_Component_Model_Sphere");
        break;
    case 5:
        strModelTag = TEXT("Prototype_Component_Model_Around_0");
        break;
    case 6:
        strModelTag = TEXT("Prototype_Component_Model_Around_1");
        break;
    case 7:
        strModelTag = TEXT("Prototype_Component_Model_Empty_Cyclinder");
        break;
    case 8:
        strModelTag = TEXT("Prototype_Component_Model_Effect_0");
        break;
    case 9:
        strModelTag = TEXT("Prototype_Component_Model_Effect_1");
        break;
    case 10:
        strModelTag = TEXT("Prototype_Component_Model_Hit");
        break;
    case 11:
        strModelTag = TEXT("Prototype_Component_Model_Optima_1");
        break;
    case 12:
        strModelTag = TEXT("Prototype_Component_Model_Optima_2");
        break;
    case 13:
        strModelTag = TEXT("Prototype_Component_Model_Optima_Cylinder");
        break;
    case 14:
        strModelTag = TEXT("Prototype_Component_Model_Optima_Cylinder1");
        break;
    case 15:
        strModelTag = TEXT("Prototype_Component_Model_Optima_Cylinder2");
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

CEffect_Instance* CEffectTool::Create_Effect_Instance(CVIBuffer_Instance::INSTANCE_DESC InstanceDesc, const _char* EffectName)
{
    string str = string(EffectName);
    if (str.empty())
        return nullptr;
    //Prototype_Component_VIBuffer_Instance_Rect
    if (FAILED(m_pGameInstance->Add_Prototype(g_Level, TEXT("Prototype_Component_VIBuffer_Instance_" + *EffectName),
        CVIBuffer_Instance_Rect::Create(m_pDevice, m_pContext, InstanceDesc))))
        return nullptr;

    CEffect_Instance::EFFECT_INSTANCE_DESC pDesc = {};
    pDesc.strEffectName = str;
    pDesc.vPosition = { 0.f,0.f,0.f,1.f };
    pDesc.vColor = { 1.f,1.f,1.f,1.f };

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
