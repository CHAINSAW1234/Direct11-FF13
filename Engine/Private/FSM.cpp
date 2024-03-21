#include "FSM.h"
#include "FSM_State.h"


CFSM::CFSM(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CFSM::CFSM(const CFSM& rhs)
	:CComponent(rhs)
{
}

HRESULT CFSM::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFSM::Initialize(void* pArg)
{
	//if (pArg == nullptr)
	//	return E_FAIL;

	//FSM_DESC* FSMDesc = (FSM_DESC*)pArg;
	//m_mapFSM_State = *FSMDesc->mapFSM_State;
	//
	//pCurrent_State = Find_State(FSMDesc->Current_State);
	//if (nullptr == pCurrent_State)
	//	return E_FAIL;

	//Safe_AddRef(pCurrent_State);

	return S_OK;
}

void CFSM::Update(_float fTimeDelta)
{
	if (pCurrent_State == nullptr)
		return;


	/* FSM function& ���ٷ� �غ����� �õ� : Update���� ChangeState���� ó���� �� �ְԲ�
	////���� ���¿��� subfsm~ �� ã��
	//auto mapiter = m_mapFSM_subState.find(0); //idle;
	//SubFSM* subfsm = mapiter->second;

	//auto transitions = subfsm->vecTransition;
	//_uint nextState = ::Escape_int;
	//for (auto& trans : transitions)
	//{
	//	if (trans.first())
	//	{
	//		nextState = trans.second;
	//		break;
	//	}
	//}

	//if (nextState == ::Escape_int)
	//	return;

	//Change_State(nextState);*/

	pCurrent_State->OnStateUpdate(fTimeDelta);
}

void CFSM::Start()
{
	for (auto& mapFSM_State : m_mapFSM_State) {
		mapFSM_State.second->Start();
	}
}


void CFSM::Change_State(_uint iState)
{
	auto pNextState = Find_State(iState);

	if (nullptr == pNextState || pCurrent_State == pNextState)
		return;

	// ====== ���°� ����Ǿ��ٸ� ======
	// ���� ���� Terminate
	if (pCurrent_State != nullptr)
		pCurrent_State->OnStateExit();

	// ���� ���� ����
	Safe_Release(pCurrent_State);
	pCurrent_State = pNextState;
	Safe_AddRef(pCurrent_State);
	// �ٲ� ������ Execute 
	pCurrent_State->OnStateEnter();

}

void CFSM::Add_State(_uint iState, CFSM_State* pInitState)
{
	if (nullptr == pInitState) {
		return;
	}	
	m_mapFSM_State.emplace(iState, pInitState);
}

CFSM_State* CFSM::Find_State(_uint iState)
{
	auto iter = m_mapFSM_State.find(iState);
	if (iter == m_mapFSM_State.end()) return nullptr;
	return iter->second;
}


CFSM* CFSM::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFSM* pInstance = new CFSM(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFSM"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


CComponent* CFSM::Clone(void* pArg)
{
	CFSM* pInstance = new CFSM(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CFSM"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFSM::Free()
{
	__super::Free();

	Safe_Release(pCurrent_State);
	for (auto& pFSMstate : m_mapFSM_State)
		Safe_Release(pFSMstate.second);

	m_mapFSM_State.clear();
}

