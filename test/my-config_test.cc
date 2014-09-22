#include <gtest/gtest.h>
#include "../src/my_config.h"

class MyConfigTest : public ::testing::Test
{
 protected:
  virtual void SetUp()
  {
    config_ = myconfig_create();
  }

  virtual void TearDown()
  {
    config_->destroy(config_);
  }

  MyConfig *config_;
};


TEST_F(MyConfigTest, NewEmptyConfig)
{
  EXPECT_EQ('\0', config_->delimiter);
  EXPECT_STREQ("", config_->newline);

  MyConfigSectionItor *itor = config_->create_section_itor(config_);
  EXPECT_TRUE(itor->begin(itor) == itor->end(itor));
  itor->destroy(itor);
}

TEST_F(MyConfigTest, LoadNonExistFile)
{
  int ret = config_->load_from(config_, "non-exist-file");
  EXPECT_NE(0, ret);

  MyConfigSectionItor *itor = config_->create_section_itor(config_);
  EXPECT_TRUE(itor->begin(itor) == itor->end(itor));
  itor->destroy(itor);
}

TEST_F(MyConfigTest, LoadFromFile)
{
  int ret = config_->load_from(config_, "../src/pattern.ini");
  EXPECT_EQ(0, ret);

  EXPECT_EQ('=', config_->delimiter);
  EXPECT_STREQ("\r\n", config_->newline);

  MyConfigSection *section = config_->get_section(config_, "IsEnableChecksum");
  ASSERT_TRUE(section != NULL);

  section = config_->get_section(config_, "CheckSum");
  EXPECT_TRUE(section != NULL);

  MyConfigItemItor *itor = section->create_item_itor(section);
  int item_count = 0;
  MyConfigItem *item = NULL;
  for (item = itor->begin(itor); item != itor->end(itor); item = itor->next(itor)) {
    item_count++;
    if (item_count == 12) {
      EXPECT_STREQ("CACHE", item->key);
      EXPECT_STREQ("cache.img", item->value);
    }
  }
  itor->destroy(itor);

  EXPECT_EQ(12, item_count);
}

TEST_F(MyConfigTest, LoadAnotherFile)
{
  int ret = config_->load_from(config_, "../src/pattern.ini");
  EXPECT_EQ(0, ret);

  config_->clear(config_);
  ret = config_->load_from(config_, "test.ini");
  EXPECT_EQ(0, ret);

  EXPECT_EQ(':', config_->delimiter);
  EXPECT_STREQ("\n", config_->newline);

  MyConfigSection *section = config_->get_section(config_, "Test");
  ASSERT_TRUE(section != NULL);

  MyConfigItemItor *itor = section->create_item_itor(section);
  MyConfigItem *item = NULL;
  for (item = itor->begin(itor); item != itor->end(itor); item = itor->next(itor)) {
    EXPECT_STREQ("key space", item->key);
    EXPECT_STREQ("value space", item->value);
  }
  itor->destroy(itor);

  section = config_->get_section(config_, "CheckSum");
  EXPECT_EQ(NULL, section);

  section = config_->get_section(config_, "IsEnableChecksum");
  EXPECT_EQ(NULL, section);
}

TEST_F(MyConfigTest, LoadFromInvalidFormattedFile)
{
  int ret = config_->load_from(config_, "invalid.ini");
  EXPECT_NE(0, ret);

  MyConfigSectionItor *itor = config_->create_section_itor(config_);
  MyConfigSection *section = itor->begin(itor);
  EXPECT_EQ(itor->end(itor), section);

  itor->destroy(itor);
}

TEST_F(MyConfigTest, AddSectionToEmptyConfig)
{
  MyConfigSection *section = config_->create_section(config_, "New Section");
  EXPECT_TRUE(section != NULL);

  section = config_->get_section(config_, "New Section");
  EXPECT_TRUE(section != NULL);
  EXPECT_STREQ("New Section", section->name);
}

TEST_F(MyConfigTest, AddSectionToLoadedConfig)
{
  int ret = config_->load_from(config_, "test.ini");
  ASSERT_EQ(0, ret);

  MyConfigSection *section = config_->create_section(config_, "New");
  EXPECT_TRUE(section != NULL);
  EXPECT_STREQ("New", section->name);

  section = config_->get_section(config_, "New");
  EXPECT_TRUE(section != NULL);

  section = config_->get_section(config_, "Test");
  EXPECT_TRUE(section != NULL);

  // the creation will fail if the section already exists
  section = config_->create_section(config_, "New");
  EXPECT_EQ(NULL, section);
}

TEST_F(MyConfigTest, DeleteSection)
{
  int ret = config_->load_from(config_, "../src/pattern.ini");
  ASSERT_EQ(0, ret);

  ret = config_->delete_section(config_, "NonExistSection");
  EXPECT_NE(0, ret);

  ret = config_->delete_section(config_, "IsEnableChecksum");
  EXPECT_EQ(0, ret);

  MyConfigSection *section = config_->get_section(config_, "IsEnableChecksum");
  EXPECT_EQ(NULL, section);
}

TEST_F(MyConfigTest, RenameSection)
{
  int ret = config_->load_from(config_, "../src/pattern.ini");
  ASSERT_EQ(0, ret);

  MyConfigSection *section = config_->rename_section(config_, "IsEnableChecksum", "EnableChecksum");
  EXPECT_TRUE(section != NULL);

  section = config_->get_section(config_, "IsEnableChecksum");
  EXPECT_EQ(NULL, section);
}

