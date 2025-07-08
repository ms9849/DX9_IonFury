#include "Bullet.h"

#include "GameInstance.h"

CBullet::CBullet(LPDIRECT3DDEVICE9 pGraphicDev)
	: CGameObject{ pGraphicDev }
{
}

CBullet::CBullet(const CBullet& Prototype )
	: CGameObject{ Prototype }
{
}

const RAY_DESC& CBullet::Get_RayDesc()
{
	RAY_DESC Desc;
	Desc.fSpeed = m_fBulletSpeed;
	Desc.vDir = m_vDir;
	Desc.vPos = m_pTransformCom->Get_State(STATE::POSITION);

	return Desc;
}

void CBullet::Set_Desc(const BULLET_DESC& Desc)
{
	m_fSumTime = 0.f;
	m_fBulletSpeed = Desc.fBulletSpeed;
	m_vDir = Desc.vDir;
	m_fDuration = Desc.fDuration;
	m_pTransformCom->Set_State(STATE::POSITION, Desc.vPos);

	m_pTransformCom->LookAt(m_pTransformCom->Get_State(STATE::POSITION) + m_vDir);
	m_pTransformCom->Set_Scale(Desc.vBulletScale);
	m_bPlayerBullet = Desc.isPlayerBullet;
}

HRESULT CBullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg == nullptr)
		return S_OK;

	BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);
	m_fBulletSpeed = pDesc->fBulletSpeed;

	m_vDir = pDesc->vDir;
	m_fDuration = pDesc->fDuration;
	m_pTransformCom->Set_State(STATE::POSITION, pDesc->vPos);

	m_pTransformCom->LookAt(m_pTransformCom->Get_State(STATE::POSITION) + m_vDir);
	m_pTransformCom->Set_Scale(pDesc->vBulletScale);
	m_bPlayerBullet = pDesc->isPlayerBullet;

	return S_OK;
}

void CBullet::Priority_Update(_float fTimeDelta)
{
}

void CBullet::Update(_float fTimeDelta)
{
	m_fSumTime += fTimeDelta;
	if (m_fSumTime >= m_fDuration)
		m_isDead = true;

	m_pTransformCom->Go_Direction(m_vDir, fTimeDelta);
}

void CBullet::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CBullet::Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Set_Transform();

	if(m_bPlayerBullet)
		m_pTextureCom->Set_Texture(0);
	else
		m_pTextureCom->Set_Texture(0);

	m_pVIBufferCom->Render();

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

void CBullet::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
	m_isDead = true;
}

void CBullet::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider)
{
	m_isDead = true;
}

const COLLISION_DESC& CBullet::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;
	Desc.pCollider = m_pSphereColliderCom;

	return Desc;
}

HRESULT CBullet::Ready_Components()
{
	/* Com_Transform */ //기본 속도 3
	CTransform::TRANSFORM_DESC		TransformDesc{ m_fBulletSpeed , D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_Bullet"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_SphereCollider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_SphereCollider"),
		TEXT("Com_SphereCollider"), reinterpret_cast<CComponent**>(&m_pSphereColliderCom))))
		return E_FAIL;

	return S_OK;
}

CBullet* CBullet::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBullet* pInstance = new CBullet(pGraphicDev);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("CREATE FAILED: Prototype_Bullet");
	}

	return pInstance;
}

CGameObject* CBullet::Clone(void* pArg)
{
	CBullet* pInstance = new CBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("CREATE FAILED: Clone_Bullet");
	}

	return pInstance;
}

void CBullet::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pSphereColliderCom);
}
