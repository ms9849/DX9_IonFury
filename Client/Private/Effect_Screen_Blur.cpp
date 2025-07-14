#include "Effect_Screen_Blur.h"

#include "GameInstance.h"

CEffect_Screen_Blur::CEffect_Screen_Blur(LPDIRECT3DDEVICE9 pGraphic_Device) :
    CEffect { pGraphic_Device }
{
}

CEffect_Screen_Blur::CEffect_Screen_Blur(const CEffect_Screen_Blur& Prototype) :
    CEffect { Prototype }
{
}

HRESULT CEffect_Screen_Blur::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEffect_Screen_Blur::Initialize(void* pArg)
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


void CEffect_Screen_Blur::Priority_Update(_float fTimeDelta)
{
}

void CEffect_Screen_Blur::Update(_float fTimeDelta)
{
    m_fFrame += 1.f;
    m_fAge += fTimeDelta;

    if (m_fAge >= m_fLifeTime)
    {
        m_fAge = 0.f;
        m_isDead = true;
        m_fFrame = 0.f;
    }

    if (!m_isDead)
        m_pGameInstance->Add_RenderGroup(RENDER::UI, this);
}

void CEffect_Screen_Blur::Late_Update(_float fTimeDelta)
{
    m_fCamDistance = 0.1f;
}

HRESULT CEffect_Screen_Blur::Render()
{
    m_pTransformCom->Set_Transform();

    m_pTextureCom->Set_Texture(m_iTextureNum);

    Begin_RenderState();

    m_pVIBufferCom->Render();

    End_RenderState();

    return S_OK;
}

HRESULT CEffect_Screen_Blur::Ready_Components()
{
    /* Com_Transform */
    CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
        TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Effect_Screen_Blur"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Screen_Blur::Begin_RenderState()
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

HRESULT CEffect_Screen_Blur::End_RenderState()
{
    m_pGraphic_Device->SetRenderState(D3DRS_ZENABLE, TRUE);

    m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_OldViewMatrix);
    m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_OldProjMatrix);

    return S_OK;
}

void CEffect_Screen_Blur::Set_Pos(const _float3& vPos)
{
}

void CEffect_Screen_Blur::Set_TextureNum(_int iTextureNum)
{
    m_iTextureNum = iTextureNum;
}

CEffect_Screen_Blur* CEffect_Screen_Blur::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CEffect_Screen_Blur* pInstance = new CEffect_Screen_Blur(pGraphicDev);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Create Failed : Effect Screen Blur");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CEffect_Screen_Blur::Clone(void* pArg)
{
    CEffect_Screen_Blur* pInstance = new CEffect_Screen_Blur(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Clone Failed : Effect Screen Blur");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CEffect_Screen_Blur::Free()
{
    __super::Free();
}
