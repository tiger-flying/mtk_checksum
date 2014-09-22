#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "my_getopt.h"
#include "my_config.h"

unsigned calculate_sum(const char *filepath);

static void usage()
{
  printf("mtkchecksum [-c pattern_config_file] [-t images_folder]\n");
  printf("All available options are: \n");
  printf("\t-c\tIndicate the path of the pattern config file. If you don't specify this option, the program will search current directory to find a \"pattern.ini\" file. The program will abort if it can't find a config file.\n");
  printf("\t-t\tSpecify the folder that contains the factory files. If you don't specify the path, then the program will check current working directory(Note: not necessary where the mtkchecksum resides).\n");
  printf("\t-h\tShow this help message.\n");
  printf("\n");
}

int main(int argc, char **argv)
{
  // default to current working directory
  const char *factory_folder = "";
  const char *pattern_path = "pattern.ini";
  const char * const checksum_file_name = "Checksum.ini";
  int ch;
  while ((ch = my_getopt(argc, argv, ":c:t:h")) != -1) {
    switch (ch) {
      case 'h':
        usage();
        exit(0);
      case 'c':
        pattern_path = optarg;
        break;
      case 't':
        factory_folder = optarg;
        break;
      case ':': {
        if (optopt == 'c') {
          fprintf(stderr, "pattern file path needed for -c option\n");
        } else if (optopt == 't') {
          fprintf(stderr, "factory folder path needed for -t option\n");
        }
        exit(1);
      }
      case '?':
        fprintf(stderr, "unknown option -- %c\n", optopt);
        break;
      default:
        usage();
        exit(1);
    }
  }

  MyConfig *config = myconfig_create();
  int ret = config->load_from(config, pattern_path);
  if (ret != 0) {
    perror("failed to load config file");
    exit(1);
  }

  MyConfigSection *section = config->get_section(config, "CheckSum");
  if (section == NULL) {
    fprintf(stderr, "the config file has no CheckSum section!\n");
    config->destroy(config);
    exit(1);
  }

  char path[512] = {'\0'};
  char sum_str[11] = {'\0'};

  MyConfigItem *item = NULL;
  MyConfigItemItor *itor = section->create_item_itor(section);
  for (item = itor->begin(itor); item != itor->end(itor); item = itor->next(itor)){
    printf("calculate %s...\n", item->value);
    sprintf(path, "%s%s", factory_folder, item->value);
    // Note: the sum will be 0xffffffff if the path does not exist or there are other
    // error to open the file.
    unsigned sum = calculate_sum(path);
    // I don't know why, but the Checksum.ini only use the lower tow bytes value,
    // so I convert the sum to unsigned short to cut off the higher two bytes
    sprintf(sum_str, "0x%0.8x", (unsigned short)sum);

    item->set_value(item, sum_str);
  }

  itor->destroy(itor);

  sprintf(path, "%s%s", factory_folder, checksum_file_name);
  config->save_as(config, path);

  return 0;
}
