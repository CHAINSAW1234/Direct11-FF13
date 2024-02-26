#ifndef InputDev_h__
#define InputDev_h__

#include "Base.h"

/* Ű����� ���콺�� �ʱ�ȭ�ϰ� ���� �Է���ġ�� ���¸� �޾ƿ´�. */

BEGIN(Engine)

class CInput_Device : public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;

public:
	_byte	Get_KeyState(_uint eState, _ubyte byKeyID);

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) { return m_tMouseState.rgbButtons[eMouse]; }

	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *(((_long*)&m_tMouseState) + eMouseState);
	}

public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Tick(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	_byte					m_DeviceKeyState[256];		// Ű���忡 �ִ� ��� Ű���� �����ϱ� ���� ����
	_ubyte					m_eKeyState[256] = { 0, };			// ���� Ű���� ���� ����
	DIMOUSESTATE			m_tMouseState;

public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void	Free(void);

};
END
#endif // InputDev_h__
