#include "BoxCollider.h"

CBoxCollider::CBoxCollider(LPDIRECT3DDEVICE9 pGraphic_Device) :
	CComponent { pGraphic_Device }
{
}

/* m_pVB는 각자 다 달라질 것. */
CBoxCollider::CBoxCollider(const CBoxCollider& Prototype) :
	CComponent ( Prototype ),
	m_vScale { Prototype.m_vScale }
	, m_iVertexStride{ Prototype.m_iVertexStride }
	, m_iNumVertices{ Prototype.m_iNumVertices }
	, m_iFVF{ Prototype.m_iFVF }
	, m_ePrimitiveType{ Prototype.m_ePrimitiveType }
	, m_iNumPrimitive{ Prototype.m_iNumPrimitive }
	, m_iIndexStride{ Prototype.m_iIndexStride }
	, m_iNumIndices{ Prototype.m_iNumIndices }
	, m_eIndexFormat{ Prototype.m_eIndexFormat }
	, m_iNumVerticesX{ Prototype.m_iNumVerticesX }
	, m_iNumVerticesZ{ Prototype.m_iNumVerticesZ }
	, m_vMax { Prototype.m_vMax }
	, m_vMin { Prototype.m_vMin }
{
	memcpy(m_vLocalPos, Prototype.m_vLocalPos, sizeof(_float3) * 8);
	memcpy(m_vAxis, Prototype.m_vAxis, sizeof(_float3));
}

void CBoxCollider::Set_Matrix(const _float4x4& matWorld)
{
	for (_uint i = 0; i < 8; ++i)
	{
		D3DXVec3TransformNormal(&m_vLocalPos[i], &m_vLocalPos[i], &matWorld);
	}

	/* 정점 정보, 즉 크기는 콜라이더마다 다르니까 일단 둔다. */
	VTXCOLOR* pVertices = { nullptr };

	/* 할당한 공간에 접근하여 값을 기록하낟. */
	m_pVB->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

	pVertices[0].vPosition = m_vLocalPos[0];
	pVertices[0].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[1].vPosition = m_vLocalPos[1];
	pVertices[1].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[2].vPosition = m_vLocalPos[2];
	pVertices[2].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[3].vPosition = m_vLocalPos[3];
	pVertices[3].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[4].vPosition = m_vLocalPos[4];
	pVertices[4].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[5].vPosition = m_vLocalPos[5];
	pVertices[5].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[6].vPosition = m_vLocalPos[6];
	pVertices[6].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[7].vPosition = m_vLocalPos[7];
	pVertices[7].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	m_pVB->Unlock();

	/* vMin 찾기 */
	_float3 vMin = { FLT_MAX, FLT_MAX, FLT_MAX };
	_float3 vMax = { FLT_MIN, FLT_MIN, FLT_MIN };

	for (_uint i = 0; i < 8; ++i)
	{
		/* 셋 다 작다면 */
		if (m_vLocalPos[i].x <= vMin.x && m_vLocalPos[i].y <= vMin.y && m_vLocalPos[i].z <= vMin.z)
		{
			m_vMin = m_vLocalPos[i];
			vMin = m_vLocalPos[i];
		}

		if (m_vLocalPos[i].x >= vMax.x && m_vLocalPos[i].y >= vMax.y && m_vLocalPos[i].z >= vMax.z)
		{
			m_vMax = m_vLocalPos[i];
			vMax = m_vLocalPos[i];
		}
	}
}

