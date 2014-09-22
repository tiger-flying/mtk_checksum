#include "my_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "my_linklist.h"


/* MyConfigItem Begin */

static void item_set_key(MyConfigItem *self, const char *new_key)
{
  int new_len = strlen(new_key);
  int old_len = 0;

  if (self->key != NULL) {
    old_len = strlen(self->key);
  }

  char *new_buf = NULL;
  if (new_len > old_len) {
    new_buf = (char *)malloc(new_len + 1);
    if (new_buf == NULL) {
      fprintf(stderr, "failed to malloc buffer for new key\n");
      return;
    }
    free(self->key);
    self->key = new_buf;
  }

  strcpy(self->key, new_key);
}

static void item_set_value(MyConfigItem *self, const char *new_value)
{
  int new_len = strlen(new_value);
  int old_len = 0;

  if (self->value != NULL) {
    old_len = strlen(self->value);
  }

  char *new_buf = NULL;
  if (new_len > old_len) {
    new_buf = (char *)malloc(new_len + 1);
    if (new_buf == NULL) {
      fprintf(stderr, "failed to malloc buffer for new value\n");
      return;
    }
    free(self->value);
    self->value = new_buf;
  }

  strcpy(self->value, new_value);
}

static void item_release(MyConfigItem *self)
{
  if (self->key != NULL) {
    free(self->key);
    self->key = NULL;
  }

  if (self->value != NULL) {
    free(self->value);
    self->value = NULL;
  }
}

static void item_destroy(MyConfigItem *self)
{
  item_release(self);
  free(self);
}

static void init_item(MyConfigItem *item)
{
  item->key = NULL;
  item->value = NULL;

  item->set_value = &item_set_value;
  item->set_key = &item_set_key;
  item->destroy = &item_destroy;
}
/* MyConfigItem End */



/* MyConfigItemItor Begin */
static MyConfigItem* section_item_itor_begin(MyConfigItemItor *self)
{
  self->current = self->item_list->head->next;
  if (self->current == self->item_list->end)
    return NULL;

  return self->current->data;
}

static MyConfigItem* section_item_itor_next(MyConfigItemItor *self)
{
  self->current = self->current->next;

  return self->current->data;
}

static MyConfigItem* section_item_itor_end(MyConfigItemItor *self)
{
  return NULL;
}

static int section_item_itor_remove_current(MyConfigItemItor *self)
{
  if (self->current == NULL || self->current == self->item_list->end)
    return -1;

  MyNode *node = self->current;
  self->current = node->prev;
  self->item_list->remove(self->item_list, node);

  return 0;
}

static void section_item_itor_destroy(MyConfigItemItor *self)
{
  free(self);
}

static void init_section_item_itor(MyConfigItemItor *itor)
{
  itor->item_list = NULL;

  itor->begin = &section_item_itor_begin;
  itor->next = &section_item_itor_next;
  itor->end = &section_item_itor_end;
  itor->remove_current = &section_item_itor_remove_current;
  itor->destroy = &section_item_itor_destroy;
}
/* MyConfigItemItor End */

/* MyConfigSection Begin */
static MyConfigItemItor* section_create_item_itor(MyConfigSection *self)
{
  MyConfigItemItor *itor = (MyConfigItemItor *)malloc(sizeof(MyConfigItemItor));
  assert(itor != NULL);

  init_section_item_itor(itor);
  itor->item_list = self->item_list;

  return itor;
}

static MyConfigItem* section_get_item(MyConfigSection *self, const char *key)
{
  MyConfigItemItor *itor = self->create_item_itor(self);
  MyConfigItem *item = NULL;
  for (item = itor->begin(itor); item != itor->end(itor); item = itor->next(itor)) {
    if (0 == strcmp(item->key, key))
      break;
  }

  itor->destroy(itor);
  return item;
}

static MyConfigItem* section_create_item(MyConfigSection *self, const char *key, const char *value)
{
  MyConfigItem *item = self->get_item(self, key);
  if (item != NULL) {
    return NULL;
  }

  item = (MyConfigItem *)malloc(sizeof(MyConfigItem));
  if (item == NULL) {
    fprintf(stderr, "failed to malloc item\n");
    return NULL;
  }

  init_item(item);
  item->set_key(item, key);
  item->set_value(item, value);

  self->item_list->append(self->item_list, item);

  return item;
}

