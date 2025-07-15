#include "MapTrashBox.h"

#include "GameInstance.h"
#include "Bullet.h"
#include "ItemArmor.h"
#include "ItemHealpack.h"
#include "ItemPistolBullet.h"
#include "ItemShootGunBullet.h"
#include "ItemPortableHealPack.h"

CMapTrashBox::CMapTrashBox(LPDIRECT3DDEVICE9 pGraphicDev) :
    CGameObject { pGraphicDev }
{
}

CMapTrashBox::CMapTrashBox(const CMapTrashBox& Prototype) :
    CGameObject( Prototype )
{
}

void CMapTrashBox::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
    //아마 레이충돌만 발생할거라서 여기에 뭐 안해도 됨
}

void CMapTrashBox::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider, const _float3& vPos)
{
    //여기서 레이랑 충돌판정 하고 실제로 체력이 닳아서 없어지거나 하는 표현을 보여줄 것
	if (static_cast<CBullet*>(pDst) != nullptr && m_iHp >= 0)
	{
		m_iHp -= 1;

		if (m_iHp < 0)
		{
			CItem* pItem = static_cast<CItem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_pObjectDesc.iLayerLevel, TEXT("Prototype_GameObject_Item_Portable_Healpack"), &m_pObjectDesc));
			/* 아이템 떨구는 로직도 추가할 것 */
			m_isDead = true;
			
			//_int iRandNum = m_pGameInstance->Random(0.f, ENUM_CLASS(CItem::BULLET::END));
			//CItem* pItem;

			//if (iRandNum == ENUM_CLASS(CItem::BULLET::PISTOL))
			//	pItem = static_cast<CItem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_pObjectDesc.iLayerLevel, TEXT("Prototype_GameObject_Item_Pistol_Bullet"), &m_pObjectDesc));
			//
			//else if (iRandNum == ENUM_CLASS(CItem::BULLET::SHOOTGUN))
			//	pItem = static_cast<CItem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_pObjectDesc.iLayerLevel, TEXT("Prototype_GameObject_Item_ShootGun_Bullet"), &m_pObjectDesc));
			//	
			//else
			//	return;

		/*	_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
			vPos.y -= 0.3;
			pItem->Set_Pos(vPos);*/
			m_pGameInstance->Add_Clone_ToLayer(pItem, m_pObjectDesc.iLayerLevel,TEXT("Layer_Items"));
		}
	}
}

const COLLISION_DESC& CMapTrashBox::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pCollider = m_pBoxColliderCom;
	Desc.pTransform = m_pTransformCom;

	return Desc;
}

HRESULT CMapTrashBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMapTrashBox::Initialize(void* pArg)
{
	m_pObjectDesc = *static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg == nullptr)
		return S_OK;

	//_float3* pDesc = reinterpret_cast<_float3*>(pArg);
	//m_pTransformCom->Set_State(STATE::POSITION, *pDesc);
	//m_pTransformCom->Set_Scale({ 0.3f, 0.3f, 0.3f });

    return S_OK;
}

void CMapTrashBox::Priority_Update(_float fTimeDelta)
{
	//테스트용 추후 발견하시면 지워버리셔도 됩니다
}

void CMapTrashBox::Update(_float fTimeDelta)
{
}

void CMapTrashBox::Late_Update(_float fTimeDelta)
{
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CMapTrashBox::Render()
{
	if (m_isDead) return S_OK;

	Begin_RenderState();

	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(2 - m_iHp);

	m_pVIBufferCom->Render();

	End_RenderState();
	
    return S_OK;
}

HRESULT CMapTrashBox::Begin_RenderState()
{
	_float4x4 matCameraWorld =  m_pGameInstance->Get_CameraWorld();

	m_pTransformCom->Set_State(STATE::RIGHT, *(_float3 *)&matCameraWorld[0] * 0.3f);
	m_pTransformCom->Set_State(STATE::LOOK, *(_float3*)&matCameraWorld[2] * 0.3f);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_pGraphic_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pGraphic_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAREF, 200);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);


	return S_OK;
}

HRESULT CMapTrashBox::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CMapTrashBox::Ready_Components()
{
	/* 텍스쳐는 추후 세팅할 것 */
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 4.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_BoxCollider */
	CBoxCollider::BOXCOLLIDER_DESC Desc;
	Desc.vPosition = { 0.f, 0.f, 0.f };
	Desc.fScaleX = 0.2f;
	Desc.fScaleZ = 0.2f;
	Desc.fScaleY = 0.2f;
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_BoxCollider"),
		TEXT("Com_BoxCollider"), reinterpret_cast<CComponent**>(&m_pBoxColliderCom), &Desc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(m_pObjectDesc.iLayerLevel, TEXT("Prototype_Component_Texture_Map_TrashBox"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

    return S_OK;
}

CMapTrashBox* CMapTrashBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMapTrashBox* pInstance = new CMapTrashBox(pGraphicDev);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

    return pInstance;
}

CGameObject* CMapTrashBox::Clone(void* pArg)
{
	CMapTrashBox* pInstance = new CMapTrashBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CMapTrashBox::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pBoxColliderCom);
}
