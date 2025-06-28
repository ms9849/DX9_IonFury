#include "SphereCollider.h"

CSphereCollider::CSphereCollider(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CComponent{ pGraphic_Device }
{
}

CSphereCollider::CSphereCollider(const CSphereCollider& rhs) :
	CComponent(rhs),
	m_fRadius { rhs.m_fRadius },
	m_vLocalPos { rhs.m_vLocalPos }
{
}

HRESULT CSphereCollider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSphereCollider::Initialize(void* pArg)
{
	if (pArg == nullptr)
		return S_OK;

	SPHERECOLLIDER_DESC* pDesc = static_cast<SPHERECOLLIDER_DESC*>(pArg);
	
	m_fRadius = pDesc->fRadius;
	m_vLocalPos = pDesc->vLocalPos;

	return S_OK;
}

CSphereCollider* CSphereCollider::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSphereCollider* pInstance = new CSphereCollider(pGraphicDev);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create Failed Prpototype : Sphere Collider");
	}

	return pInstance;
}

CComponent* CSphereCollider::Clone(void* pArg)
{
	CSphereCollider* pInstance = new CSphereCollider(*this);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create Failed Prpototype : Sphere Collider");
	}

	return pInstance;
}

void CSphereCollider::Free()
{
	__super::Free();
}
