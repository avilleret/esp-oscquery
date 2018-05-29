#include "ossia.h"
#include <sstream>

namespace ossia
{

node::node(const std::string& name, node* parent) :
  m_name(name), m_parent(parent)
{
}

node& node::add_parameter(){
  m_has_parameter = true;
  return *this;
}

void node::remove_parameter(){
  m_has_parameter = false;
}

node& node::add_child(const std::string& name)
{
  m_children.push_back(std::move(node{name,this}));
  return m_children.back();
}

char* node::to_string()
{
  char* string;
  cJSON *json = cJSON_CreateObject();
  to_json(json);

  string = cJSON_Print(json);

  cJSON_Delete(json);
  return string;
}

std::string node::get_fullpath() const
{
  std::vector<std::string> list;
  list.push_back(get_name());

  const node* n = get_parent();
  while(n)
  {
    list.push_back(n->get_name());
    n = n->get_parent();
  }
  std::stringstream fp;

  for (std::vector<std::string>::reverse_iterator i = list.rbegin();
          i != list.rend(); ++i ) {
    fp << "/" << *i;
  }

  return fp.str();
}

void node::to_json(cJSON *json) const
{
  if(!json)
    return;

  cJSON_AddItemToObject(json, "FULL_PATH", cJSON_CreateString(get_fullpath().c_str()));
  cJSON_AddItemToObject(json, "ACCESS", cJSON_CreateIntArray({0},1));

  if (m_has_parameter)
  {
    cJSON_AddItemToObject(json, "ACCESS", cJSON_CreateNumber(1));
    cJSON_AddItemToObject(json, "TYPE", cJSON_CreateString("f"));
    cJSON_AddItemToObject(json, "VALUE", cJSON_CreateNumber(m_value));
  }

  if(!m_children.empty())
  {
    cJSON * contents = cJSON_CreateObject();

    for (const auto& c : m_children)
    {
      cJSON * obj = cJSON_CreateObject();
      c.to_json(obj);
      cJSON_AddItemToObject(contents, c.get_name().c_str(), obj);
    }

    cJSON_AddItemToObject(json, "CONTENTS", contents);
  }

}
}

