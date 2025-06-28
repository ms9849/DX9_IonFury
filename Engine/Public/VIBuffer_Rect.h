#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Rect final : public CVIBuffer
{
private:
	CVIBuffer_Rect(LPDIRECT3DDEVICE9 pGraphic_Device);
	CVIBuffer_Rect(const CVIBuffer_Rect& Prototype);
	virtual ~CVIBuffer_Rect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	// CVIBuffer을(를) 통해 상속됨
	_bool Picking(CTransform* pTransform, _float3* pOut, _float3 vPos, _float3 vLook) override;

public:
	static CVIBuffer_Rect* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END