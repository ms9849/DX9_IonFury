#include "Client_Defines.h"
#include "UIObject.h"

NS_BEGIN(Engine)
class CTransform;
class CTexture;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CUIUseableItems final : public CUIObject
{
private:
	CUIUseableItems(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUIUseableItems(const CUIUseableItems& Prototype);
	virtual ~CUIUseableItems() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_UseableItems();

private:
	CTexture* m_pTextureCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	class CPlayer* m_pPlayer{ nullptr };
	class CUIText* m_pText{ nullptr };
	_uint m_iUseableItems{};
	UIOBJECT_DESC	m_tagDesc{};

private:
	HRESULT Ready_Components();

public:
	static CUIUseableItems* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END