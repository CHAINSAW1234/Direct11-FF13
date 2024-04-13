#pragma once
#include "Sphere.h"

BEGIN(Client)
class CSphere_Heal final : public CSphere
{
private:
	CSphere_Heal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSphere_Heal(const CSphere_Heal& rhs);
	virtual ~CSphere_Heal() = default;

private:
	virtual void Check_Collision() override;

public:
	static CSphere_Heal* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
