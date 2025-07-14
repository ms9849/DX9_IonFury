#include "VIBuffer_Cube.h"
#include "GameInstance.h"

CVIBuffer_Cube::CVIBuffer_Cube(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer { pGraphic_Device }
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube& Prototype)
	: CVIBuffer{ Prototype }
{
	memcpy(m_iIndices, Prototype.m_iIndices, sizeof(_ushort) * 36);
}

HRESULT CVIBuffer_Cube::Initialize_Prototype()
{
	m_iNumVertices = 8;
	m_iVertexStride = sizeof(VTXCUBE);
	m_iFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);
	m_ePrimitiveType = D3DPT_TRIANGLELIST;	
	m_iNumPrimitive = 12;	

	m_iIndexStride = 2;
	m_iNumIndices = 36;
	m_eIndexFormat = D3DFMT_INDEX16;
	
	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iNumVertices * m_iVertexStride, 0, m_iFVF, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;	

	VTXCUBE* pVertices = { nullptr };
	m_pVertexPositions = new _float3[m_iNumVertices];

	/* 할당한 공간에 접근하여 값을 기록하낟. */
	m_pVB->Lock(0, /*m_iNumVertices * m_iVertexStride*/0, reinterpret_cast<void**>(&pVertices), 0);

	m_pVertexPositions[0] = pVertices[0].vPosition = _float3(-0.5f, 0.5f, -0.5f);
	pVertices[0].vTexcoord = pVertices[0].vPosition;

	m_pVertexPositions[1] = pVertices[1].vPosition = _float3(0.5f, 0.5f, -0.5f);
	pVertices[1].vTexcoord = pVertices[1].vPosition;

	m_pVertexPositions[2] = pVertices[2].vPosition = _float3(0.5f, -0.5f, -0.5f);
	pVertices[2].vTexcoord = pVertices[2].vPosition;

	m_pVertexPositions[3] = pVertices[3].vPosition = _float3(-0.5f, -0.5f, -0.5f);
	pVertices[3].vTexcoord = pVertices[3].vPosition;

	m_pVertexPositions[4] = pVertices[4].vPosition = _float3(-0.5f, 0.5f, 0.5f);
	pVertices[4].vTexcoord = pVertices[4].vPosition;

	m_pVertexPositions[5] = pVertices[5].vPosition = _float3(0.5f, 0.5f, 0.5f);
	pVertices[5].vTexcoord = pVertices[5].vPosition;

	m_pVertexPositions[6] = pVertices[6].vPosition = _float3(0.5f, -0.5f, 0.5f);
	pVertices[6].vTexcoord = pVertices[6].vPosition;

	m_pVertexPositions[7] = pVertices[7].vPosition = _float3(-0.5f, -0.5f, 0.5f);
	pVertices[7].vTexcoord = pVertices[7].vPosition;

	m_pVB->Unlock();

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iIndexStride * m_iNumIndices, 0, m_eIndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;


	_ushort* pIndices = { nullptr };

	m_pIB->Lock(0, 0, reinterpret_cast<void**>(&pIndices), 0);

	/* + x */
	m_iIndices[0] = pIndices[0] = 1; m_iIndices[1] = pIndices[1] = 5; m_iIndices[2] = pIndices[2] = 6;
	m_iIndices[3] = pIndices[3] = 1; m_iIndices[4] = pIndices[4] = 6; m_iIndices[5] = pIndices[5] = 2;

	/* - x */
	m_iIndices[6] = pIndices[6] = 4; m_iIndices[7] = pIndices[7] = 0; m_iIndices[8] = pIndices[8] = 3;
	m_iIndices[9] = pIndices[9] = 4; m_iIndices[10] = pIndices[10] = 3; m_iIndices[11] = pIndices[11] = 7;

	/* + y */
	m_iIndices[12] = pIndices[12] = 4; m_iIndices[13] = pIndices[13] = 5; m_iIndices[14] = pIndices[14] = 1;
	m_iIndices[15] = pIndices[15] = 4; m_iIndices[16] = pIndices[16] = 1; m_iIndices[17] = pIndices[17] = 0;

	/* - y */
	m_iIndices[18] = pIndices[18] = 3; m_iIndices[19] = pIndices[19] = 2; m_iIndices[20] = pIndices[20] = 6;
	m_iIndices[21] = pIndices[21] = 3; m_iIndices[22] = pIndices[22] = 6; m_iIndices[23] = pIndices[23] = 7;

	/* + z */
	m_iIndices[24] = pIndices[24] = 5; m_iIndices[25] = pIndices[25] = 4; m_iIndices[26] = pIndices[26] = 7;
	m_iIndices[27] = pIndices[27] = 5; m_iIndices[28] = pIndices[28] = 7; m_iIndices[29] = pIndices[29] = 6;

	/* - z */
	m_iIndices[30] = pIndices[30] = 0; m_iIndices[31] = pIndices[31] = 1; m_iIndices[32] = pIndices[32] = 2;
	m_iIndices[33] = pIndices[33] = 0; m_iIndices[34] = pIndices[34] = 2; m_iIndices[35] = pIndices[35] = 3;

	/*
	정점 순서는 다 맞음
	2트 
	*/
	m_pIB->Unlock();

	return S_OK;
}

HRESULT CVIBuffer_Cube::Initialize(void* pArg)
{
	return S_OK;
}

