#include "Input_Device.h"

Engine::CInput_Device::CInput_Device(void)
{

}

_byte CInput_Device::Get_KeyState(_uint eState, _ubyte byKeyID)
{
	if (m_DeviceKeyState[byKeyID] & 0x80)
	{
		if (m_eKeyState[byKeyID] == KEY_FREE)
			m_eKeyState[byKeyID] = KEY_DOWN;
		else
			m_eKeyState[byKeyID] = KEY_PRESS;
	}
	else
	{
		if (m_eKeyState[byKeyID] == KEY_DOWN || m_eKeyState[byKeyID] == KEY_PRESS)
			m_eKeyState[byKeyID] = KEY_UP;
		else
			m_eKeyState[byKeyID] = KEY_FREE;
	}

	return (m_eKeyState[byKeyID] == eState);

}

HRESULT Engine::CInput_Device::Initialize(HINSTANCE hInst, HWND hWnd)
{
	// DInput �İ�ü�� �����ϴ� �Լ�
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		nullptr)))
		return E_FAIL;

	// Ű���� ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;

	// ������ Ű���� ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ��ġ�� ���� �������� �������ִ� �Լ�, (Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pKeyBoard->Acquire();


	// ���콺 ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// ������ ���콺 ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pMouse->Acquire();


	return S_OK;
}

void Engine::CInput_Device::Tick(void)
{

	/* Ű����� ���콺�� � �Է»��¸� ������ �ִ����� �����Ѵ�. */
	m_pKeyBoard->GetDeviceState(256, m_DeviceKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

CInput_Device* CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
	CInput_Device* pInstance = new CInput_Device();

	if (FAILED(pInstance->Initialize(hInst, hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CInput_Device"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::CInput_Device::Free(void)
{
	__super::Free();
	//Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}

