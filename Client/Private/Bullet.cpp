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

HRESULT CBullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(pArg);

	if (pDesc == nullptr)
		return S_OK;

	m_vDir = pDesc->vDir;
	m_pTransformCom->Set_State(STATE::POSITION, pDesc->vPos);

	return S_OK;
}

void CBullet::Priority_Update(_float fTimeDelta)
{
}

void CBullet::Update(_float fTimeDelta)
{
	m_pTransformCom->Go_Direction(m_vDir, fTimeDelta);
}

void CBullet::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CBullet::Render()
{
	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(3);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CBullet::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 3.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Sky"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
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
}
