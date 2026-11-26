//
// convert
// stolen and lost mobile telephone open data
// JSON to SQL
//

#include "ini.h"
#include "pdjson.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

// params to read from config file
typedef struct {
  char *input;
  char *output;
  char *tpl;
} Config;

// data for single Mobile Telephone Report (mtr)
// field names are the same as in open data source file
// but it doesn't matter - these field names can be different
typedef struct {
  char id[16];
  char dk[11];
  char insert_date[11];
  char imei[16];
  char nz[32];
  char nk[16];
  char ovd[128];
  char dtl[255];
} MTreport;

static MTreport mtr;

// no strdup in C99, need this helper for work around
char *c99strdup(const char *s) {
  size_t len = strlen(s) + 1;
  char *dup = malloc(len);
  if (dup != NULL) {
    memcpy(dup, s, len);
  }
  return dup;
}

// config callback handler (for ini.h)
static int callback_handler(void *user, const char *section, const char *name,
                            const char *value) {
  Config *cfg = (Config *)user;

  if (MATCH("input", "file")) {
    cfg->input = c99strdup(value);
    if (cfg->input == NULL) {
      perror("Config callback handler failed to allocate memory");
      return 0;
    }
  } else if (MATCH("output", "file")) {
    cfg->output = c99strdup(value);
    if (cfg->output == NULL) {
      perror("Config callback handler failed to allocate memory");
      return 0;
    }
  } else if (MATCH("output", "template")) {
    cfg->tpl = c99strdup(value);
    if (cfg->tpl == NULL) {
      perror("Config callback handler failed to allocate memory");
      return 0;
    }
  } else {
    // unexpected param
    return 0;
  }

  return 1;
}

int main(void) {

  Config cfg = {0};

  int err = ini_parse("config.ini", callback_handler, &cfg);
  if (err < 0) {
    fprintf(stderr, "Abort: failed to read 'config.ini'\n");
    return 1;
  } else if (err > 0) {
    fprintf(stderr, "Abort: config error on line %d\n", err);
    return 1;
  }

  printf("Input: %s\n", cfg.input);
  printf("Output: %s\n", cfg.output);
  printf("Insert Template: %s\n", cfg.tpl);

  FILE *ifp = fopen(cfg.input, "rb");
  if (!ifp) {
    perror("Abort: failed to open input file");
    free(cfg.input);
    free(cfg.output);
    free(cfg.tpl);
    return 1;
  }

  // remove old output file first
  if (remove(cfg.output) == 0) {
    printf("Old file '%s' deleted.\n", cfg.output);
  }

  FILE *ofp = fopen(cfg.output, "a");
  if (!ofp) {
    perror("Abort: failed to open output file");
    fclose(ofp);
    free(cfg.input);
    free(cfg.output);
    free(cfg.tpl);
    return 1;
  }

  json_stream s;
  json_open_stream(&s, ifp);

  // json object count for debug
  int i = 0;

  int awaiting_key = 0;
  int awaiting_val = 0;
  char *key;
  enum json_type type;
  while ((type = json_next(&s)) != JSON_DONE && type != JSON_ERROR) {
    // short run for debug
    // if (i >= 10) {
    //   break;
    // }

    switch (type) {
    case JSON_OBJECT:
      // reset structure for coming object data
      memset(&mtr, 0, sizeof(mtr));
      // as object started we know next thing will be a key
      awaiting_key = 1;
      break;
    case JSON_OBJECT_END:
      // we have parsed data for whole object
      // append it to output file
      fprintf(ofp, cfg.tpl, mtr.id, mtr.ovd, mtr.insert_date, mtr.nz, mtr.imei,
              mtr.nk, mtr.dk, mtr.dtl);
      fprintf(ofp, "\n");
      free(key);
      // debug
      i++;
      break;
    case JSON_ARRAY:
      break;
    case JSON_ARRAY_END:
      break;
    case JSON_STRING:
      if (awaiting_key == 1) {
        key = malloc(strlen(json_get_string(&s, NULL)) + 1);
        if (key == NULL) {
          perror("Abort: malloc error for key");
          return 1;
        }
        strcpy(key, json_get_string(&s, NULL));
        awaiting_key = 0;
        awaiting_val = 1;
      } else if (awaiting_val == 1) {
        if (strcmp(key, "id") == 0) {
          strncpy(mtr.id, json_get_string(&s, NULL), sizeof(mtr.id) - 1);
          mtr.id[sizeof(mtr.id) - 1] = '\0';
        } else if (strcmp(key, "ovd") == 0) {
          strncpy(mtr.ovd, json_get_string(&s, NULL), sizeof(mtr.ovd) - 1);
          mtr.ovd[sizeof(mtr.ovd) - 1] = '\0';
        } else if (strcmp(key, "insert_date") == 0) {
          strncpy(mtr.insert_date, json_get_string(&s, NULL),
                  sizeof(mtr.insert_date) - 1);
          mtr.insert_date[sizeof(mtr.insert_date) - 1] = '\0';
        } else if (strcmp(key, "nz") == 0) {
          strncpy(mtr.nz, json_get_string(&s, NULL), sizeof(mtr.nz) - 1);
          mtr.nz[sizeof(mtr.nz) - 1] = '\0';
        } else if (strcmp(key, "imei") == 0) {
          strncpy(mtr.imei, json_get_string(&s, NULL), sizeof(mtr.imei) - 1);
          mtr.imei[sizeof(mtr.imei) - 1] = '\0';
        } else if (strcmp(key, "nk") == 0) {
          strncpy(mtr.nk, json_get_string(&s, NULL), sizeof(mtr.nk) - 1);
          mtr.nk[sizeof(mtr.nk) - 1] = '\0';
        } else if (strcmp(key, "dk") == 0) {
          strncpy(mtr.dk, json_get_string(&s, NULL), sizeof(mtr.dk) - 1);
          mtr.dk[sizeof(mtr.dk) - 1] = '\0';
        } else if (strcmp(key, "dtl") == 0) {
          strncpy(mtr.dtl, json_get_string(&s, NULL), sizeof(mtr.dtl) - 1);
          mtr.dtl[sizeof(mtr.dtl) - 1] = '\0';
        }
        awaiting_val = 0;
        awaiting_key = 1;
      }
      break;
    case JSON_NUMBER:
      // printf("Number: %f\n", json_get_number(&s));
      break;
    case JSON_TRUE:
      // printf("Boolean: true\n");
      break;
    case JSON_FALSE:
      // printf("Boolean: false\n");
      break;
    case JSON_NULL:
      if (awaiting_val == 1) {
        if (strcmp(key, "dtl") == 0) {
          strncpy(mtr.dtl, "", sizeof(mtr.dtl) - 1);
          mtr.dtl[sizeof(mtr.dtl) - 1] = '\0';
        }
        awaiting_val = 0;
        awaiting_key = 1;
      }
      break;
    default:
      break;
    }
  }

  if (type == JSON_ERROR) {
    fprintf(stderr, "Parse error on line %zu, pos %zu: %s\n",
            json_get_lineno(&s), json_get_position(&s), json_get_error(&s));
  }

  json_close(&s);
  fclose(ifp);
  fclose(ofp);
  free(cfg.input);
  free(cfg.output);
  free(cfg.tpl);
  printf("Finished.\n");
  return 0;
}
