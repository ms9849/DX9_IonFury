#pragma once

#include "Client_Defines.h"
#include "Item.h"


NS_BEGIN(Client)

class CItemCoffee final : public CItem
{
private:
    CItemCoffee(LPDIRECT3DDEVICE9 pGraphic_Device);
    CItemCoffee(const CItemCoffee& Prototype);
    virtual ~CItemCoffee() = default;

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
    static CItemCoffee* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END;
