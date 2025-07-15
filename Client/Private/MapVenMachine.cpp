#include "MapVenMachine.h"

#include "GameInstance.h"
#include "Item.h"
#include "ItemArmorPack.h"
#include "ItemPistolBullet.h"
#include "ItemShootGunBullet.h"
#include "ItemHealpack.h"
#include "Bullet.h"

CMapVenMachine::CMapVenMachine(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCubeObject{ pGraphic_Device }
{
}

CMapVenMachine::CMapVenMachine(const CMapVenMachine& Prototype)
	: CCubeObject(Prototype)
{
}

/*
오브젝트 아이디 추가해야됨
*/

HRESULT CMapVenMachine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMapVenMachine::Initialize(void* pArg)
{
	m_pObjectDesc = *static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CMapVenMachine::Priority_Update(_float fTimeDelta)
{
}

void CMapVenMachine::Update(_float fTimeDelta)
{
	if (m_fTimeAcc <= 3.5f && m_bBroken)
		Item_Dispense(fTimeDelta);
}

void CMapVenMachine::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderGroup(RENDER::NONBLEND, this);
}

HRESULT CMapVenMachine::Render()
{
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	m_pTransformCom->Set_Transform();

	m_pTextureCom->Set_Texture(m_iTextureNum); // 터지면 1로

	m_pVIBufferCom->Render();

	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	return S_OK;
}

HRESULT CMapVenMachine::Ready_Components()
{
	/* Com_Transform */
	CTransform::TRANSFORM_DESC		TransformDesc{ 5.f, D3DXToRadian(90.0f) };
	if (FAILED(__super::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), reinterpret_cast<CComponent**>(&m_pTransformCom), &TransformDesc)))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(m_pObjectDesc.iLayerLevel, TEXT("Prototype_Component_Texture_Map_Ven_Machine"),
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

void CMapVenMachine::Item_Dispense(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;
	m_fItemCoolDown += fTimeDelta;

	if (m_fItemCoolDown >= 0.06f)
	{
		_int iRandomNum = static_cast<_int>(m_pGameInstance->Random(0.f, 6.0f));
		CItem* pItem;
		
		switch (iRandomNum)
		{
		case 0:
			pItem = static_cast<CItem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_pObjectDesc.iLayerLevel,
				TEXT("Prototype_GameObject_Item_ArmorPack"), &m_pObjectDesc));
			break;
		case 1:
			pItem = static_cast<CItem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_pObjectDesc.iLayerLevel,
				TEXT("Prototype_GameObject_Item_Pistol_Bullet"), &m_pObjectDesc));
			break;
		case 2:
			pItem = static_cast<CItem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_pObjectDesc.iLayerLevel,
				TEXT("Prototype_GameObject_Item_ShootGun_Bullet"), &m_pObjectDesc));
			break;
		case 3:
			pItem = static_cast<CItem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_pObjectDesc.iLayerLevel,
				TEXT("Prototype_GameObject_Item_ArmorPack"), &m_pObjectDesc));
			break;
		case 4:
			pItem = static_cast<CItem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_pObjectDesc.iLayerLevel,
				TEXT("Prototype_GameObject_Item_Burger"), &m_pObjectDesc));
			break;
		case 5:
			pItem = static_cast<CItem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_pObjectDesc.iLayerLevel,
				TEXT("Prototype_GameObject_Item_Coffee"), &m_pObjectDesc));
			break;
		default:
			pItem = static_cast<CItem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_pObjectDesc.iLayerLevel,
				TEXT("Prototype_GameObject_Item_Burger"), &m_pObjectDesc));
			break;
		}

		_float3 vDir = { m_pGameInstance->Random(-1.f, 0.f), 0.f,  m_pGameInstance->Random(-1.f, 1.f) };
		pItem->Set_Jump(true);
		pItem->Set_Parabola(true, vDir);

		m_pGameInstance->Add_Clone_ToLayer(pItem, m_pObjectDesc.iLayerLevel, TEXT("Layer_Items"));
		m_pGameInstance->PlaySoundOnce(TEXT("Vending_ItemDrop.ogg"), CHANNELID::SOUND_EFFECT, 0.3f);
		m_fItemCoolDown = 0.f;
	}
}

void CMapVenMachine::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta)
{
}

void CMapVenMachine::OnCollision(CGameObject* pDst, COLLISION eColType, _float fTimeDelta, CComponent* pCollider, const _float3& vPos)
{
	if (static_cast<CBullet*>(pDst) != nullptr)
	{
		m_iHp -= 1;

		if (m_iHp <= 0 && m_bBroken == false)
		{
			m_bBroken = true;
			m_iTextureNum = 1;
			m_pGameInstance->PlaySoundOnce(TEXT("Vending_Explode.ogg"), CHANNELID::SOUND_EFFECT, 0.5f);
		}
	}
}

const COLLISION_DESC& CMapVenMachine::Get_CollisionDesc(COLLISION eColType)
{
	COLLISION_DESC Desc;
	Desc.pTransform = m_pTransformCom;
	Desc.pCollider = m_BoxColliderCom;

	return Desc;
}

CMapVenMachine* CMapVenMachine::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CMapVenMachine* pInstance = new CMapVenMachine(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMapVenMachine::Clone(void* pArg)
{
	CMapVenMachine* pInstance = new CMapVenMachine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMapVenMachine::Free()
{
	__super::Free();
}
