/////////////////////////////////////////////////////
//                                                   
//    Generated with QuarcStudio. (ex. ImGui Editor) 
//    QuarcStudio is designed with love by pers0na2. 
//    All rights to QuarcStudio belong to pers0na2.  
//                                                   
/////////////////////////////////////////////////////


#ifndef QUARCTAB
#define QUARCTAB

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#define IM_USE using namespace ImGui; 
#include <imgui_internal.h>
extern ImFont* iconfont;
namespace QGUI 
{

	bool tab(const char* label, bool selected)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);
		const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = ImGui::CalcItemSize({55+0,50+0}, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

		const ImRect bb(pos, pos + size);
		ImGui::ItemSize(size, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		bool hovered, held;
		bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, NULL);

		if (hovered || held)
			ImGui::SetMouseCursor(0);

		if (selected)
		{
			window->DrawList->AddRectFilled(ImVec2(bb.Min.x + 0, bb.Min.y + 48), ImVec2(bb.Max.x + 0, bb.Max.y + 0), ImColor(7,142,246,255),0, 15); 
			window->DrawList->AddRectFilledMultiColor(ImVec2(bb.Min.x + 0, bb.Min.y + 44), ImVec2(bb.Max.x + 0, bb.Max.y + 0), ImColor(7,142,246,0),ImColor(7,142,246,0),ImColor(7,142,246,255),ImColor(7,142,246,255)); 
		}
		else
		{
			if (!hovered)
			{
			}
			else
			{
			}
		}


		if(selected)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229/ 255.f , 229/ 255.f ,229/ 255.f ,255/ 255.f ));
            ImGui::PushFont(iconfont);
			ImGui::RenderText(ImVec2(bb.Min.x + 0 + (55 / 2 - label_size.x / 2) - 5, bb.Min.y + size.y / 2 - label_size.y / 2 - 5), label);
            ImGui::PopFont();
			ImGui::PopStyleColor();
		}
		else
		{
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(229 / 255.f, 229 / 255.f, 229 / 255.f, 255 / 255.f));
            ImGui::PushFont(iconfont);
            ImGui::RenderText(ImVec2(bb.Min.x + 0 + (55 / 2 - label_size.x / 2) - 5, bb.Min.y + size.y / 2 - label_size.y / 2 - 5), label);
            ImGui::PopFont();
            ImGui::PopStyleColor();
		}
		return pressed;
	}

}

#endif
