/////////////////////////////////////////////////////
//                                                   
//    Generated with QuarcStudio. (ex. ImGui Editor) 
//    QuarcStudio is designed with love by pers0na2. 
//    All rights to QuarcStudio belong to pers0na2.  
//                                                   
/////////////////////////////////////////////////////


#ifndef QUARCCOMBO
#define QUARCCOMBO

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#define IM_USE using namespace ImGui; 
#include <imgui_internal.h>

namespace QGUI 
{

	static float CalcMaxPopupHeightFromItemCount(int items_count)
	{
		ImGuiContext & g = *GImGui; 
		if (items_count <= 0)
			return FLT_MAX; 
		return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2); 
	}

	bool combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
	bool combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);
	bool combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1);

	bool begincombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
	{
		IM_USE;
		ImGuiContext& g = *GImGui;
		bool has_window_size_constraint = (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint) != 0;
		g.NextWindowData.Flags &= ~ImGuiNextWindowDataFlags_HasSizeConstraint;

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : ImGui::GetFrameHeight();
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
		const float expected_w = ImGui::CalcItemWidth();
		const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size : expected_w;

		const ImRect frame_bb(window->DC.CursorPos + ImVec2(15, 0), window->DC.CursorPos + ImVec2(w +0, 16));
		const ImRect total_bb(window->DC.CursorPos - ImVec2(0, 15), frame_bb.Max);

		ImGui::ItemSize(total_bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(frame_bb, id, &hovered, &held);
		bool popup_open = ImGui::IsPopupOpen(id, ImGuiPopupFlags_None);

		if (hovered || held)
			ImGui::SetMouseCursor(0);

		const float value_x2 = ImMax(frame_bb.Min.x, frame_bb.Max.x - arrow_size);

			window->DrawList->AddRectFilled(ImVec2(frame_bb.Min.x + 0, frame_bb.Min.y + 0), ImVec2(frame_bb.Max.x + 0, frame_bb.Max.y + 0), ImColor(0.137255f,0.137255f,0.137255f,1.000000f),0, 15); 
		if (!(flags & ImGuiComboFlags_NoArrowButton))
		{
			ImU32 text_col = ImGui::GetColorU32(ImGuiCol_Text);
				if (value_x2 + arrow_size - style.FramePadding.x <= frame_bb.Max.x)
					ImGui::RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y + 0,frame_bb.Min.y + style.FramePadding.y +0), text_col, ImGuiDir_Down,0.7);
		}

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, 255 / 255.f));
		if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
			ImGui::RenderText(frame_bb.Min + ImVec2(5, 16 / 2 - ImGui::CalcTextSize(preview_value).y / 2), preview_value);
		ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, 255 / 255.f));
		if (label_size.x > 0)
			ImGui::RenderText(ImVec2(frame_bb.Min.x, total_bb.Min.y - 1), label);
		ImGui::PopStyleColor();

		if ((pressed || g.NavActivateId == id) && !popup_open)
		{
			if (window->DC.NavLayerCurrent == 0)
				window->NavLastIds[0] = id;
			ImGui::OpenPopupEx(id, ImGuiPopupFlags_None);
			popup_open = true;
		}

		if (!popup_open)
			return false;

		if ((flags & ImGuiComboFlags_HeightMask_) == 0)
			flags |= ImGuiComboFlags_HeightRegular;
		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_));
		int popup_max_height_in_items = -1;
		if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
		else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
		else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
		ImGui::SetNextWindowSizeConstraints(ImVec2(w - 15, 0.0f), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

		char name[16];
		ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size);

		if (ImGuiWindow * popup_window = ImGui::FindWindowByName(name))
			if (popup_window->WasActive)
			{
				ImVec2 size_expected = ImGui::CalcWindowExpectedSize(popup_window);
				if (flags & ImGuiComboFlags_PopupAlignLeft)
					popup_window->AutoPosLastDirection = ImGuiDir_Left;
				ImRect r_outer = ImGui::GetWindowAllowedExtentRect(popup_window);
				ImVec2 pos = ImGui::FindBestWindowPosForPopupEx(frame_bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, frame_bb, ImGuiPopupPositionPolicy_ComboBox);
				ImGui::SetNextWindowPos(pos);
			}

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(style.FramePadding.x, style.WindowPadding.y));
		bool ret = ImGui::Begin(name, NULL, window_flags);
		ImGui::PopStyleVar();

		if (!ret)
		{
			ImGui::EndPopup();
			return false;
		}
		return true;
	}

	static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
	{
		const char* const* items = (const char* const*)data;
		if (out_text)
			*out_text = items[idx];
		return true;
	}

	static bool Items_SingleStringGetter(void* data, int idx, const char** out_text)
	{
		// FIXME-OPT: we could pre-compute the indices to fasten this. But only 1 active combo means the waste is limited.
		const char* items_separated_by_zeros = (const char*)data;
		int items_count = 0;
			const char* p = items_separated_by_zeros;
			while (*p)
			{
			if (idx == items_count)
				break;
			p += strlen(p) + 1;
			items_count++;
		}
		if (!*p)
			return false;
		if (out_text)
			*out_text = p;
		return true;
	}

	bool combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
	{
		IM_USE;
		ImGuiContext& g = *GImGui;
			const char* preview_value = NULL;
		if (*current_item >= 0 && *current_item < items_count)
			items_getter(data, *current_item, &preview_value);
		if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
			SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));
		if (!begincombo(label, preview_value, ImGuiComboFlags_None))
			return false;
		bool value_changed = false;

		for (int i = 0; i < items_count; i++)
		{
			PushID((void*)(intptr_t)i);
			const bool item_selected = (i == *current_item);
			const char* item_text;
			if (!items_getter(data, i, &item_text))
				item_text = " * Unknown item * ";
			if (Selectable(item_text, item_selected))
			{
			value_changed = true;
				*current_item = i;
			}
			if (item_selected)
				SetItemDefaultFocus(); 
			PopID();
		}

		EndCombo();
		return value_changed;
	}

	bool combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
	{
		const bool value_changed = combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
		return value_changed;
	}

	bool combo(const char* label, int* current_item, const char* items_separated_by_zeros, int height_in_items)
	{
		int items_count = 0;
		const char* p = items_separated_by_zeros;

		while (*p)
		{
			p += strlen(p) + 1;
			items_count++;
		}

		bool value_changed = combo(label, current_item, Items_SingleStringGetter, (void*)items_separated_by_zeros, items_count, height_in_items);
		return value_changed;
	}

}

#endif
