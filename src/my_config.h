#ifndef _MY_CONFIG_H_
#define _MY_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

  struct MyLinkList;
  struct MyNode;

  typedef struct MyConfigItem {
    char *key;
    char *value;

    // set key to new_key
    void (*set_key)(struct MyConfigItem *self, const char *new_key);

    // set value to new_value
    void (*set_value)(struct MyConfigItem *self, const char *new_value);

    // destroy all resources that the item holds
    void (*destroy)(struct MyConfigItem *self);
  } MyConfigItem;

  typedef struct MyConfigItemItor {
    struct MyLinkList *item_list;
    struct MyNode *current;

    // return the first item of the section or the same value of end() if there are no items at all
    MyConfigItem* (*begin)(struct MyConfigItemItor *self);

    // return the next item
    MyConfigItem* (*next)(struct MyConfigItemItor *self);

    // a guard item that is not part of the items, to indicate the end
    MyConfigItem* (*end)(struct MyConfigItemItor *self);

    int (*remove_current)(struct MyConfigItemItor *self);

    void (*destroy)(struct MyConfigItemItor *self);
  } MyConfigItemItor;

  typedef struct MyConfigSection {
    char *name;
    struct MyLinkList *item_list;

    // create a item iterator, the returned result must be free manually
    MyConfigItemItor* (*create_item_itor)(struct MyConfigSection *self);

    // query an item via its key
    MyConfigItem* (*get_item)(struct MyConfigSection *self, const char *key);

    // create an item
    MyConfigItem* (*create_item)(struct MyConfigSection *self, const char *key, const char *value);

    // delete an item via its key
    int (*delete_item)(struct MyConfigSection *self, const char *key);

    // change the item's key
    MyConfigItem* (*rename_item)(struct MyConfigSection *self, const char *old_key, const char *new_key);

    void (*destroy)(struct MyConfigSection *self);

  } MyConfigSection;

  typedef struct MyConfigSectionItor {
    struct MyLinkList *section_list;
    struct MyNode *current;

    MyConfigSection* (*begin)(struct MyConfigSectionItor *self);
    MyConfigSection* (*next)(struct MyConfigSectionItor *self);
    MyConfigSection* (*end)(struct MyConfigSectionItor *self);
    int (*remove_current)(struct MyConfigSectionItor *self);
    void (*destroy)(struct MyConfigSectionItor *self);
  } MyConfigSectionItor;

  typedef struct MyConfig {
    // must be "\n" or "\r\n". default is empty string. it will be set when load config file.
    // if it still an empty string when saving, the program will use "\n" as newline.
    // set it manually before load_from if you already know what newline the config file uses,
    // otherwise leave it to be empty so the program will set it when loading config file.
    char newline[3];

    // must be '=' or ':'. default is '\0'. it will be set when load config file.
    // set it manually before load_from if you know the what delimiter the config file uses.
    char delimiter;

    struct MyLinkList *section_list;

    // load config from file
    int (*load_from)(struct MyConfig *self, const char *filepath);

    // save current config to file
    int (*save_as)(struct MyConfig *self, const char *filepath);

    // query and return section via its name
    MyConfigSection* (*get_section)(struct MyConfig *self, const char *section_name);

    // create a new section
    MyConfigSection* (*create_section)(struct MyConfig *self, const char *section_name);

    // delete a section
    int (*delete_section)(struct MyConfig *self, const char *section_name);

    // rename a section
    MyConfigSection* (*rename_section)(struct MyConfig *self, const char *old_name, const char *new_name);

    // create a iterator to iterate all sections
    MyConfigSectionItor* (*create_section_itor)(struct MyConfig *self);

    // change newline string, must be less than two characters; should be "\n" or "\r\n"
    int (*set_newline)(struct MyConfig *self, const char *newline);

    // clear all data of the config object self. if you want to load another file, make sure to
    // call clear before call load_from.
    void (*clear)(struct MyConfig *self);

    // destroy @self. must not access this variable and its member after destroy
    void (*destroy)(struct MyConfig *self);
  } MyConfig;

  MyConfig* myconfig_create();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _MY_CONFIG_H_
