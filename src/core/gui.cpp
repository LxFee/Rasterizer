#include "gui.h"
#include <string>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_sdlrenderer.h"
#include "image.h"

item_t::item_t(const std::string& _name, item_type_t _type, void* _data)
    : name(_name), type(_type), data(_data) {}

group_t::group_t(const std::string& _name, std::vector<item_t> _items)
    : name(_name), items(_items) {}

widget_t::widget_t(const std::string& _title, std::vector<group_t> _groups)
    : title(_title), groups(_groups) {}


void draw_gui(widget_t* widget) {

}