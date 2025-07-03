#pragma once  

#include "Client_Defines.h"
#include "Player.h"
#include "LandObject.h"

NS_BEGIN(Engine)
class CSphereCollider;
class CTexture;
class CTransform;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CItem abstract : public CLandObject
{
protected:
    enum class BULLET {PISTOL, SHOOTGUN};

protected:
    CItem(LPDIRECT3DDEVICE9 pGraphic_Device);
    CItem(const CItem& Prototype);
    virtual ~CItem() = default;

protected:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

protected:
    CTexture* m_pTextureCom = { nullptr };  
    CTransform* m_pPlayerTransformCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CSphereCollider* m_pSphereColliderCom = { nullptr };

    bool    _isGoUp{ false };
    _float  m_fItemOriginPosY{};
    CPlayer* m_pPlayer{ nullptr };

protected:
    virtual HRESULT Ready_Components();
    virtual HRESULT Begin_RenderState();
    virtual HRESULT End_RenderState();
    void Item_Animation(_float fTimeDelta);

protected:
    virtual HRESULT Ready_Collider();
    virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType);
    

protected:
    //static CItem* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void Free();
};

NS_END