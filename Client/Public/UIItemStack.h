#include "Client_Defines.h"
#include "UIObject.h"
#include "UIFont.h"
#include "UIText.h"


NS_BEGIN(Engine)
class CTransform;
class CVIBuffer_Rect;
NS_END

NS_BEGIN(Client)

class CUIItemStack final : public CUIObject
{
private:
	CUIItemStack(LPDIRECT3DDEVICE9 pGraphic_Device);
	CUIItemStack(const CUIItemStack& Prototype);
	virtual ~CUIItemStack() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Set_ItemStack(const _tchar* strText);

private:
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

private:
	CUIText* m_pText{ nullptr };
	UIOBJECT_DESC	m_tagDesc{};

private:
	HRESULT Ready_Components();

public:
	static CUIItemStack* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END