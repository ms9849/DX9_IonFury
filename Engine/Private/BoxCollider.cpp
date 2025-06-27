#include "BoxCollider.h"

CBoxCollider::CBoxCollider(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CComponent { pGraphic_Device }
{
}

CBoxCollider::CBoxCollider(const CBoxCollider& rhs) :
	CComponent ( rhs ),
	m_fScale { rhs.m_fScale }
{
	memcpy(m_vLocalPos, rhs.m_vLocalPos, sizeof(_float3) * 8);
	memcpy(m_vAxis, rhs.m_vAxis, sizeof(_float3));
}

HRESULT CBoxCollider::Initialize_Prototype()
{
	/*
	기본적으로 1,1,1 의 길이를 가진 충돌체가 등록된다.

	추후 콜라이더의 위치를 조정 가능하게 할 수 있지만 이 부분은 당장은 생략.

	현재는 크기만 조정 가능하도록 설정
	*/
	m_vLocalPos[0] = _float3(-0.5f, 0.5f, -0.5f);
	m_vLocalPos[1] = _float3(0.5f, 0.5f, -0.5f);
	m_vLocalPos[2] = _float3(0.5f, -0.5f, -0.5f);
	m_vLocalPos[3] = _float3(-0.5f, -0.5f, -0.5f);
	m_vLocalPos[4] = _float3(-0.5f, 0.5f, 0.5f);
	m_vLocalPos[5] = _float3(0.5f, 0.5f, 0.5f);
	m_vLocalPos[6] = _float3(0.5f, -0.5f, 0.5f);
	m_vLocalPos[7] = _float3(-0.5f, -0.5f, 0.5f);

	/*
	0, 1, 2
	right, up look 순.
	*/
	m_fScale = { 1.f, 1.f, 1.f };
	m_vAxis[0] = _float3(0.5f, 0.f, 0.f );
	m_vAxis[1] = _float3(0.f, 0.5f, 0.f);
	m_vAxis[2] = _float3(0.f, 0.f, 0.5f);

	return S_OK;
}

HRESULT CBoxCollider::Initialize(void* pArg)
{
	CBoxCollider::BOXCOLLIDER_DESC* pDesc = static_cast<BOXCOLLIDER_DESC*>(pArg);

	if (pDesc == nullptr)
		return S_OK;

	m_fScale.x = pDesc->fScaleX;
	m_fScale.y = pDesc->fScaleY;
	m_fScale.z = pDesc->fScaleZ;

	D3DXVec3Scale(&m_vAxis[0], &m_vAxis[0], m_fScale.x);
	D3DXVec3Scale(&m_vAxis[0], &m_vAxis[0], m_fScale.y);
	D3DXVec3Scale(&m_vAxis[0], &m_vAxis[0], m_fScale.z);

	for (int i = 0; i < 8; ++i)
	{
		m_vLocalPos[i].x *= m_fScale.x;
		m_vLocalPos[i].y *= m_fScale.y;
		m_vLocalPos[i].z *= m_fScale.z;
	}

	return S_OK;
}

CBoxCollider* CBoxCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBoxCollider* pInstance = new CBoxCollider(pGraphicDev);

	if(FAILED(pInstance->Initialize_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent* CBoxCollider::Clone(void* pArg)
{
	CBoxCollider* pInstance = new CBoxCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CBoxCollider::Free()
{
	__super::Free();
}
