void ignore_result_helper(int __attribute__((unused)) dummy, ...);

#define IGNORE_RESULT(X) ignore_result_helper(0, (X))