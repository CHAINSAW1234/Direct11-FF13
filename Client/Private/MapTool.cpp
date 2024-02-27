#include "stdafx.h"
#include "MapTool.h"
#include "ImGUI_Manager.h"

CMapTool::CMapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CMapTool::CMapTool(const CMapTool& rhs)
    :CGameObject(rhs)
{
}

HRESULT CMapTool::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMapTool::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pImGUI_Manager = CImGUI_Manager::Get_Instance(m_pDevice, m_pContext);

    return S_OK;
}

void CMapTool::Tick(_float fTimeDelta)
{
    m_pImGUI_Manager->Tick(fTimeDelta);

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    Show_Picking_ImGUI();

}

void CMapTool::Late_Tick(_float fTimeDelta)
{
    m_pImGUI_Manager->Late_Tick(fTimeDelta);
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMapTool::Render()
{
    m_pImGUI_Manager->Render();

    return S_OK;
}

void CMapTool::Show_Picking_ImGUI()
{
    if (nullptr != m_pPickingTarget) {
        m_pImGUI_Manager->EditTransform(*((CTransform*)m_pPickingTarget->Get_Component(g_strTransformTag))->Get_WorldFloat4x4());
    }

}

HRESULT CMapTool::Add_Components()
{
    return S_OK;
}

HRESULT CMapTool::Set_RenderState()
{
    return S_OK;
}

HRESULT CMapTool::Reset_RenderState()
{
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
    Safe_Release(m_pImGUI_Manager);
}
