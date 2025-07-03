#include "VIBuffer.h"

#include "GameInstance.h"

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent { pGraphic_Device }
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& Prototype)
	: CComponent{ Prototype }
	, m_pVB { Prototype.m_pVB }
	, m_pIB { Prototype.m_pIB }
	, m_iVertexStride { Prototype.m_iVertexStride }
	, m_iNumVertices { Prototype.m_iNumVertices }
	, m_iFVF { Prototype.m_iFVF }
	, m_ePrimitiveType { Prototype.m_ePrimitiveType }
	, m_iNumPrimitive { Prototype.m_iNumPrimitive }
	, m_iIndexStride { Prototype.m_iIndexStride }
	, m_iNumIndices { Prototype.m_iNumIndices }
	, m_eIndexFormat { Prototype.m_eIndexFormat }
	, m_iNumVerticesX { Prototype.m_iNumVerticesX }
	, m_iNumVerticesZ { Prototype.m_iNumVerticesZ }
  	, m_pVertexPositions {Prototype.m_pVertexPositions }
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iVertexStride);

	m_pGraphic_Device->SetFVF(m_iFVF);

	m_pGraphic_Device->SetIndices(m_pIB);

	m_pGraphic_Device->DrawIndexedPrimitive(m_ePrimitiveType, 0, 0, m_iNumVertices, 0, m_iNumPrimitive);

	return S_OK;
}

_float CVIBuffer::Compute_Height(const _float3& vLocalPos, CTransform* pTransform)
{
	return -1.f;
}

void CVIBuffer::Free()
{
	__super::Free();

	if(false == m_isCloned)
		Safe_Delete_Array(m_pVertexPositions);

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