void CBoxCollider::Set_Scale(const _float3& vScale)
{
	m_vScale = vScale;

	for (_uint i = 0; i < 8; ++i)
	{
		m_vLocalPos[i].x *= vScale.x;
		m_vLocalPos[i].y *= vScale.y;
		m_vLocalPos[i].z *= vScale.z;
	}

	/* 정점 정보, 즉 크기는 콜라이더마다 다르니까 일단 둔다. */
	VTXCOLOR* pVertices = { nullptr };

	/* 할당한 공간에 접근하여 값을 기록하낟. */
	m_pVB->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

	pVertices[0].vPosition = m_vLocalPos[0];
	pVertices[0].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[1].vPosition = m_vLocalPos[1];
	pVertices[1].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[2].vPosition = m_vLocalPos[2];
	pVertices[2].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[3].vPosition = m_vLocalPos[3];
	pVertices[3].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[4].vPosition = m_vLocalPos[4];
	pVertices[4].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[5].vPosition = m_vLocalPos[5];
	pVertices[5].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[6].vPosition = m_vLocalPos[6];
	pVertices[6].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[7].vPosition = m_vLocalPos[7];
	pVertices[7].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	m_pVB->Unlock();

	/* vMin 찾기 */
	_float3 vMin = { FLT_MAX, FLT_MAX, FLT_MAX };
	_float3 vMax = { FLT_MIN, FLT_MIN, FLT_MIN };

	for (_uint i = 0; i < 8; ++i)
	{
		/* 셋 다 작다면 */
		if (m_vLocalPos[i].x <= vMin.x && m_vLocalPos[i].y <= vMin.y && m_vLocalPos[i].z <= vMin.z)
		{
			m_vMin = m_vLocalPos[i];
			vMin = m_vLocalPos[i];
		}

		if (m_vLocalPos[i].x >= vMax.x && m_vLocalPos[i].y >= vMax.y && m_vLocalPos[i].z >= vMax.z)
		{
			m_vMax = m_vLocalPos[i];
			vMax = m_vLocalPos[i];
		}
	}
}

void CBoxCollider::Set_Pos(const _float3& vPos)
{
	m_vPos += vPos;

	for (_uint i = 0; i < 8; ++i)
	{
		m_vLocalPos[i].x += vPos.x;
		m_vLocalPos[i].y += vPos.y;
		m_vLocalPos[i].z += vPos.z;
	}

	VTXCOLOR* pVertices = { nullptr };

	/* 할당한 공간에 접근하여 값을 기록하낟. */
	m_pVB->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

	pVertices[0].vPosition = m_vLocalPos[0];
	pVertices[0].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[1].vPosition = m_vLocalPos[1];
	pVertices[1].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[2].vPosition = m_vLocalPos[2];
	pVertices[2].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[3].vPosition = m_vLocalPos[3];
	pVertices[3].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[4].vPosition = m_vLocalPos[4];
	pVertices[4].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[5].vPosition = m_vLocalPos[5];
	pVertices[5].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[6].vPosition = m_vLocalPos[6];
	pVertices[6].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[7].vPosition = m_vLocalPos[7];
	pVertices[7].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	m_pVB->Unlock();

	/* vMin 찾기 */
	_float3 vMin = { FLT_MAX, FLT_MAX, FLT_MAX };
	_float3 vMax = { FLT_MIN, FLT_MIN, FLT_MIN };

	for (_uint i = 0; i < 8; ++i)
	{
		/* 셋 다 작다면 */
		if (m_vLocalPos[i].x <= vMin.x && m_vLocalPos[i].y <= vMin.y && m_vLocalPos[i].z <= vMin.z)
		{
			m_vMin = m_vLocalPos[i];
			vMin = m_vLocalPos[i];
		}

		if (m_vLocalPos[i].x >= vMax.x && m_vLocalPos[i].y >= vMax.y && m_vLocalPos[i].z >= vMax.z)
		{
			m_vMax = m_vLocalPos[i];
			vMax = m_vLocalPos[i];
		}
	}
}