TEST_F(MyConfigTest, ChangeDelimiter)
{
  int ret = config_->load_from(config_, "../src/pattern.ini");
  ASSERT_EQ(0, ret);

  EXPECT_EQ('=', config_->delimiter);

  config_->delimiter = ':';
  EXPECT_EQ(':', config_->delimiter);
}

TEST_F(MyConfigTest, ChangeNewLine)
{
  int ret = config_->load_from(config_, "../src/pattern.ini");
  ASSERT_EQ(0, ret);

  EXPECT_STREQ("\r\n", config_->newline);

  char nl[5] = {'\0'};
  strcpy(nl, "\n");
  ret = config_->set_newline(config_, nl);
  EXPECT_EQ(0, ret);
  EXPECT_STREQ(nl, config_->newline);

  strcpy(nl, "\r\n\n"); // more than two characters
  ret = config_->set_newline(config_, nl);
  EXPECT_NE(0, ret);
  EXPECT_STREQ("\n", config_->newline);
}

TEST_F(MyConfigTest, SaveToInvalidPath)
{
  int ret = config_->load_from(config_, "../src/pattern.ini");
  ASSERT_EQ(0, ret);

  EXPECT_NO_FATAL_FAILURE(ret = config_->save_as(config_, "non-exist-path/pattern.ini"));
  EXPECT_NE(0, ret);
}

TEST_F(MyConfigTest, SaveAndReload)
{
  int ret = config_->load_from(config_, "../src/pattern.ini");
  ASSERT_EQ(0, ret);

  MyConfigSection *section = config_->rename_section(config_, "IsEnableChecksum", "EnableChecksum");
  ASSERT_TRUE(section != NULL);

  const char * const new_path = "pattern.ini";
  ret = config_->save_as(config_, new_path);
  ASSERT_EQ(0, ret);

  config_->clear(config_);

  ret = config_->load_from(config_, new_path);
  ASSERT_EQ(0, ret);

  section = config_->get_section(config_, "IsEnableChecksum");
  EXPECT_EQ(NULL, section);

  section = config_->get_section(config_, "EnableChecksum");
  EXPECT_TRUE(section != NULL);

  remove(new_path);
}

class MyConfigSectionTest : public MyConfigTest
{
 protected:
  virtual void SetUp()
  {
    section_ = NULL;
    MyConfigTest::SetUp();

    int ret = config_->load_from(config_, "test.ini");
    if (ret == 0) {
      section_ = config_->get_section(config_, "Test");
    }
  }

  MyConfigSection *section_;
};

TEST_F(MyConfigSectionTest, GetItem)
{
  MyConfigItem *item = section_->get_item(section_, "NonExistItem");
  EXPECT_EQ(NULL, item);

  item = section_->get_item(section_, "key space");
  EXPECT_TRUE(item != NULL);
  EXPECT_STREQ("key space", item->key);
}

TEST_F(MyConfigSectionTest, AddItem)
{
  MyConfigItem *item = section_->create_item(section_, "new key", "new value");
  EXPECT_TRUE(item != NULL);

  item = section_->get_item(section_, "new key");
  EXPECT_TRUE(item != NULL);

  // return NULL if the key already exists, do not change the value of the existed item
  item = section_->create_item(section_, "key space", "other value");
  EXPECT_EQ(NULL, item);
}

TEST_F(MyConfigSectionTest, DeleteItem)
{
  int ret = section_->delete_item(section_, "non-exist key");
  EXPECT_NE(0, ret);

  ret = section_->delete_item(section_, "key space");
  EXPECT_EQ(0, ret);

  MyConfigItem *item = section_->get_item(section_, "key space");
  EXPECT_EQ(NULL, item);
}

TEST_F(MyConfigSectionTest, RenameItem)
{
  MyConfigItem *item = section_->rename_item(section_, "non-exist key", "new key");
  EXPECT_EQ(NULL, item);

  item = section_->rename_item(section_, "key space", "new key");
  ASSERT_TRUE(item != NULL);
  EXPECT_STREQ("new key", item->key);

  item = section_->get_item(section_, "key space");
  EXPECT_EQ(NULL, item);

  item = section_->create_item(section_, "key1", "value2");
  ASSERT_TRUE(item != NULL);
  // can't rename to an already exists key
  item = section_->rename_item(section_, "key1", "new key");
  EXPECT_EQ(NULL, item);
}

class MyConfigItemTest : public MyConfigSectionTest
{
 protected:
  virtual void SetUp()
  {
    MyConfigSectionTest::SetUp();

    MyConfigItemItor *itor = section_->create_item_itor(section_);
    item_ = itor->begin(itor);

    itor->destroy(itor);
  }

  MyConfigItem *item_;
};

TEST_F(MyConfigItemTest, SetItemValue)
{
  EXPECT_STREQ("key space", item_->key); // Note: no trailing space
  EXPECT_STREQ("value space", item_->value);

  item_->set_value(item_, "This new value is longer than original one.");

  EXPECT_STREQ("key space", item_->key);
  EXPECT_STREQ("This new value is longer than original one.", item_->value);
}
