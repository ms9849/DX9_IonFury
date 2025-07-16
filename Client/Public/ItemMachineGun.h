#pragma once

#include "Client_Defines.h"
#include "Item.h"

NS_BEGIN(Client)

class CItemMachineGun : public CItem
{
private:
	CItemMachineGun(LPDIRECT3DDEVICE9 pGraphicDev);
	CItemMachineGun(const CItemMachineGun& Prototype);
	virtual ~CItemMachineGun() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

protected:
    virtual HRESULT Ready_Components() override;
    virtual HRESULT Begin_RenderState() override;
    virtual HRESULT End_RenderState() override;

public:
    virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override;

public:
	static CItemMachineGun* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