HRESULT CBoxCollider::Initialize_Prototype()
{
	m_iNumVertices = 8;
	m_iVertexStride = sizeof(VTXCOLOR);
	m_iFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	m_ePrimitiveType = D3DPT_TRIANGLELIST;
	m_iNumPrimitive = 12;

	m_iIndexStride = 2;
	m_iNumIndices = 36;
	m_eIndexFormat = D3DFMT_INDEX16;

	/*
	기본적으로 1,1,1 의 길이를 가진 충돌체가 등록된다. 마찬가지로 크기도 1,1,1 로 세팅된다.

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

	m_vScale = { 1.f, 1.f, 1.f };
	m_vAxis[0] = _float3(0.5f, 0.f, 0.f );
	m_vAxis[1] = _float3(0.f, 0.5f, 0.f);
	m_vAxis[2] = _float3(0.f, 0.f, 0.5f);

	m_vMin = m_vLocalPos[3];
	m_vMax = m_vLocalPos[5];

	return S_OK;
}

HRESULT CBoxCollider::Initialize(void* pArg)
{
	CBoxCollider::BOXCOLLIDER_DESC* pDesc = static_cast<BOXCOLLIDER_DESC*>(pArg);

	if (pDesc != nullptr)
	{
		m_vPos = pDesc->vPosition;
		for (_uint i = 0; i < 8; ++i)
		{
			m_vLocalPos[i].x += m_vPos.x;
			m_vLocalPos[i].y += m_vPos.y;
			m_vLocalPos[i].z += m_vPos.z;
		}

		m_vScale.x = pDesc->fScaleX;
		m_vScale.y = pDesc->fScaleY;
		m_vScale.z = pDesc->fScaleZ;

		D3DXVec3Scale(&m_vAxis[0], &m_vAxis[0], m_vScale.x);
		D3DXVec3Scale(&m_vAxis[1], &m_vAxis[1], m_vScale.y);
		D3DXVec3Scale(&m_vAxis[2], &m_vAxis[2], m_vScale.z);

		for (_uint i = 0; i < 8; ++i)
		{
			m_vLocalPos[i].x *= m_vScale.x;
			m_vLocalPos[i].y *= m_vScale.y;
			m_vLocalPos[i].z *= m_vScale.z;
		}
	}

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iNumVertices * m_iVertexStride, 0, m_iFVF, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	/* 정점 정보, 즉 크기는 콜라이더마다 다르니까 일단 둔다. */
	VTXCOLOR* pVertices = { nullptr };

	/* 할당한 공간에 접근하여 값을 기록하낟. */
	m_pVB->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0);

	pVertices[0].vPosition = m_vLocalPos[0];
	pVertices[0].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[1].vPosition = m_vLocalPos[1];
	pVertices[1].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[2].vPosition = m_vLocalPos[2];
	pVertices[2].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[3].vPosition = m_vLocalPos[3];
	pVertices[3].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[4].vPosition = m_vLocalPos[4];
	pVertices[4].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[5].vPosition = m_vLocalPos[5];
	pVertices[5].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[6].vPosition = m_vLocalPos[6];
	pVertices[6].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	pVertices[7].vPosition = m_vLocalPos[7];
	pVertices[7].vColor = D3DCOLOR_ARGB(255, 255, 0, 0);

	m_pVB->Unlock();

	/* 인덱스 자체는 똑같으니까 여기서 세팅 해줌 */
	_ushort* pIndices = { nullptr };

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iIndexStride * m_iNumIndices, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	m_pIB->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	/* + x */
	pIndices[0] = 1; pIndices[1] = 5;  pIndices[2] = 6;
	pIndices[3] = 1; pIndices[4] = 6;  pIndices[5] = 2;


	pIndices[6] = 4; pIndices[7] = 0;  pIndices[8] = 3;
	pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;


	pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
	pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;


	pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
	pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;


	pIndices[24] = 5; pIndices[25] = 4; pIndices[26] = 7;
	pIndices[27] = 5; pIndices[28] = 7; pIndices[29] = 6;


	pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
	pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

	m_pIB->Unlock();

	m_vMin = m_vLocalPos[3];
	m_vMax = m_vLocalPos[5];

	return S_OK;
}

void CBoxCollider::Render(const _float3& vPos)
{
	_float4x4 m_matIdentity;

	D3DXMatrixIdentity(&m_matIdentity);
	
	memcpy(&m_matIdentity.m[3][0], vPos, sizeof(_float3));

	m_pGraphic_Device->SetTransform(D3DTS_WORLD, &m_matIdentity);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pGraphic_Device->SetTexture(0, nullptr);

	m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iVertexStride);

	m_pGraphic_Device->SetFVF(m_iFVF);

	m_pGraphic_Device->SetIndices(m_pIB);

	m_pGraphic_Device->DrawIndexedPrimitive(m_ePrimitiveType, 0, 0, m_iNumVertices, 0, m_iNumPrimitive);

	m_pGraphic_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pGraphic_Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_pGraphic_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
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

	Safe_Release(m_pIB);
	Safe_Release(m_pVB);
}
