#pragma once

#define VERIFY(condition, operation, filename, rv, value, exit) \
  do { \
    if (!(condition)) { \
      fprintf(stderr, "failed to %s %s: %s\n", \
              operation, filename, strerror(errno)); \
      rv = value; \
      exit; \
    } \
  } while (0)


#define CHECK(condition, operation, filename, rv, value, label) \
  VERIFY(condition, operation, filename, rv, value, goto label)


#define ROLLBACK(condition, operation, filename, rv, value) \
  VERIFY(condition, operation, filename, rv, value, (void)0)
