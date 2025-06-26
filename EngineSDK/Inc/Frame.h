#pragma once

#include "Component.h"

/* Test%d.png */

/* 텍스쳐들을 들고 있는다.  */

NS_BEGIN(Engine)

class ENGINE_DLL CFrame final : public CComponent
{
private:
	CFrame(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFrame(const CFrame& Prototype);
	virtual ~CFrame() = default;

public:
	virtual HRESULT Initialize_Prototype(TEXTURE eType, const _tchar* pTextureFilePath, const _uint strFonts[], _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Set_Frame(const _tchar* strKey);

private:
	LPDIRECT3DBASETEXTURE9			m_Frame;

public:
	static CFrame* Create(LPDIRECT3DDEVICE9 pGraphic_Device, TEXTURE eType, const _tchar* pTextureFilePath, const _uint strFonts[], _uint iNumTextures);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END