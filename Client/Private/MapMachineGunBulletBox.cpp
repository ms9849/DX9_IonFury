#include "MapMachineGunBulletBox.h"

#include "GameInstance.h"

CMapMachineGunBulletBox::CMapMachineGunBulletBox(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCubeObject{ pGraphic_Device }
{
}

CMapMachineGunBulletBox::CMapMachineGunBulletBox(const CMapMachineGunBulletBox& Prototype)
	: CCubeObject(Prototype)
{
}

/*
오브젝트 아이디 추가해야됨
*/

HRESULT CMapMachineGunBulletBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapMachineGunBulletBox::Initialize(void* pArg)
{
	m_pObjectDesc = *static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CMapMachineGunBulletBox::Priority_Update(_float fTimeDelta)
{
}

void CMapMachineGunBulletBox::Update(_float fTimeDelta)
{
}

void CMapMachineGunBulletBox::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CMapMachineGunBulletBox::Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(0);

	m_pVIBufferCom->Render();

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

HRESULT CMapMachineGunBulletBox::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(m_pObjectDesc.iLayerLevel, TEXT("Prototype_Component_Texture_Map_MachineGun_Bullet_Box"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(m_pObjectDesc.iLayerLevel, TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Collider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_BoxColliderCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

void CMapMachineGunBulletBox::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
}

const COLLISION_DESC& CMapMachineGunBulletBox::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;
	Desc.pCollider = m_BoxColliderCom;

	return Desc;
}

CMapMachineGunBulletBox* CMapMachineGunBulletBox::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMapMachineGunBulletBox* pInstance = new CMapMachineGunBulletBox(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapMachineGunBulletBox::Clone(void* pArg)
{
	CMapMachineGunBulletBox* pInstance = new CMapMachineGunBulletBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapMachineGunBulletBox::Free()
{
	__super::Free();
}
