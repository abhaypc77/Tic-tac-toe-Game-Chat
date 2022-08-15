#define XODL_SER(...) xo_debug_log('S', __FILE__, __LINE__, (char *)__FUNCTION__, __VA_ARGS__)
#define XODL_CLI(...) xo_debug_log('C', __FILE__, __LINE__, (char *)__FUNCTION__, __VA_ARGS__)

extern void xo_debug_log(char entity, char *file, int line, char *fun_name, char *format, ...);

