#include "Effect_Pistol_Fire.h"

#include "GameInstance.h"

CEffect_Pistol_Fire::CEffect_Pistol_Fire(LPDIRECT3DDEVICE9 pGraphic_Device): 
    CEffect { pGraphic_Device }
{
}

CEffect_Pistol_Fire::CEffect_Pistol_Fire(const CEffect_Pistol_Fire& Prototype) :
    CEffect( Prototype )
{
}

HRESULT CEffect_Pistol_Fire::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Pistol_Fire::Initialize(void* pArg)
{	
    if (FAILED(Ready_Components()))
        return E_FAIL;

    D3DVIEWPORT9		ViewportDesc{};
    m_pGraphic_Device->GetViewport(&ViewportDesc);
    D3DXMatrixIdentity(&m_ViewMatrix);
    D3DXMatrixOrthoLH(&m_ProjMatrix, ViewportDesc.Width, ViewportDesc.Height, 0.0f, 1.f);

    m_fWinSizeX = ViewportDesc.Width;
    m_fWinSizeY = ViewportDesc.Height;

    m_pTransformCom->Set_Scale(_float3(100, 100, 1.f));
    m_pTransformCom->Set_State(STATE::POSITION, _float3(750 - m_fWinSizeX * 0.5f, -380 + m_fWinSizeY * 0.5f, 0.f));
   
    m_fNumFrame = 90.f;

    return S_OK;
}

void CEffect_Pistol_Fire::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Pistol_Fire::Update(_float fTimeDelta)
{
   // Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
}

void CEffect_Pistol_Fire::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CEffect_Pistol_Fire::Render()
{
    m_pTransformCom->Set_Transform();

    m_pTextureCom->Set_Texture(static_cast<_uint>(m_fFrame));

    Begin_RenderState();

    m_pVIBufferCom->Render();

    End_RenderState();
    
    return S_OK;
}

HRESULT CEffect_Pistol_Fire::Ready_Components()
{
    /* Com_Transform */
    CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Effect"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Pistol_Fire::Begin_RenderState()
{
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

HRESULT CEffect_Pistol_Fire::End_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);

    m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_OldViewMatrix);
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_OldProjMatrix);

    return S_OK;
}


CEffect_Pistol_Fire* CEffect_Pistol_Fire::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CEffect_Pistol_Fire* pInstance = new CEffect_Pistol_Fire(pGraphicDev);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : Effect Pistol Fire");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CEffect_Pistol_Fire::Clone(void* pArg)
{
    CEffect_Pistol_Fire* pInstance = new CEffect_Pistol_Fire(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : Effect Pistol Fire");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CEffect_Pistol_Fire::Free()
{
    __super::Free();
}
