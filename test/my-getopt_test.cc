#include <gtest/gtest.h>

#include "../src/my_getopt.h"

class MyGetOptTest : public ::testing::Test
{
 protected:
  virtual void SetUp()
  {
    argc = 10;
    for (int i = 0; i < argc; i++) {
      argv[i] = new char[128];
      argv[i][0] = '\0';
    }

    strcpy(argv[0], "fake_prog");
  }

  virtual void TearDown()
  {

    // Important: reset the global variables, or the following tests might fail.
    optind = 1;
    optarg = NULL;
    optopt = 0;

    argc = 10;
    for (int i = 0; i < argc; i++) {
      delete [] argv[i];
    }
  }

  int argc;
  char *argv[10];
  int ch;
};

TEST_F(MyGetOptTest, InvalidArgList)
{
  EXPECT_DEATH({ ch = my_getopt(argc, NULL, "ab:"); }, "");
}

TEST_F(MyGetOptTest, NullOptString)
{
  ch = my_getopt(argc, argv, NULL);
  EXPECT_EQ(-1, ch);
  EXPECT_EQ(1, optind);
  EXPECT_EQ(NULL, optarg);
}

TEST_F(MyGetOptTest, EmptyOptString)
{
  ch = my_getopt(argc, argv, "");
  EXPECT_EQ(-1, ch);
  EXPECT_EQ(1, optind);
  EXPECT_EQ(NULL, optarg);
}

TEST_F(MyGetOptTest, OptionsNotInTheFront)
{
  // change argc to fit our need
  argc = 5;
  strcpy(argv[1], "param1");
  strcpy(argv[2], "-b");
  strcpy(argv[3], "343");
  strcpy(argv[4], "-a");

  while ((ch = my_getopt(argc, argv, "ab:")) != -1) {
    // do nothing
  }

  EXPECT_EQ(1, optind);
}

TEST_F(MyGetOptTest, OptionsInTheEnd)
{
  argc = 7;
  strcpy(argv[1], "-b");
  strcpy(argv[2], "arg1");
  strcpy(argv[3], "-a");
  strcpy(argv[4], "param");
  strcpy(argv[5], "-c");
  strcpy(argv[6], "arg2");

  while ((ch = my_getopt(argc, argv, "ab:c:")) != -1) {
    // do nothing
  }
  EXPECT_EQ(4, optind);
  EXPECT_EQ('a', optopt);
  EXPECT_EQ(NULL, optarg);
}

TEST_F(MyGetOptTest, UnknownOption)
{
  argc = 5;
  strcpy(argv[1], "-a");
  strcpy(argv[2], "-e");
  strcpy(argv[3], "-b");
  strcpy(argv[4], "arg1");

  while ((ch = my_getopt(argc, argv, "ab:")) != -1) {
    if (ch == '?')
      break;
  }

  EXPECT_EQ('?', ch);
  EXPECT_EQ(3, optind);
}

TEST_F(MyGetOptTest, UnknownOptionInTheEnd)
{
  argc = 5;
  strcpy(argv[1], "-a");
  strcpy(argv[2], "-b");
  strcpy(argv[3], "arg1");
  strcpy(argv[4], "-e");

  while ((ch = my_getopt(argc, argv, "ab:")) != -1) {
    // do nothing
  }

  EXPECT_EQ(5, optind);
  EXPECT_EQ('e', optopt);
  EXPECT_STREQ("arg1", optarg);
}

TEST_F(MyGetOptTest, MissingArgument)
{
  argc = 5;
  strcpy(argv[1], "-b");
  strcpy(argv[2], "-a");
  strcpy(argv[3], "-c");
  strcpy(argv[4], "arg1");

  while ((ch = my_getopt(argc, argv, "ab:c:")) != -1) {
    // will not recognize the "-a" as an option
    EXPECT_NE('a', ch);
    if (ch == 'b') {
      // "-a" is recognized as the argument of "-b"
      EXPECT_STREQ("-a", optarg);
    }
  }
}

TEST_F(MyGetOptTest, MissingArgument2)
{
  argc = 3;
  strcpy(argv[1], "-a");
  strcpy(argv[2], "-b");

  while ((ch = my_getopt(argc, argv, "ab:")) != -1) {
    EXPECT_NE('b', ch);
    if (ch != 'a') {
      EXPECT_EQ('?', ch);
    }
  }

  EXPECT_EQ('b', optopt);
  EXPECT_EQ(4, optind);
  EXPECT_EQ(NULL, optarg);
}

