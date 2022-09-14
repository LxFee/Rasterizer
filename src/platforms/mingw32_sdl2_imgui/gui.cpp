#include "gui.h"

#include <string>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"
#include "platform.h"

item_t::item_t(const std::string& _name, item_type_t type, void* _data,
               float mi, float mx)
    : name(_name), data(_data), type(type), mi(mi), mx(mx) {}

group_t::group_t(const std::string& _name, std::vector<item_t> _items)
    : name(_name), items(_items) {}

widget_t::widget_t(const std::string& _title, std::vector<group_t> _groups)
    : title(_title), groups(_groups) {}

void draw_gui(window_t* window, widget_t* widget) {
    if(!widget) return;
    ImGuiContext* ctx = (ImGuiContext*)window_get_gui_context(window);
    if(!ctx) return;
    int id = 0;
    ImGui::SetCurrentContext(ctx);
    ImGui::Begin(widget->title.c_str());
    for(auto& group : widget->groups) {
        /* using PushID() to add unique identifier in the ID stack, and changing
         * style.*/
        /* issuse: https://github.com/ocornut/imgui/issues/2523 */
        /* assert fail: g.ActiveId == id || g.ActiveId == 0 || g.DragDropActive
         */
        ImGui::PushID(id++);
        if(ImGui::CollapsingHeader(group.name.c_str())) {
            for(auto& item : group.items) {
                switch(item.type) {
                    case ITEM_TYPE_FLOAT:
                        ImGui::SliderFloat(item.name.c_str(), (float*)item.data,
                                           item.mi, item.mx);
                        break;
                    case ITEM_TYPE_FLOAT2:
                        ImGui::SliderFloat2(item.name.c_str(),
                                            (float*)item.data, item.mi,
                                            item.mx);
                        break;
                    case ITEM_TYPE_FLOAT3:
                        ImGui::SliderFloat3(item.name.c_str(),
                                            (float*)item.data, item.mi,
                                            item.mx);
                        break;
                    case ITEM_TYPE_COLOR3:
                        ImGui::ColorEdit3(item.name.c_str(), (float*)item.data);
                        break;
                    case ITEM_TYPE_COLOR4:
                        ImGui::ColorEdit4(item.name.c_str(), (float*)item.data);
                        break;
                }
            }
        }
        ImGui::PopID();
    }
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}