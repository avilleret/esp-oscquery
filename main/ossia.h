#pragma once
#include <string>
#include <vector>
#include "cJSON.h"

namespace ossia {
class node {
  public:
    node(const std::string& name = "", node* parent = nullptr);
    node& add_parameter();
    void remove_parameter();
    node& add_child(const std::string& name);
    void to_json(cJSON* json) const;
    char* to_string();
    const node* get_parent() const { return m_parent;}
    const std::string& get_name() const { return m_name;}
    std::string get_fullpath() const;

    void set_value(float f) {m_value = f;}
    float get_value(){ return m_value;}

  private:
    std::string m_name{};
    node* m_parent{nullptr};
    std::vector<node> m_children{};
    bool m_has_parameter{false};
    float m_value{0};
};
}
