#pragma once  

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CAnimation;
class CSphereCollider;
NS_END

NS_BEGIN(Client)

class CMapGrinder final : public CGameObject
{
protected:
    CMapGrinder(LPDIRECT3DDEVICE9 pGraphic_Device);
    CMapGrinder(const CMapGrinder& Prototype);
    virtual ~CMapGrinder() = default;

private:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

private:
    CTexture* m_pTextureCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CAnimation* m_pAnimationCom = { nullptr };
    CSphereCollider* m_pSphereColliderCom = { nullptr };

private:
    virtual HRESULT Ready_Components();
    virtual HRESULT Begin_RenderState();
    virtual HRESULT End_RenderState();

private:
    virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override;
    virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType) override;

public:
    static CMapGrinder* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END