/*
* _bool CVIBuffer_Cube::Picking(CTransform* pTransform, _float3* pOut, _float3 vPos, _float3 vLook)
{
	_bool bPicked = false;
	_float3 vClosestPoint = {};
	_float fMinDist = FLT_MAX;

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		_float3 v0 = m_pVertexPositions[m_iIndices[i * 3]];
		_float3 v1 = m_pVertexPositions[m_iIndices[i * 3 + 1]];
		_float3 v2 = m_pVertexPositions[m_iIndices[i * 3 + 2]];

		_float3 vTempOut;
		if (m_pGameInstance->Picking_InLocalSpace(pTransform->Get_WorldMatrixInvPtr(), vPos, vLook, v0, v1, v2, &vTempOut))
		{
			_float3 vWorldOut;
			D3DXVec3TransformCoord(&vWorldOut, &vTempOut, pTransform->Get_WorldMatrixPtr());

			_float3 vDir = vWorldOut - vPos;
			_float fLen = D3DXVec3Length(&vDir);

			if (fLen < fMinDist)
			{
				fMinDist = fLen;
				vClosestPoint = vWorldOut;
				bPicked = true;
			}
		}
	}

	if (bPicked)
	{
		*pOut = vClosestPoint;
	}

	return bPicked;
}
*/
_bool CVIBuffer_Cube::Picking(CTransform* pTransform, _float3* pOut, _float3 vPos, _float3 vLook)
{
	_bool bPicked = false;
	_float3 vClosestPoint = {};
	_float fMinDist = FLT_MAX;

	D3DXVec3Normalize(&vLook, &vLook);

	for (_uint i = 0; i < m_iNumPrimitive; ++i)
	{
		_float3 v0 = m_pVertexPositions[m_iIndices[i * 3]];
		_float3 v1 = m_pVertexPositions[m_iIndices[i * 3 + 1]];
		_float3 v2 = m_pVertexPositions[m_iIndices[i * 3 + 2]];

		_float3 vLocalOut;
		if (m_pGameInstance->Picking_InLocalSpace(pTransform->Get_WorldMatrixInvPtr(), vPos, vLook, v0, v1, v2, &vLocalOut))
		{
			_float3 vWorldOut;
			D3DXVec3TransformCoord(&vWorldOut, &vLocalOut, pTransform->Get_WorldMatrixPtr());

			_float3 vDir = vWorldOut - vPos;
			_float fLen = D3DXVec3Length(&vDir);

			if (fLen < fMinDist)
			{
				fMinDist = fLen;
				vClosestPoint = vWorldOut;
				bPicked = true;
			}
		}
	}

	if (bPicked)
	{
		*pOut = vClosestPoint;
	}

	return bPicked;
}

_float CVIBuffer_Cube::Compute_Height(const _float3& vLocalPos, CTransform* pTransform)
{
	_float3 fResult;
	_float4x4 WorldMat{};
	_float3 vWorldRay = { 0.f, - 1.0f, 0.f };
	_float3 vWorldPos;
	D3DXVec3TransformCoord(&vWorldPos, &vLocalPos, pTransform->Get_WorldMatrixPtr());

	if (Picking(pTransform, &fResult, vWorldPos, vWorldRay))
		return fResult.y;

	else
		return vWorldPos.y - 1.0f;
}

//_float CVIBuffer_Cube::Compute_Height(const _float3& vLocalPos, const _float4x4* vWorldMatInv)
//{
//	_float fHighestY = 0.f, fY = 0.f;
//	_float3 vLocalPosResult = { 0.f, 0.f, 0.f };
//
//	D3DXPLANE Plane = {};
//
//	for (_uint i = 0; i < m_iNumPrimitive; ++i)
//	{
//		_float3 vFirst = m_pVertexPositions[m_iIndices[i * 3]];
//		_float3 vSecond = m_pVertexPositions[m_iIndices[i * 3 + 1]];
//		_float3 vThird = m_pVertexPositions[m_iIndices[i * 3 + 2]];
//
//		D3DXPlaneFromPoints(&Plane, &vFirst, &vSecond, &vThird);
//
//		if (fabsf(Plane.b) < 0.01f)
//			continue;
//		/*
//		생각되는 원인
//
//		1.	로컬상의 레이가 타다가 평면인 상태에서 모서리 부분에서 레이가 평면과 평행하거나 제대로 닿지 않는 상황
//		-> 1번 케이스는 확인 했고ㅓ 아마 아닐거임.
//
//		2,  여러 개의 평면 중 y가 가장 높은 곳을 고르긴 하지만
//			로컬 기준이기 때문에 실제 월드 y를 구해줘야 함
//		-> 오버라이딩이 아니라 각자 내부에서 구현하고 클라단에선 캐스팅해서 구현해줘야 할 듯함
//		-> 터레인 매니저 단에서 로직은 분리해놨으니까 ... 캐스팅했을떄 큰 문제는 없을 것 같음
//
//		*/
//
//		/*
//
//		*/
//		vLocalPosResult.y = (-Plane.a * vLocalPos.x - Plane.c * vLocalPos.z - Plane.d) / Plane.b;
//		D3DXVec3TransformNormal(&vLocalPosResult, &vLocalPosResult, vWorldMatInv);
//
//		if (vLocalPosResult.y > fHighestY)
//			fHighestY = vLocalPosResult.y;
//	}
//
//	return fHighestY;
//}

CVIBuffer_Cube* CVIBuffer_Cube::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CVIBuffer_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Cube::Clone(void* pArg)
{
	CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CVIBuffer_Cube");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Cube::Free()
{
	__super::Free();
}

