#ifndef RASTERIZER_GUI_H_
#define RASTERIZER_GUI_H_

#include <string>
#include <vector>
#include "platform.h"

typedef enum {ITEM_TYPE_FLOAT, ITEM_TYPE_FLOAT2, ITEM_TYPE_FLOAT3, ITEM_TYPE_COLOR3, ITEM_TYPE_COLOR4} item_type_t;

struct item_t {
    item_t(const std::string& _name, item_type_t type, void* _data, float mi = 0.0f, float mx = 0.0f);
    item_type_t get_type() const ;
    std::string name;
    item_type_t type;
    void* data;
    float mi, mx;
};

struct group_t {
    group_t(const std::string& _name, std::vector<item_t> _items);
    std::string name;
    std::vector<item_t> items;
};

struct widget_t {
    widget_t(const std::string& _title, std::vector<group_t> _groups);
    std::string title;
    std::vector<group_t> groups;
};

void draw_gui(window_t* window, widget_t* widget);

#endif // RASTERIZER_GUI_H_