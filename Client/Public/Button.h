#pragma once  

#include "Client_Defines.h"
#include "GameObject.h"

NS_BEGIN(Engine)
class CSphereCollider;
class CTexture;
class CTransform;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CButton final : public CGameObject
{
protected:
    CButton(LPDIRECT3DDEVICE9 pGraphic_Device);
    CButton(const CButton& Prototype);
    virtual ~CButton() = default;

private:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta);
    virtual void Update(_float fTimeDelta);
    virtual void Late_Update(_float fTimeDelta);
    virtual HRESULT Render();

public:
    void Button_Control(_float fTimeDelta);

    void Set_Active(_bool bActive);
    void Set_TargetID(_uint iTargetID);

    _bool Get_Lever_State();

private:
    CTexture* m_pTextureCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CSphereCollider* m_pSphereColliderCom = { nullptr };

private:
    virtual HRESULT Ready_Components();
    virtual HRESULT Begin_RenderState();
    virtual HRESULT End_RenderState();

    _uint   m_iTargetID = { 0 };
    _bool   m_bActive = { false };
    _bool   m_bOpen = { false };

private:
    virtual void OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta) override;
    virtual const COLLISION_DESC& Get_CollisionDesc(COLLISION eColType) override;

public:
    static CButton* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END