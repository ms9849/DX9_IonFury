#include "MapWall.h"

#include "GameInstance.h"

CMapWall::CMapWall(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject{ pGraphic_Device }
{
}

CMapWall::CMapWall(const CMapWall& Prototype)
	: CGameObject(Prototype)
{
}

/*
오브젝트 아이디 추가해야됨
*/

HRESULT CMapWall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapWall::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_float3{ 50.f, 5.f, 0.5f });
	m_pTransformCom->Set_State(STATE::POSITION, { 27.5f, 2.5f, 30.f });

	return S_OK;
}

void CMapWall::Priority_Update(_float fTimeDelta)
{
}

void CMapWall::Update(_float fTimeDelta)
{
}

void CMapWall::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CMapWall::Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(1);

	m_pVIBufferCom->Render();

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

HRESULT CMapWall::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_Texture_Map_Wall"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_Component_VIBuffer_Cube"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Collider */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_BoxColliderCom), nullptr)))
		return E_FAIL;

	return S_OK;
}

void CMapWall::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
}

const COLLISION_DESC& CMapWall::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;
	Desc.pCollider = m_BoxColliderCom;

	return Desc;
}

CMapWall* CMapWall::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMapWall* pInstance = new CMapWall(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapWall::Clone(void* pArg)
{
	CMapWall* pInstance = new CMapWall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapWall::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_BoxColliderCom);
}
