#include "Effect_Black_Sight.h"

#include "GameInstance.h"

CEffect_Black_Sight::CEffect_Black_Sight(LPDIRECT3DDEVICE9 pGraphicDev) :
    CEffect { pGraphicDev }
{
}

CEffect_Black_Sight::CEffect_Black_Sight(const CEffect_Black_Sight& Prototype) :
    CEffect { Prototype }
{
}

HRESULT CEffect_Black_Sight::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Black_Sight::Initialize(void* pArg)
{
    if (FAILED(Ready_Components()))
        return E_FAIL;

    D3DVIEWPORT9		ViewportDesc{};
    m_pGraphic_Device->GetViewport(&ViewportDesc);
    D3DXMatrixIdentity(&m_ViewMatrix);
    D3DXMatrixOrthoLH(&m_ProjMatrix, ViewportDesc.Width, ViewportDesc.Height, 0.0f, 1.f);

    m_fWinSizeX = ViewportDesc.Width;
    m_fWinSizeY = ViewportDesc.Height;

    m_pTransformCom->Set_Scale(_float3(m_fWinSizeX, m_fWinSizeY, 1.f));
    m_pTransformCom->Set_State(STATE::POSITION, _float3(m_fWinSizeX * 0.5f - m_fWinSizeX * 0.5f, m_fWinSizeY * 0.5f - m_fWinSizeY * 0.5f, 0.f));

    m_fNumFrame = 90.f;

    return S_OK;
}

void CEffect_Black_Sight::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Black_Sight::Update(_float fTimeDelta)
{
}

void CEffect_Black_Sight::Late_Update(_float fTimeDelta)
{
    if(m_iAlpha <= 255)
        m_iAlpha += 10.f * fTimeDelta;

    if (m_iAlpha >= 255)
    {
        m_iAlpha = 255;
        m_fFrame = 0.f;
        m_isDead = true;
    }

    if (!m_isDead)
        m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);

}

HRESULT CEffect_Black_Sight::Render()
{
    m_pTransformCom->Set_Transform();

    m_pTextureCom->Set_Texture(0);

    Begin_RenderState();

    m_pVIBufferCom->Render();

    End_RenderState();

    return S_OK;
}

HRESULT CEffect_Black_Sight::Ready_Components()
{
    /* Com_Transform */
    CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Effect_Black_Sight"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Black_Sight::Begin_RenderState()
{
    //m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    //m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    //m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    //m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    //m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(m_iAlpha, 255, 255, 255));

    // 알파 블렌딩 설정
    m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


    /*직교 투영 파트*/
    m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, FALSE);

    m_pGraphic_Device->GetTransform(D3DTS_VIEW, &m_OldViewMatrix);
    m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &m_OldProjMatrix);

    m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_ViewMatrix);
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_ProjMatrix);

    return S_OK;
}

HRESULT CEffect_Black_Sight::End_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);

    m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_OldViewMatrix);
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_OldProjMatrix);

    return S_OK;
}

void CEffect_Black_Sight::Set_Pos(const _float3& vPos)
{
}

CEffect_Black_Sight* CEffect_Black_Sight::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CEffect_Black_Sight* pInstance = new CEffect_Black_Sight(pGraphicDev);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : Effect Black Sight");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CEffect_Black_Sight::Clone(void* pArg)
{
     CEffect_Black_Sight* pInstance = new CEffect_Black_Sight(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : Effect Black Sight");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CEffect_Black_Sight::Free()
{
    __super::Free();
}
