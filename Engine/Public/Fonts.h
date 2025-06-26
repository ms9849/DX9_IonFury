#pragma once

#include "Component.h"

/* Test%d.png */

/* 텍스쳐들을 들고 있는다.  */

NS_BEGIN(Engine)

class ENGINE_DLL CFonts final : public CComponent
{
private:
	CFonts(LPDIRECT3DDEVICE9 pGraphic_Device);
	CFonts(const CFonts& Prototype);
	virtual ~CFonts() = default;

public:
	virtual HRESULT Initialize_Prototype(TEXTURE eType, const _tchar* pTextureFilePath, const _uint strFonts[], _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg);

public:
	HRESULT Set_Font(_uint strKey);

private:
	_uint										m_iNumTextures = {};
	map<_uint, LPDIRECT3DBASETEXTURE9>	m_Fonts;

public:
	static CFonts* Create(LPDIRECT3DDEVICE9 pGraphic_Device, TEXTURE eType, const _tchar* pTextureFilePath, const _uint strFonts[], _uint iNumTextures);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END