static int section_delete_item(MyConfigSection *self, const char *key)
{
  MyConfigItemItor *itor = self->create_item_itor(self);
  MyConfigItem *item = NULL;

  for (item = itor->begin(itor); item != itor->end(itor); item = itor->next(itor)) {
    if (0 == strcmp(item->key, key)) {
      itor->remove_current(itor);
      return 0;
    }
  }

  return -1;
}

static MyConfigItem* section_rename_item(MyConfigSection *self, const char *old_key, const char *new_key)
{
  MyConfigItem *item = self->get_item(self, old_key);
  if (item == NULL) {
    return NULL;
  }

  MyConfigItem *item2 = self->get_item(self, new_key);
  if (item2 != NULL) {
    return NULL;
  }

  item->set_key(item, new_key);

  return item;
}

static void section_release(MyConfigSection *self)
{
  if (self->name != NULL) {
    free(self->name);
    self->name = NULL;
  }

  self->item_list->release(self->item_list);
}

static void section_destroy(MyConfigSection *self)
{
  section_release(self);
  self->item_list->destroy(self->item_list);
  free(self);
}

static void init_section(MyConfigSection *section)
{
  section->name = NULL;
  section->item_list = create_linklist();

  section->create_item_itor = &section_create_item_itor;
  section->get_item = &section_get_item;
  section->create_item = &section_create_item;
  section->delete_item = &section_delete_item;
  section->rename_item = &section_rename_item;
  section->destroy = &section_destroy;
}
/* MyConfigSection End */



/* MyConfigSectionItor Begin */
static MyConfigSection* config_section_itor_begin(MyConfigSectionItor *self)
{
  self->current = self->section_list->head->next;
  if (self->current == self->section_list->end)
    return NULL;

  return self->current->data;
}

static MyConfigSection* config_section_itor_next(MyConfigSectionItor *self)
{
  self->current = self->current->next;
  return self->current->data;
}

static MyConfigSection* config_section_itor_end(MyConfigSectionItor *self)
{
  return NULL;
}

static int config_section_itor_remove_current(MyConfigSectionItor *self)
{
  if (self->current == NULL || self->current == self->section_list->end)
    return -1;

  MyNode *node = self->current;
  self->current = self->current->prev;
  self->section_list->remove(self->section_list, node);

  return 0;
}

static void config_section_itor_destroy(MyConfigSectionItor *self)
{
  free(self);
}

static void init_config_section_itor(MyConfigSectionItor *itor)
{
  itor->section_list = NULL;
  itor->current = NULL;

  itor->begin = &config_section_itor_begin;
  itor->next = &config_section_itor_next;
  itor->end = &config_section_itor_end;
  itor->remove_current = &config_section_itor_remove_current;
  itor->destroy = &config_section_itor_destroy;
}
/* MyConfigSectionItor End */

/* MyConfig Begin */

static MyConfigSectionItor* config_create_section_itor(MyConfig *self)
{
  MyConfigSectionItor *itor = (MyConfigSectionItor *)malloc(sizeof(MyConfigSectionItor));
  assert(itor != NULL);

  init_config_section_itor(itor);
  itor->section_list = self->section_list;

  return itor;
}

static MyConfigSection* config_get_section(MyConfig *self, const char *section_name)
{
  MyConfigSectionItor *itor = self->create_section_itor(self);
  MyConfigSection *section = NULL;

  for (section = itor->begin(itor); section != itor->end(itor); section = itor->next(itor)) {
    if (0 == strcmp(section->name, section_name)) {
      break;
    }
  }

  itor->destroy(itor);

  return section;
}

static MyConfigSection* config_create_section(MyConfig *self, const char *section_name)
{
  MyConfigSection *section = self->get_section(self, section_name);
  if (section != NULL) {
    return NULL;
  }

  section = (MyConfigSection *)malloc(sizeof(MyConfigSection));
  if (section == NULL) {
    fprintf(stderr, "failed to malloc section\n");
    return NULL;
  }
  init_section(section);

  int len = strlen(section_name);
  section->name = (char *)malloc(len + 1);
  if (section->name == NULL) {
    fprintf(stderr, "failed to malloc section name\n");
    free(section);
    return NULL;
  }

  strcpy(section->name, section_name);

  self->section_list->append(self->section_list, section);

  return section;
}

