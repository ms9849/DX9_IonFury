#pragma once  

#include "Client_Defines.h"
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
public:
    enum class BULLET {PISTOL, SHOOTGUN, END};

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

    _bool    _isGoUp{ false };
    _float  m_fItemOriginPosY{};
    class CPlayer* m_pPlayer{ nullptr };
    _float  m_fTimeAcc = { 0.f };
    _bool   m_bParabola = { false };
    _float3 m_vParabolaDir = {};

public:
    void Set_Pos(const _float3& vPos);
    void Set_Parabola(_bool bFlag, const _float3& vParabolaDir);
    //정해진 방향으로 x,z축 이동을 하면서, y는 포물선을 그려야 한다 
    void Parabola(_float fSpeed, _float fTimeDelta);

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