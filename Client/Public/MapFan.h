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

class CMapFan final : public CGameObject
{
protected:
    CMapFan(LPDIRECT3DDEVICE9 pGraphic_Device);
    CMapFan(const CMapFan& Prototype);
    virtual ~CMapFan() = default;

private:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

private:
    CTransform* m_pTransformCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CSphereCollider* m_pSphereColliderCom = { nullptr };
    CAnimation* m_pAnimationCom{ nullptr };

    map<_wstring, CTexture*> m_pTextureComs{};
    _wstring m_strFrameKey{ TEXT("Blood_Fan_Active") };

    _wstring m_strFrameKeys[4] = {
        TEXT("Default_Fan_Idle"), TEXT("Default_Fan_Active"),
        TEXT("Blood_Fan_Idle"), TEXT("Blood_Fan_Active")
    };


private:
    virtual HRESULT Ready_Components();
    virtual HRESULT Begin_RenderState();
    virtual HRESULT End_RenderState();

private:
    virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override;
    virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType) override;

public:
    static CMapFan* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END