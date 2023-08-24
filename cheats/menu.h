#pragma once
#include "../includes.hpp"

class c_menu : public singleton<c_menu> {
public:
	void draw( bool is_open );
	void menu_setup(ImGuiStyle &style);
	void render2(bool is_open);

	float dpi_scale = 1.f;

	ImFont* futura;
	ImFont* futura_large;
	ImFont* astrium;
	ImFont* icons;
	ImFont* bigxd;
	ImFont* futura_small;
	ImFont* gotham;
	ImFont* ico_menu;
	ImFont* ico_bottom;
	ImFont* weapon_icons;
	ImFont* weapon_icons2;
	ImFont* interfaces;
	ImFont* timersz;
	ImFont* isis;
	ImFont* smallest_pixel;
	ImFont* font;

	ImFont* medium;
	ImFont* bold;
	ImFont* tab_icons;
	ImFont* logo;
	ImFont* tab_title;
	ImFont* tab_title_icon;
	ImFont* subtab_title;
	ImFont* combo_arrow;

	float public_alpha;
	IDirect3DDevice9* device;
	float color_buffer[4] = { 1.f, 1.f, 1.f, 1.f };
private:
	struct {
		ImVec2 WindowPadding;
		float  WindowRounding;
		ImVec2 WindowMinSize;
		float  ChildRounding;
		float  PopupRounding;
		ImVec2 FramePadding;
		float  FrameRounding;
		ImVec2 ItemSpacing;
		ImVec2 ItemInnerSpacing;
		ImVec2 TouchExtraPadding;
		float  IndentSpacing;
		float  ColumnsMinSpacing;
		float  ScrollbarSize;
		float  ScrollbarRounding;
		float  GrabMinSize;
		float  GrabRounding;
		float  TabRounding;
		float  TabMinWidthForUnselectedCloseButton;
		ImVec2 DisplayWindowPadding;
		ImVec2 DisplaySafeAreaPadding;
		float  MouseCursorScale;
	} styles;

	bool update_dpi = false;
	bool update_scripts = false;
	void dpi_resize(float scale_factor, ImGuiStyle &style);

	int active_tab_index;
	ImGuiStyle style;
	int width = 850, height = 560;
	float child_height;

	float preview_alpha = 1.f;

	int active_tab;

	int rage_section;
	int legit_section;
	int visuals_section;
	int players_section;
	int misc_section;
	int settings_section;

	// we need to use 'int child' to seperate content in 2 childs

	ImGuiStyle        _style;
	 
	int current_profile = -1;

	std::string preview = crypt_str("None");
};

class IRefCounted
{
public:
	virtual int AddReference() = 0;
	virtual int Release() = 0;
};


struct LightDesc_t;
class C_StudioRender
{
private:
	template <typename T, typename ... args_t>
	constexpr T CallVFunc(void* thisptr, std::size_t nIndex, args_t... argList)
	{
		using VirtualFnxz = T(__thiscall*)(void*, decltype(argList)...);
		return (*static_cast<VirtualFnxz**>(thisptr))[nIndex](thisptr, argList...);
	}
public:
	void SetAmbientLightColors(const Vector4D* pAmbientOnlyColors)
	{
		CallVFunc<void>(this, 20, pAmbientOnlyColors);
	}
	void SetAmbientLightColors(const Vector* pAmbientOnlyColors)
	{
		CallVFunc<void>(this, 21, pAmbientOnlyColors);
	}
	void SetLocalLights(int nLights, const LightDesc_t* pLights)
	{
		CallVFunc<void>(this, 22, nLights, pLights);
	}

	void SetColorModulation(float const* arrColor)
	{
		CallVFunc<void>(this, 27, arrColor);
	}

	void SetAlphaModulation(float flAlpha)
	{
		CallVFunc<void>(this, 28, flAlpha);
	}

	void ForcedMaterialOverride(IMaterial* pMaterial, OverrideType_t nOverrideType = 0, int nOverrides = 0)
	{
		CallVFunc<void>(this, 33, pMaterial, nOverrideType, nOverrides);
	}
};

inline C_StudioRender* m_StudioRender = new C_StudioRender();