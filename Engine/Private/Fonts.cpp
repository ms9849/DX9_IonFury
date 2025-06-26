#include "Fonts.h"

CFonts::CFonts(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent{ pGraphic_Device }
{
}

CFonts::CFonts(const CFonts& Prototype)
	: CComponent{ Prototype }
	, m_iNumTextures{ Prototype.m_iNumTextures }
	, m_Fonts{ Prototype.m_Fonts }
{
	for (auto& pFont : m_Fonts)
		Safe_AddRef(pFont.second);
}

HRESULT CFonts::Initialize_Prototype(TEXTURE eType, const _tchar* pTextureFilePath, const _uint strFonts[], _uint iNumTextures)
{
	for (size_t i = 0; i < iNumTextures; i++)
	{
		LPDIRECT3DBASETEXTURE9		pTexture = { nullptr };
		_tchar		szFileName[MAX_PATH] = {};

		wsprintf(szFileName, pTextureFilePath, strFonts[i]);

		HRESULT hr = D3DXCreateTextureFromFile(m_pGraphic_Device, szFileName, reinterpret_cast<LPDIRECT3DTEXTURE9*>(&pTexture));

		if (FAILED(hr))
			return E_FAIL;

		m_Fonts.emplace(strFonts[i], pTexture);
	}

	return S_OK;
}

HRESULT CFonts::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CFonts::Set_Font(_uint iKey)
{
	int a = 10;
	auto	iter = m_Fonts.find(iKey);
	if (iter == m_Fonts.end())
		return E_FAIL;

	m_pGraphic_Device->SetTexture(0, iter->second);

	return S_OK;
}

CFonts* CFonts::Create(LPDIRECT3DDEVICE9 pGraphic_Device, TEXTURE eType, const _tchar* pTextureFilePath, const _uint strFonts[], _uint iNumTextures)
{
	CFonts* pInstance = new CFonts(pGraphic_Device);

	if (FAILED(pInstance->Initialize_Prototype(eType, pTextureFilePath, strFonts, iNumTextures)))
	{
		MSG_BOX("Failed to Created : CFonts");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CFonts::Clone(void* pArg)
{
	CFonts* pInstance = new CFonts(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFonts");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFonts::Free()
{
	__super::Free();

	for (auto& pFont : m_Fonts)
		Safe_Release(pFont.second);

	m_Fonts.clear();
}
