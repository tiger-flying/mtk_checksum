#include <gtest/gtest.h>

#include "../src/my_linklist.h"

class MyLinkListTest : public ::testing::Test {
 protected:
  virtual void SetUp()
  {
    list_ = create_linklist();
  }

  virtual void TearDown()
  {
    list_->destroy(list_);
    list_ = NULL;
  }

  MyLinkList *list_;
};

TEST_F(MyLinkListTest, Creation)
{
  ASSERT_TRUE(list_ != NULL);
  EXPECT_TRUE(list_->head != NULL);
  EXPECT_TRUE(list_->end != NULL);
  EXPECT_TRUE(list_->append != NULL);
}

TEST_F(MyLinkListTest, AddNode)
{
  const int size = 3;
  int arr[size];

  for (int i = 0; i < size; i++)
    arr[i] = i;

  for (int i = 0; i < size; i++)
    list_->append(list_, arr + i);

  MyNode *node = list_->head->next;
  EXPECT_TRUE(node != list_->end);

  for (int i = 0; node != list_->end && i < size; node = node->next, i++) {
    EXPECT_EQ(i, *(int *)node->data);
  }

  EXPECT_EQ(list_->end, node);
}

TEST_F(MyLinkListTest, RemoveInvalidNode)
{
  int ret = list_->remove(list_, list_->head);
  EXPECT_NE(0, ret);

  ret = list_->remove(list_, list_->end);
  EXPECT_NE(0, ret);
}

TEST_F(MyLinkListTest, RemoveNode)
{
  const int size = 5;
  int arr[size];

  for (int i = 0; i < size; i++)
    arr[i] = i;

  for (int i = 0; i < size; i++)
    list_->append(list_, arr + i);

  MyNode *node = list_->find(list_, arr + 2);

  int ret = list_->remove(list_, node);
  EXPECT_EQ(0, ret);

  node = list_->head->next;
  while (node != list_->end) {
    EXPECT_NE(2, *(int *)node->data);
    node = node->next;
  }

}

TEST_F(MyLinkListTest, FindNode)
{
  const int size = 5;
  int arr[size];

  for (int i = 0; i < size; i++)
    arr[i] = i;

  for (int i = 0; i < size; i++)
    list_->append(list_, arr + i);

  MyNode *node = list_->find(list_, arr + 2);

  ASSERT_TRUE(node != NULL);
  EXPECT_EQ(2, *(int *)node->data);
}
