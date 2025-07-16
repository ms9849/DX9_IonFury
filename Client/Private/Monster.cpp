#include "Monster.h"

#include "Bullet.h"
#include "BehaviorNode.h"
#include "ItemArmorPack.h"
#include "ItemPistolBullet.h"
#include "ItemShootGunBullet.h"
#include "ItemHealpack.h"

CMonster::CMonster(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CLandObject{ pGraphic_Device }
{

}

CMonster::CMonster(const CMonster& Prototype)
	: CLandObject{ Prototype }
{

}

HRESULT CMonster::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CMonster::Initialize()
{
	m_bRideCube = true;
	CLandObject::LANDOBJECT_DESC			Desc{};
	Desc.pLandTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(m_pObjectDesc.iLayerLevel, TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pLandVIBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(m_pObjectDesc.iLayerLevel, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;

	return S_OK;
}

void CMonster::Priority_Update(_float fTimeDelta)
{

}

void CMonster::Update(_float fTimeDelta)
{

}

void CMonster::Late_Update(_float fTimeDelta)
{

}

HRESULT CMonster::Render()
{

	return S_OK;
}

void CMonster::RecoveryHp()
{
	m_fCurHp = m_fMaxHp;
}

_float CMonster::Get_Hp()
{
	return m_fCurHp;
}

void CMonster::Drop_Item()
{
	_int iRandomNum = static_cast<_int>(m_pGameInstance->Random(0.f, 6.0f));
	CItem* pItem;

	switch (iRandomNum)
	{
	case 0:
		pItem = static_cast<CItem*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_pObjectDesc.iLayerLevel,
			TEXT("Prototype_GameObject_Item_Healpack"), &m_pObjectDesc));
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
	default:
		pItem = nullptr;
		break;
	}

	if (pItem != nullptr)
	{
		pItem->Set_Pos(m_pTransformCom->Get_State(STATE::POSITION));
		m_pGameInstance->Add_Clone_ToLayer(pItem, m_pObjectDesc.iLayerLevel, TEXT("Layer_Items"));
	}
}


HRESULT CMonster::Ready_Animations()
{
	return S_OK;
}

HRESULT CMonster::Ready_Components()
{
	return S_OK;
}

HRESULT CMonster::Begin_RenderState()
{
	return S_OK;
}

HRESULT CMonster::End_RenderState()
{
	return S_OK;
}

void CMonster::Attack()
{

}

void CMonster::Move()
{
}

void CMonster::RotateToPlayer(CTransform* pTranform)
{ 
	_float4x4 matWorldTemp = m_pGameInstance->Get_CameraWorld();

	_float3 vLook;
	memcpy(&vLook, *(_float3*)&matWorldTemp.m[2][0], sizeof(_float3));
	vLook *= -1;
	vLook.y = 0.f;
	D3DXVec3Normalize(&vLook, &vLook);

	_float3 vUp = { 0.f, 1.f, 0.f };

	_float3 vRight;
	D3DXVec3Cross(&vRight, &vUp, &vLook);
	D3DXVec3Normalize(&vRight, &vRight);

	_float3 scale = pTranform->Get_Scaled();

	vRight *= scale.x;
	vUp *= scale.y;
	vLook *= scale.z;

	_float4x4 matWorld = *pTranform->Get_WorldMatrixPtr();
	_float3 vPos = pTranform->Get_State(STATE::POSITION);

	memcpy(&matWorld.m[0][0], &vRight, sizeof(_float3));
	memcpy(&matWorld.m[1][0], &vUp, sizeof(_float3));
	memcpy(&matWorld.m[2][0], &vLook, sizeof(_float3));
	memcpy(&matWorld.m[3][0], &vPos, sizeof(_float3));

	pTranform->Set_Transform(matWorld);



	//_float4x4		ViewMatrix = m_pGameInstance->Get_CameraWorldInv();

	//pTranform->Set_State(STATE::RIGHT, *reinterpret_cast<_float3*>(&ViewMatrix.m[0]));
	///*m_pTransformCom->Set_State(STATE::UP, *reinterpret_cast<_float3*>(&ViewMatrix.m[1]));*/
	//pTranform->Set_State(STATE::LOOK, *reinterpret_cast<_float3*>(&ViewMatrix.m[2]));

}

void CMonster::Free()
{
	__super::Free();

	Safe_Release(m_pAnimationCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pSightCom);
	Safe_Release(m_pBoxColliderCom);
	Safe_Release(m_pBoxColliderHead);
	Safe_Release(m_pSphereColliderCom);

	for (auto& iter : m_pTextureComs)
	{
		Safe_Release(iter.second);
	}
	m_pTextureComs.clear();
}
