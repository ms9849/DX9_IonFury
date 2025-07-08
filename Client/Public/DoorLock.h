#pragma once  

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CSphereCollider;
class CTexture;
class CTransform;
class CVIBuffer_Rect;
class CAnimation;
NS_END

NS_BEGIN(Client)

class CDoorLock final : public CGameObject
{
protected:
    CDoorLock(LPDIRECT3DDEVICE9 pGraphic_Device);
    CDoorLock(const CDoorLock& Prototype);
    virtual ~CDoorLock() = default;

private:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

public:
    void Set_TargetID(_uint iTargetID);
    void Set_Can_Open(_bool bCanOpen);

private:
    CTransform* m_pTransformCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CSphereCollider* m_pSphereColliderCom = { nullptr };
    CAnimation* m_pAnimationCom{ nullptr };

    map<_wstring, CTexture*> m_pTextureComs{};
    _wstring m_strFrameKey{ TEXT("DoorLock_Idle") };

    _wstring m_strFrameKeys[3] = {
        TEXT("DoorLock_Idle"), TEXT("DoorLock_Unlock"), TEXT("DoorLock_Open")
    };

    _bool       m_bOpen{ false };
    _bool       m_bCanOpen{ false };
    _uint       m_iTargetID{ 0 };

private:
    virtual HRESULT Ready_Components();
    virtual HRESULT Begin_RenderState();
    virtual HRESULT End_RenderState();

private:
    virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override;
    virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType) override;

public:
    static CDoorLock* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END