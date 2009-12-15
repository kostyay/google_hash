#include <iostream>
#include <google/<%= type %>_hash_map>
#include <ruby.h>

using google::<%= type %>_hash_map;      // namespace where class lives by default
using std::cout;
using std::endl;
using __gnu_cxx::hash;  // or __gnu_cxx::hash, or maybe tr1::hash, depending on your OS
extern "C" {

struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return (s1 == s2) || (s1 && s2 && strcmp(s1, s2) == 0);
  }
};


struct eqint
{
  inline bool operator()(int s1, int s2) const
  {
    return s1 == s2;
  }
};

typedef struct {
  <%= type %>_hash_map<int, VALUE> *hash_map;
} RCallback;

static VALUE rb_cGoogleHash<%= type %>;


static void mark_hash_map_values(RCallback incoming) {

  for(<%= type %>_hash_map<int, VALUE>::iterator it = incoming.hash_map->begin(); it != incoming.hash_map->end(); ++it) {
    cout << "marking";
    rb_gc_mark(it->second);
  }

}

static void free_hash_callback(RCallback* cb) {
  delete cb->hash_map;
}

static VALUE callback_alloc _((VALUE)); // what does this line do?

static VALUE
callback_alloc( VALUE klass )
{
    VALUE cb;
    RCallback* cbs;
    cb = Data_Make_Struct(klass, RCallback, mark_hash_map_values, free_hash_callback, cbs);
    cbs->hash_map = new <%= type %>_hash_map<int, VALUE>();
    <% if setup_code %>
	 cbs->hash_map-><%= setup_code %>;
    <% end %> 
    return cb;
}

#define GetCallbackStruct(obj)	(Check_Type(obj, T_DATA), (RCallback*)DATA_PTR(obj))

static VALUE
rb_mri_hash_new(VALUE freshly_created) {

  // we don't actually have anything special to do here...
  return freshly_created;
}


static VALUE rb_ghash_set(VALUE cb, VALUE set_this, VALUE to_this) {
  if(!(TYPE(set_this) == T_FIXNUM)) {
     rb_raise(rb_eTypeError, "not valid value");
  }
  RCallback* cbs = GetCallbackStruct(cb);
  (*cbs->hash_map)[FIX2INT(set_this)] = to_this;  
  return to_this; // ltodo test that it returns value...
}

static VALUE rb_ghash_get(VALUE cb, VALUE get_this) {
  if(!(TYPE(get_this) == T_FIXNUM)) {
     rb_raise(rb_eTypeError, "not valid value");
  }
  RCallback* cbs = GetCallbackStruct(cb);
  VALUE out = (*cbs->hash_map)[FIX2INT(get_this)];
  // todo if out == 0 return Qnil
  return out;
}

void init_<%= type %>() {
    rb_cGoogleHash<%= type %> = rb_define_class("GoogleHash<%= type.capitalize %>", rb_cObject);

    rb_define_alloc_func(rb_cGoogleHash<%= type %>, callback_alloc); // I guess it calls this for us, pre initialize... 

    rb_define_method(rb_cGoogleHash<%= type %>, "initialize", RUBY_METHOD_FUNC(rb_mri_hash_new), 0); 
    rb_define_method(rb_cGoogleHash<%= type %>, "[]=", RUBY_METHOD_FUNC(rb_ghash_set), 2); 
    rb_define_method(rb_cGoogleHash<%= type %>, "[]", RUBY_METHOD_FUNC(rb_ghash_get), 1); 

  } 
}