TEST_F(MyGetOptTest, MissingArgument3)
{
  argc = 3;
  strcpy(argv[1], "-a");
  strcpy(argv[2], "-b");

  while ((ch = my_getopt(argc, argv, ":ab:")) != -1) {
    EXPECT_NE('b', ch);
    if (ch != 'a') {
      // ch is ':' rather than '?' comparing to last test case
      EXPECT_EQ(':', ch);
    }
  }

  EXPECT_EQ('b', optopt);
  EXPECT_EQ(4, optind);
  EXPECT_EQ(NULL, optarg);
}

TEST_F(MyGetOptTest, MergedOptions)
{
  argc = 3;
  strcpy(argv[1], "-acb");
  strcpy(argv[2], "arg");

  while ((ch = my_getopt(argc, argv, "abc:")) != -1) {
    EXPECT_NE('b', ch);
    if (ch == 'c') {
      EXPECT_STREQ("b", optarg);
    }
  }

  EXPECT_EQ(2, optind);
  EXPECT_EQ('c', optopt);
  EXPECT_STREQ("b", optarg);
}

TEST_F(MyGetOptTest, MergedOptions2)
{
  argc = 4;
  strcpy(argv[1], "-ab");
  strcpy(argv[2], "arg");
  strcpy(argv[3], "arg1");

  while ((ch = my_getopt(argc, argv, "ab:")) != -1) {
    EXPECT_TRUE(ch == 'a' || ch == 'b');
  }

  EXPECT_EQ(3, optind);
  EXPECT_EQ('b', optopt);
  EXPECT_STREQ("arg", optarg);
}

TEST_F(MyGetOptTest, SingleDashTermination)
{
  argc = 5;
  strcpy(argv[1], "-ba");
  strcpy(argv[2], "-");
  strcpy(argv[3], "-c123");

  while ((ch = my_getopt(argc, argv, "ab:c:")) != -1) {
    EXPECT_NE('c', ch);
  }

  EXPECT_EQ(2, optind); // points to the "-"
  EXPECT_EQ(0, optopt);
  EXPECT_STREQ("a", optarg); // no reset to optarg
}

TEST_F(MyGetOptTest, DoubleDashTermination)
{
  argc = 6;
  strcpy(argv[1], "-ba");
  strcpy(argv[2], "-c123");
  strcpy(argv[3], "--");
  strcpy(argv[4], "-d");
  strcpy(argv[5], "arg");

  while ((ch = my_getopt(argc, argv, "ab:c:d:")) != -1) {
    EXPECT_NE('d', ch);
  }

  EXPECT_EQ(4, optind);
  EXPECT_EQ('-', optopt);
  EXPECT_STREQ("123", optarg);
}

TEST_F(MyGetOptTest, ArgBeginsWithDoubleDash)
{
  argc = 3;
  strcpy(argv[1], "-ba");
  strcpy(argv[2], "--acb");

  while ((ch = my_getopt(argc, argv, "ab:c:")) != -1) {
    if (ch == '?') {
      EXPECT_EQ('-', optopt);
    }
  }

  EXPECT_EQ(3, optind);
  EXPECT_EQ('c', optopt);
  EXPECT_STREQ("b", optarg);
}

TEST_F(MyGetOptTest, IndexOutOfBound)
{
  argc = 3;
  strcpy(argv[1], "-ab");
  strcpy(argv[2], "arg1");

  optind = 3;
  while ((ch = my_getopt(argc, argv, "ab:")) != -1) {
    // should not be executed
    FAIL() << "Shouldn't get here";
  }

  EXPECT_EQ(3, optind);
}

TEST_F(MyGetOptTest, OptionIsDigit)
{
  argc = 2;
  strcpy(argv[1], "-1234");

  int opt = '1';
  while ((ch = my_getopt(argc, argv, "12345")) != -1) {
    EXPECT_EQ(opt, ch);
    opt++;
  }

  EXPECT_EQ('5', opt);
  EXPECT_EQ('4', optopt);
}

// TODO: how to test if there is any error messages printed to stderr?(opterr equals to 1 and the first character of optstring is not ':')
