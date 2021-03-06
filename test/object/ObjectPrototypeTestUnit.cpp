#include "ObjectPrototypeTestUnit.hpp"

#include "object/object.hpp"
#include "object/object_ptr.hpp"
#include "object/object_store.hpp"
#include "object/prototype_node.hpp"

#include "../Item.hpp"

#include <fstream>

using namespace oos;
using namespace std;

ObjectPrototypeTestUnit::ObjectPrototypeTestUnit()
  : unit_test("prototype", "ObjectStore Prototype Test Unit")
{
  add_test("empty", std::tr1::bind(&ObjectPrototypeTestUnit::empty_store, this), "test empty object store");
  add_test("find", std::tr1::bind(&ObjectPrototypeTestUnit::test_find, this), "find prototype test");
  add_test("one", std::tr1::bind(&ObjectPrototypeTestUnit::one_prototype, this), "one prototype");
  add_test("hierarchy", std::tr1::bind(&ObjectPrototypeTestUnit::prototype_hierachy, this), "prototype hierarchy");
  add_test("iterator", std::tr1::bind(&ObjectPrototypeTestUnit::prototype_traverse, this), "prototype iterator");
  add_test("relation", std::tr1::bind(&ObjectPrototypeTestUnit::prototype_relation, this), "prototype relation");
}

ObjectPrototypeTestUnit::~ObjectPrototypeTestUnit()
{}

void
ObjectPrototypeTestUnit::initialize()
{}

void
ObjectPrototypeTestUnit::finalize()
{}

void
ObjectPrototypeTestUnit::empty_store()
{
  object_store ostore;
}

void
ObjectPrototypeTestUnit::test_find()
{
  object_store ostore;
  ostore.insert_prototype<Item>("item");
  
  prototype_iterator i = ostore.find_prototype<Item>();
  
  UNIT_ASSERT_TRUE(i != ostore.end(), "couldn't find prototype");
}

void
ObjectPrototypeTestUnit::one_prototype()
{
  object_store ostore;

  ostore.insert_prototype<Item>("ITEM");
  
  object *o = ostore.create("ITEM");
  
  UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <Item>");
  
  Item *i = dynamic_cast<Item*>(o);
  
  UNIT_ASSERT_NOT_NULL(i, "couldn't cast object to Item");
  
  delete i;
  
  ostore.remove_prototype("ITEM");
  
  o = ostore.create("ITEM");
  
  UNIT_ASSERT_NULL(o, "unexpected object creation");
}

void
ObjectPrototypeTestUnit::prototype_hierachy()
{
  object_store ostore;
  ostore.insert_prototype<Item>("ITEM");
  ostore.insert_prototype<ItemA, Item>("ITEM_A");
  ostore.insert_prototype<ItemB, Item>("ITEM_B");
  ostore.insert_prototype<ItemC, Item>("ITEM_C");

  object *o = ostore.create("ITEM_B");
  
  UNIT_ASSERT_NOT_NULL(o, "couldn't create object of type <ItemB>");
  
  ItemB *a = dynamic_cast<ItemB*>(o);
  
  UNIT_ASSERT_NOT_NULL(a, "couldn't cast object to ItemB");
  
  delete a;
  
  ostore.remove_prototype("ITEM_B");
  
  o = ostore.create("ITEM_B");
  
  UNIT_ASSERT_NULL(o, "unexpected object creation");
  
  ostore.remove_prototype("ITEM");
  
  o = ostore.create("ITEM_C");
  
  UNIT_ASSERT_NULL(o, "unexpected object creation");
}

void
ObjectPrototypeTestUnit::prototype_traverse()
{
  object_store ostore;
  ostore.insert_prototype<Item>("ITEM");
  ostore.insert_prototype<ItemA, Item>("ITEM_A");
  ostore.insert_prototype<ItemB, Item>("ITEM_B");
  ostore.insert_prototype<ItemC, Item>("ITEM_C");

  prototype_iterator first = ostore.begin();
  prototype_iterator last = ostore.end();
  int count(0);

  while (first != last) {
    UNIT_ASSERT_LESS(count, 5, "prototype count isn't valid");
    ++first;
    ++count;
  }
  
  UNIT_ASSERT_EQUAL(count, 5, "expected prototype size isn't 4");
}

void
ObjectPrototypeTestUnit::prototype_relation()
{
  object_store ostore;
  /*
  ostore.insert_prototype<Item>("item");
  ostore.insert_prototype<ObjectItem<Item>, Item>("object_item");
  ostore.insert_prototype<ItemPtrList>("item_ptr_list");
  ostore.insert_prototype<album>("album");
  */
//  ostore.insert_prototype<playlist>("playlist");
//  ostore.insert_prototype<track>("track");
  
//  ostore.clear(true);

  /*
  ostore.insert_prototype<Item>("item");
  ostore.insert_prototype<ObjectItem<Item>, Item>("object_item");
  ostore.insert_prototype<ItemPtrList>("item_ptr_list");
  ostore.insert_prototype<album>("album");
  */
  ostore.insert_prototype<playlist>("playlist");
  ostore.insert_prototype<album>("album");
  ostore.insert_prototype<track>("track");
  
//  ostore.clear(false);
}