static int config_delete_section(MyConfig *self, const char *section_name)
{
  MyConfigSectionItor *itor = self->create_section_itor(self);

  MyConfigSection *section = NULL;
  for (section = itor->begin(itor); section != itor->end(itor); section = itor->next(itor)) {
    if (0 == strcmp(section->name, section_name)) {
      itor->remove_current(itor);
      return 0;
    }
  }

  return -1;
}

static MyConfigSection* config_rename_section(MyConfig *self, const char *old_name, const char *new_name)
{
  MyConfigSection *section = self->get_section(self, old_name);
  if (section == NULL) {
    return NULL;
  }

  MyConfigSection *section2 = self->get_section(self, new_name);
  if (section2 != NULL) {
    return NULL;
  }

  int len_old = strlen(section->name);
  int len_new = strlen(new_name);

  if (len_new <= len_old) {
    strcpy(section->name, new_name);
  } else {
    char *new_buf = (char *)malloc(len_new + 1);
    if (new_buf == NULL) {
      fprintf(stderr, "failed to malloc memory for new name buffer\n");
      return NULL;
    }
    free(section->name);
    section->name = new_buf;
    strcpy(section->name, new_name);
  }

  return section;
}

static int config_set_newline(MyConfig *self, const char *newline)
{
  int len = strlen(newline);
  if (len > (int)sizeof(self->newline) - 1) {
    return -1;
  }

  strcpy(self->newline, newline);

  return 0;
}

static void config_release(MyConfig *self)
{
  self->section_list->release(self->section_list);
  self->delimiter = '\0';
  self->newline[0] = '\0';
}

static void config_destroy(MyConfig *self)
{
  self->clear(self);
  self->section_list->destroy(self->section_list);
  free(self);
}

/*
 * in place delete all leading and trailing spaces
 */
static char* trim(char *str)
{
  int len = strlen(str);
  char *begin = str;
  while (*begin != '\0' && (*begin == ' ' || *begin == '\t')) {
    begin++;
  }

  // 'empty' string
  if (*begin == '\0') {
    str[0] = '\0';
    return str;
  }

  char *end = str + len - 1;
  while (end != begin && (*end == ' ' || *end == '\t')) {
    end--;
  }

  // move one position forward to make it act as a guard
  end++;

  char *sp = str;
  while (begin != end)
    *sp++ = *begin++;

  *sp = '\0';

  return str;
}

