#pragma once

#include "Client_Defines.h"
#include "Item.h"


NS_BEGIN(Client)

class CItemCardKey final : public CItem
{
private:
    CItemCardKey(LPDIRECT3DDEVICE9 pGraphic_Device);
    CItemCardKey(const CItemCardKey& Prototype);
    virtual ~CItemCardKey() = default;

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
    static CItemCardKey* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END;
