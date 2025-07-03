#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cube final : public CVIBuffer
{
private:
	CVIBuffer_Cube(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Cube(const CVIBuffer_Cube& Prototype);
	virtual ~CVIBuffer_Cube() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	// CVIBuffer을(를) 통해 상속됨
	_bool Picking(CTransform* pTransform, _float3* pOut, _float3 vPos, _float3 vLook) override;
	virtual _float Compute_Height(const _float3& vLocalPos, class CTransform* pTransform = nullptr) override;

private:
	_ushort m_iIndices[36] = {};

public:
	static CVIBuffer_Cube* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END