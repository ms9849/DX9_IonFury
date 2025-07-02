#pragma once  

#include "Client_Defines.h"  
#include "GameObject.h"
#include "GameInstance.h"

NS_BEGIN(Engine)
class CTexture;
class CAnimation;
class CTransform;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CPlayer_LeftHand final : public CGameObject
{
private:
    CPlayer_LeftHand(LPDIRECT3DDEVICE9 pGraphic_Device);
    CPlayer_LeftHand(const CPlayer_LeftHand& Prototype);
    virtual ~CPlayer_LeftHand() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void Set_Player_Transform(CTransform* pTransform);
    void Set_Current_Animation(const _wstring& strFrameKey);

private:
    CAnimation* m_pAnimationCom = { nullptr };
    CTransform* m_pTransformCom = { nullptr };
    CTransform* m_pPlayerTransformCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

    map<const _wstring, CTexture*>   m_pTextureComs{};

    _wstring m_strFrameKey{TEXT("CardKey_Idle")};

    const _wstring m_strFrameKeys[3] = {
        TEXT("CardKey_Idle"), TEXT("CardKey_Up"), TEXT("CardKey_Down")
    };

private:
    HRESULT Ready_Components();
    HRESULT Ready_Animations();
    HRESULT Begin_RenderState();
    HRESULT End_RenderState();

public:
    static CPlayer_LeftHand* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END