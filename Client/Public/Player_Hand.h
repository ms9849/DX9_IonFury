#pragma once  

#include "Client_Defines.h"  
#include "GameObject.h"
#include "Animation.h"

NS_BEGIN(Engine)  
class CTexture;  
class CAnimation;  
class CTransform;  
class CVIBuffer_Rect;  
NS_END

NS_BEGIN(Client)

class CPlayer_Hand final : public CGameObject  
{  
private:  
    CPlayer_Hand(LPDIRECT3DDEVICE9 pGraphic_Device);  
    CPlayer_Hand(const CPlayer_Hand& Prototype);  
    virtual ~CPlayer_Hand() = default;  

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
    //CTexture* m_pTextureCom = { nullptr };  
    CAnimation* m_pAnimationCom = { nullptr };  
    CTransform* m_pTransformCom = { nullptr };  
    CTransform* m_pPlayerTransformCom = { nullptr };  
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };  

    map<const _wstring, CTexture*>   m_pTextureComs{};

    _wstring m_strFrameKey{ TEXT("Pistol_Idle") };
    map<const _wstring, CAnimation::FRAME_DESC> m_Frames;
    //vector<_uint> m_iAnimationFrames{};

    const _wstring m_strFrameKeys[3] = {
        TEXT("Pistol_Idle"), TEXT("Pistol_Reload"), TEXT("Pistol_Shoot")
    };

private:  
    HRESULT Ready_Components();  
    HRESULT Ready_Animations();  
    HRESULT Begin_RenderState();  
    HRESULT End_RenderState();  

public:  
    static CPlayer_Hand* Create(LPDIRECT3DDEVICE9 pGraphic_Device);  
    virtual CGameObject* Clone(void* pArg) override;  
    virtual void Free() override;  
};  

NS_END