static int config_load_from(MyConfig *self, const char *filepath)
{
  FILE *fp = fopen(filepath, "rb");
  if (fp == NULL) {
    return -1;
  }

  int guess_delimiter = (self->delimiter == '\0')? 1 : 0;
  int guess_newline = (self->newline[0] == '\0')? 1 : 0;
  char line[512];
  MyConfigSection *section = NULL;
  // indicates if there is any error when parsing
  int parse_err = 0;

  while (!feof(fp)) {
    char *linestart = NULL;
    // fgets will retain the newline but scanf %s won't
    linestart = fgets(line, sizeof(line), fp);
    if (linestart != NULL) {
      if (guess_newline) {
        int len = strlen(linestart);
        if (linestart[len - 2] == '\r') {
          strcpy(self->newline, "\r\n");
        } else {
          self->newline[0] = '\n';
          self->newline[1] = '\0';
        }

        // we have gotten the newline
        guess_newline = 0;
      }

      // parse section(header or key/value pair)
      // skip leading linestartaces
      while (*linestart != '\0' && (*linestart == ' ' || *linestart == '\t')) {
        linestart++;
      }

      if (*linestart == '\0') {
        // empty line, just continue to parse next line
        continue;
      }

      char s1[256] = {'\0'};
      char s2[256] = {'\0'};
      if (section == NULL && *linestart != '[') {
        // the config file is not begins with a section, we don't provide a
        // default section, so consider this a syntax error
        fprintf(stderr, "conifg file does not begin with a section\n");
        parse_err = 1;
        break;
      } else if (*linestart == '[') {
        // a section header line
        int count = sscanf(linestart, "[%255[^]]%255[^\r\n]", s1, s2);
        if (count != 2) {
          fprintf(stderr, "section header without trailing ']'\n");
          parse_err = 1;
          break;
        }

        trim(s1);
        if (s1[0] == '\0') {
          fprintf(stderr, "section name is empty\n");
          parse_err = 1;
          break;
        }
        section = self->create_section(self, s1);
      } else {
        // an item line

        if (guess_delimiter) {
          char *sp = linestart;

          // find out the first occurrence of a valid delimiter
          while (*sp != '\0' && *sp != '=' && *sp != ':') {
            sp++;
          }

          if (*sp == '\0') {
            fprintf(stderr, "line \"%s\" has no delimiter\n", line);
            parse_err = 1;
            break;
          }

          self->delimiter = *sp;
          guess_delimiter = 0;
        }

        // the format string
        char format[32] = {'\0'};
        char delimiter = '\0';
        sprintf(format, "%%255[^%c]%%c%%255[^\r\n]", self->delimiter);
        int count = sscanf(linestart, format, s1, &delimiter, s2);
        if (count == 2) {
          // value field is empty
          s2[0] = '\0';
        } else if (count == 1) {
            fprintf(stderr, "line \"%s\" has no delimiter\n", line);
            parse_err = 1;
            break;
        }

        trim(s1);
        trim(s2);

        section->create_item(section, s1, s2);
      }
    }
  }

  fclose(fp);

  if (parse_err) {
    // when parse error, release all sections and items
    self->clear(self);
    return -1;
  }

  return 0;
}

static int config_save_as(MyConfig *self, const char *filepath)
{
  // open as binary file, to prevent the OS to replace "\n" to "\r\n",
  // if we want "\r\n" as the newline, we should set it explicitly.
  FILE *fp = fopen(filepath, "wb");
  if (fp == NULL) {
    return -1;
  }

  char delimiter = (self->delimiter == '\0')? '=' : self->delimiter;
  char newline[3] = {'\0'};
  if (self->newline[0] == '\0') {
    newline[0] = '\n';
  } else {
    strcpy(newline, self->newline);
  }
  MyConfigSectionItor *sec_itor = self->create_section_itor(self);
  MyConfigSection *section = NULL;
  for (section = sec_itor->begin(sec_itor); section != sec_itor->end(sec_itor); section = sec_itor->next(sec_itor)) {
    // write section header
    fwrite("[", 1, 1, fp);
    fwrite(section->name, strlen(section->name), 1, fp);
    fwrite("]", 1, 1, fp);
    fwrite(newline, strlen(newline), 1, fp);

    MyConfigItemItor *item_itor = section->create_item_itor(section);
    MyConfigItem *item = NULL;
    for (item = item_itor->begin(item_itor); item != item_itor->end(item_itor); item = item_itor->next(item_itor)) {
      // write item
      fwrite(item->key, strlen(item->key), 1, fp);
      fwrite(&delimiter, 1, 1, fp);
      fwrite(item->value, strlen(item->value), 1, fp);
      fwrite(newline, strlen(newline), 1, fp);
    }
    item_itor->destroy(item_itor);
  }

  sec_itor->destroy(sec_itor);

  fclose(fp);
  return 0;
}

static void init_config(MyConfig *config)
{
  config->newline[0] = '\0';
  config->delimiter = '\0';
  config->section_list = create_linklist();

  config->load_from = &config_load_from;
  config->save_as = &config_save_as;
  config->get_section = &config_get_section;
  config->create_section = &config_create_section;
  config->delete_section = &config_delete_section;
  config->rename_section = &config_rename_section;
  config->create_section_itor = &config_create_section_itor;
  config->set_newline = &config_set_newline;
  config->clear = &config_release;
  config->destroy = &config_destroy;
}
/* MyConfig End */



MyConfig* myconfig_create()
{
  MyConfig *config = (MyConfig*)malloc(sizeof(MyConfig));

  init_config(config);

  return config;
}
