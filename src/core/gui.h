#ifndef RASTERIZER_GUI_H_
#define RASTERIZER_GUI_H_

#include <string>
#include <vector>

typedef enum {ITEM_TYPE_FLOAT, ITEM_TYPE_FLOAT2, ITEM_TYPE_FLOAT3, ITEM_TYPE_FPS_TEXT} item_type_t;

struct item_t {
    item_t(const std::string& _name, item_type_t _type, void* _data);
    std::string name;
    item_type_t type;
    void* data;
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

void draw_gui(widget_t* widget);

#endif // RASTERIZER_GUI_H_