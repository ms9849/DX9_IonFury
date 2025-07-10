#include "TrashBox.h"

#include "GameInstance.h"
#include "Bullet.h"
#include "ItemArmor.h"
#include "ItemHealpack.h"
#include "ItemPistolBullet.h"
#include "ItemShootGunBullet.h"

CTrashBox::CTrashBox(LPDIRECT3DDEVICE9 pGraphicDev) :
    CGameObject { pGraphicDev }
{
}

CTrashBox::CTrashBox(const CTrashBox& Prototype) :
    CGameObject{ Prototype }
{
}

void CTrashBox::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
    //아마 레이충돌만 발생할거라서 여기에 뭐 안해도 됨
}

void CTrashBox::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider)
{
    //여기서 레이랑 충돌판정 하고 실제로 체력이 닳아서 없어지거나 하는 표현을 보여줄 것
	if (static_cast<CBullet*>(pDst) != nullptr && m_iHp >= 0)
	{
		m_iHp -= 1;

		if (m_iHp < 0)
		{
			/* 아이템 떨구는 로직도 추가할 것 */
			m_isDead = true;
			
			_int iRandNum = m_pGameInstance->Random(0.f, ENUM_CLASS(CItem::BULLET::END));
			CItem* pItem;

			if (iRandNum == ENUM_CLASS(CItem::BULLET::PISTOL))
				pItem = static_cast<CItem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Item_Pistol_Bullet"), nullptr));
			
			else if (iRandNum == ENUM_CLASS(CItem::BULLET::SHOOTGUN))
				pItem = static_cast<CItem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Prototype_GameObject_Item_ShootGun_Bullet"), nullptr));

			else
				return;

			_float3 vPos = m_pTransformCom->Get_State(STATE::POSITION);
			pItem->Set_Pos(vPos);
			m_pGameInstance->Add_Clone_ToLayer(pItem, ENUM_CLASS(LEVEL::GAMEPLAY),TEXT("Layer_Items"));
		}
	}
}

const COLLISION_DESC& CTrashBox::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pCollider = m_pBoxColliderCom;
	Desc.pTransform = m_pTransformCom;

	return Desc;
}

HRESULT CTrashBox::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTrashBox::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (pArg == nullptr)
		return S_OK;

	_float3* pDesc = reinterpret_cast<_float3*>(pArg);
	m_pTransformCom->Set_State(STATE::POSITION, *pDesc);
	m_pTransformCom->Set_Scale({ 0.3f, 0.3f, 0.3f });

    return S_OK;
}

void CTrashBox::Priority_Update(_float fTimeDelta)
{
	//테스트용 추후 발견하시면 지워버리셔도 됩니다
	int a = 10;
}

void CTrashBox::Update(_float fTimeDelta)
{
}

void CTrashBox::Late_Update(_float fTimeDelta)
{
	Compute_CamDistance(m_pTransformCom->Get_State(STATE::POSITION));
	m_pGameInstance->Add_RenderGroup(RENDER::BLEND, this);
}

HRESULT CTrashBox::Render()
{
	if (m_isDead) return S_OK;

	Begin_RenderState();

	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(2 - m_iHp);

	m_pVIBufferCom->Render();

	End_RenderState();
	
    return S_OK;
}

HRESULT CTrashBox::Begin_RenderState()
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

HRESULT CTrashBox::End_RenderState()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	return S_OK;
}

HRESULT CTrashBox::Ready_Components()
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
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_TrashBox"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

    return S_OK;
}

CTrashBox* CTrashBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTrashBox* pInstance = new CTrashBox(pGraphicDev);

	if (FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

    return pInstance;
}

CGameObject* CTrashBox::Clone(void* pArg)
{
	CTrashBox* pInstance = new CTrashBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CTrashBox::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pBoxColliderCom);
}
