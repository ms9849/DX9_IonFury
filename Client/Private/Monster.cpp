#include "Monster.h"

#include "Bullet.h"
#include "BehaviorNode.h"

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
	CLandObject::LANDOBJECT_DESC			Desc{};
	Desc.pLandTransform = static_cast<CTransform*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_Transform")));
	Desc.pLandVIBuffer = static_cast<CVIBuffer*>(m_pGameInstance->Get_Component(ENUM_CLASS(LEVEL::GAMEPLAY), TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));

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
