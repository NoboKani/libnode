#include <node_embedding_api.h>
#include <stdio.h>
#include <string.h>

#if _MSC_VER >= 1932 // Visual Studio 2022 version 17.2+
#    pragma comment(linker, "/alternatename:__imp___std_init_once_complete=__imp_InitOnceComplete")
#    pragma comment(linker, "/alternatename:__imp___std_init_once_begin_initialize=__imp_InitOnceBeginInitialize")
#endif

static const char* script;

static napi_value napi_entry(napi_env env, napi_value exports) {

  napi_value script_string;
  if (napi_ok != napi_create_string_utf8(env,
            script,
            strlen(script),
            &script_string)) {
    printf("napi_create_string_utf8 failed\n");
    return nullptr;
  }

  napi_value result;
  if (napi_ok != napi_run_script(env, script_string, &result)) {
    printf("napi_run_script failed\n");
    return nullptr;
  }
  return nullptr;
}

int main(int argc, const char** argv) {
	if (argc <= 1) {
		printf("No script arg\n");
		return 1;
	}
	script = argv[1];
  node_run_result_t res = node_run({ argc, (const char* const*)argv, napi_entry });
  if (res.error) {
    printf("%s\n", res.error);
  }
  printf("exit code: %d\n", res.exit_code);
  return 0;
}
