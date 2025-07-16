#include "Monster.h"
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

void CMonster::Set_Hp(_float fHp)
{
	m_fMaxHp = fHp;
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
	//m_pTransformCom->Set_State(STATE::UP, *reinterpret_cast<_float3*>(&ViewMatrix.m[1]));
	//pTranform->Set_State(STATE::LOOK, *reinterpret_cast<_float3*>(&ViewMatrix.m[2]));

}

void CMonster::RandomMove(_float fTimeDelta, _float3 nextDir)
{

	_float3 vNextDir = {};
	vNextDir = nextDir;
	if (D3DXVec3Length(&vNextDir) < 0.001f || !_finite(vNextDir.x) || !_finite(vNextDir.y) || !_finite(vNextDir.z))
	{
		vNextDir = { 0.f, 0.f, 1.f };
		//char szBuffer1[128];
		//sprintf_s(szBuffer1, "[Warning] vNextDir 비정상값 감지! x: %.3f, y: %.3f, z: %.3f\n", vNextDir.x, vNextDir.y, vNextDir.z);
		//OutputDebugStringA(szBuffer1);  // 디버그 출력창에 메시지 전송
	}

	D3DXVec3Normalize(&vNextDir, &vNextDir);

	_float3 fMonsterLook = m_pTransformCom->Get_State(STATE::LOOK);
	D3DXVec3Normalize(&fMonsterLook, &fMonsterLook);

	float dot = D3DXVec3Dot(&fMonsterLook, &vNextDir);
	dot = max(-1.f, min(1.f, dot));
	float fRadian = acosf(dot);

	_float3 vCross = {};
	D3DXVec3Cross(&vCross, &fMonsterLook, &vNextDir);
	if (vCross.y < 0)
		fRadian = -fRadian;

	_float3 vTarget = m_pTransformCom->Get_State(STATE::POSITION) + vNextDir;

	m_pTransformCom->Rotation({ 0.f, 1.f, 0.f }, fRadian);
	m_pTransformCom->Go_Direction(vNextDir, fTimeDelta);
	m_pTransformCom->LookAt(vTarget);
}

void CMonster::RandomRotate()
{
	_float3 vMin = { -1.f, 0.f, -1.f };
	_float3 vMax = { 1.f, 0.f, 1.f };
	m_pGameInstance->GetRandomVector(&m_vNextDir, &vMin, &vMax);
	m_vNextDir.y = 0.f;

	if (D3DXVec3Length(&m_vNextDir) < 0.001f || !_finite(m_vNextDir.x) || !_finite(m_vNextDir.y) || !_finite(m_vNextDir.z))
	{
		m_vNextDir = { 0.f, 0.f, 1.f };
	}

	D3DXVec3Normalize(&m_vNextDir, &m_vNextDir);
	_float3 vTarget = m_pTransformCom->Get_State(STATE::POSITION) + m_vNextDir;

	m_pTransformCom->LookAt(vTarget); 
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
