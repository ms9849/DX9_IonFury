#include "Bullet_Wound.h"

#include "GameInstance.h"

CBullet_Wound::CBullet_Wound(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CEffect { pGraphic_Device }
{
}

CBullet_Wound::CBullet_Wound(const CBullet_Wound& Prototype) :
	CEffect { Prototype }
{
}

HRESULT CBullet_Wound::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBullet_Wound::Initialize(void* pArg)
{
	EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale({ 0.1f, 0.1f, 0.1f });
	m_pTransformCom->Set_State(STATE::POSITION, pDesc->vPosition);
	return S_OK;
}

void CBullet_Wound::Priority_Update(_float fTimeDelta)
{
}

void CBullet_Wound::Update(_float fTimeDelta)
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
	{
		Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
		m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
	}
}

void CBullet_Wound::Late_Update(_float fTimeDelta)
{
}

HRESULT CBullet_Wound::Render()
{
	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(0);

	if (FAILED(Begin_RenderState()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	if (FAILED(End_RenderState()))
		return E_FAIL;

	return S_OK;;
}

HRESULT CBullet_Wound::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Effect_Bullet_Wound"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}

HRESULT CBullet_Wound::Begin_RenderState()
{
	//m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	//m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	//m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	//m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return S_OK;
}

HRESULT CBullet_Wound::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	return S_OK;
}

CBullet_Wound* CBullet_Wound::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBullet_Wound* pInstance = new CBullet_Wound(pGraphicDev);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Create Failed : Bullet Wound");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBullet_Wound::Clone(void* pArg)
{
	CBullet_Wound* pInstance = new CBullet_Wound(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Clone Failed : Bullet Wound");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBullet_Wound::Free()
{
	__super::Free();
}
