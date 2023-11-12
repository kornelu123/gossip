#define ERROR_SIZEOF(type) \
_Pragma("GCC diagnostic push"); \
_Pragma("GCC diagnostic error \"-Wint-conversion\""); \
char (*_error_sizeof)[sizeof(type)] = 1; \
_Pragma("GCC diagnostic pop");


#define ERROR_OFFSETOF(type, member) \
_Pragma("GCC diagnostic push"); \
_Pragma("GCC diagnostic error \"-Wint-conversion\""); \
char (*_error_offsetof)[(unsigned long long)&((type*)0)->member] = 1; \
_Pragma("GCC diagnostic pop");
