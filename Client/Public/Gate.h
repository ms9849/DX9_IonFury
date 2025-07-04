#pragma once

#include "Client_Defines.h"
#include "LandObject.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CGate final : public CLandObject
{
private:
    CGate(LPDIRECT3DDEVICE9 pGraphic_Device);
    CGate(const CGate& Prototype);
    virtual ~CGate() = default;

private:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    virtual HRESULT Ready_Components();
    virtual HRESULT Begin_RenderState();
    virtual HRESULT End_RenderState();

    void Gate_Animation(_float fTimeDelta);

private:
    CTexture* m_pTextureCom = { nullptr };
    CTransform* m_pPlayerTransformCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

    _float m_fTimeStack{ 0.f };
    _bool m_bOpen{ false }, m_bStop{ false };

public:
    //virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override;
    //virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType) override;

public:
    static CGate* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END;
