#include <node_embedding_api.h>
#include <cstdio>
#include <sstream>
#include <thread>
#include <vector>
#include <string>
#include <uv.h>

void Callback(uv_async_t* watcher);

// Thread counter.
static long int threadsCtr = 0;

static void CallFunctionTestThread()
{
	std::ostringstream oss;
	oss << std::this_thread::get_id() << std::endl;
	printf("thread id %s", oss.str().c_str());
}

long thread_id()
{
	std::ostringstream oss;
	oss << std::this_thread::get_id();
	return std::stol(oss.str());
}

struct Thread
{
	uv_async_t async_watcher;
	long int id;
	long pid;
	std::thread thread;
};

void ThreadInit(Thread* thread_instance)
{
	thread_instance->pid = thread_id();
	CallFunctionTestThread();
	printf("thread pid: %d\n", thread_instance->pid);
}

static napi_value napi_entry(napi_env env, napi_value exports) {
	napi_value script_string;
	if (napi_ok != napi_create_string_utf8(env,
			"global.hello = 'test1'; console.log(process.argv[1], process.argv[2]); setInterval(() => {console.log(global.hello)}, 1000)",
			NAPI_AUTO_LENGTH,
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

napi_value TestCall(napi_env env, napi_callback_info info)
{
	size_t argc = 1;
	napi_value args[1];
	napi_value this_arg;
	napi_get_cb_info(env, info, &argc, args, &this_arg, nullptr);

	napi_value module_name;
	napi_get_named_property(env, this_arg, "name", &module_name);

	uint32_t version;
	napi_get_version(env, &version);

	napi_isolate context;
	napi_get_context(env, &context);

	napi_isolate isolate;
	napi_get_isolate(env, &isolate);

	Thread* thread = new Thread;
	thread->id = ++threadsCtr;

	std::thread thread_init(ThreadInit, thread);
	thread_init.detach();
	uv_async_init(uv_default_loop(), &thread->async_watcher, Callback);
	
	CallFunctionTestThread();

	return module_name;
}

static napi_value napi_entry2(napi_env env, napi_value exports) {
	napi_value script_string;
	if (napi_ok != napi_create_string_utf8(env,
			"global.hello = 'test1';console.log((new global.worker('hello world')).start(() => {})); setInterval(() => {console.log('2test', 'global', global.hello)}, 1000); ",
			NAPI_AUTO_LENGTH,
			&script_string)) {
		printf("napi_create_string_utf8 failed\n");
		return nullptr;
			}

	napi_value global;
	napi_get_global(env, &global);

	napi_property_descriptor properties[1] = {
		{"start", 0, TestCall, 0, 0, 0, napi_enumerable, 0},
	};

	napi_value object_class;
	napi_define_class(env, "worker", NAPI_AUTO_LENGTH, [](napi_env env, napi_callback_info info) -> napi_value
	{
		size_t argc = 2;
		napi_value args[2];
		napi_value this_arg;
		napi_get_cb_info(env, info, &argc, args, &this_arg, nullptr);

		napi_set_named_property(env, this_arg, "name", args[0]);

		return this_arg;
	}, nullptr, 1, properties, &object_class);

	napi_set_named_property(env, global, "worker", object_class);
	

	napi_value result;
	if (napi_ok != napi_run_script(env, script_string, &result)) {
		printf("napi_run_script failed\n");
		return nullptr;
	}
	return nullptr;
}


int main(int argc, char** argv) {

	static const char* argv0 = argv[0];

	static node_options_t options = node_options_t { {argv0, "Hello", "world"}, napi_entry2 };
	node_run_result_t res = node_run(options);
	if (res.error) {
		printf("%s\n", res.error);
	}
	printf("exit code: %d\n", res.exit_code);
	
	return 0;
}

void Callback(uv_async_t* watcher)
{
	printf("callback watcher\n");
	
    // eventsQueueItem* event;
    // typeThread* thread = (typeThread*)watcher;
    //
    // v8::HandleScope scope(v8::Isolate::GetCurrent());
    //
    // v8::Isolate* iso = v8::Isolate::GetCurrent();
    //
    // v8::Local<v8::Context> context = v8::Context::New(iso);
    //
    // v8::Local<v8::Value> cb;
    // v8::Local<v8::Value> that;
    // v8::Local<v8::Array> array;
    // v8::Local<v8::Value> args[2];
    // v8::Local<v8::Value> null = v8::Local<v8::Value>::New(iso, v8::Null(iso));
    //
    // assert(thread != NULL);
    // assert(!thread->destroyed);
    //
    // TAGG_DEBUG && printf("UV CALLBACK FOR THREAD %ld BEGIN\n", thread->id);
    //
    // v8::TryCatch onError;
    // while ((event = qPull(thread->processEventsQueue)))
    // {
    //     TAGG_DEBUG && printf("UV CALLBACK FOR THREAD %ld GOT EVENT #%ld\n", thread->id, event->serial);
    //
    //     if (event->eventType == kEventTypeEval)
    //     {
    //         TAGG_DEBUG && printf("CALLBACK kEventTypeEval IN MAIN THREAD\n");
    //
    //         assert(event->eval.hasCallback);
    //         assert(event->eval.resultado != NULL);
    //
    //         if (event->eval.error)
    //         {
    //             args[0] = v8::Exception::Error(v8::String::NewFromUtf8(iso, event->eval.resultado).ToLocalChecked());
    //             args[1] = null;
    //         }
    //         else
    //         {
    //             args[0] = null;
    //             args[1] = v8::String::NewFromUtf8(iso, event->eval.resultado);
    //         }
    //         free(event->eval.resultado);
    //
    //         cb = v8::Local<v8::Value>::New(iso, event->callback);
    //         that = v8::Local<v8::Value>::New(iso, thread->nodeObject);
    //         assert(that->IsObject());
    //         assert(cb->IsFunction());
    //         cb->ToObject(context).ToLocalChecked()->CallAsFunction(context, that->ToObject(context).ToLocalChecked(), 2, args);
    //         event->callback.Reset();
    //         event->eventType = kEventTypeEmpty;
    //         qPush(event, qitemsStore);
    //
    //         if (onError.HasCaught())
    //         {
    //             node::FatalException(iso, onError);
    //             return;
    //         }
    //     }
    //     else if (event->eventType == kEventTypeEmit)
    //     {
    //         TAGG_DEBUG && printf("CALLBACK kEventTypeEmit IN MAIN THREAD\n");
    //
    //         args[0] = v8::String::NewFromUtf8(iso, event->emit.eventName).ToLocalChecked();
    //         free(event->emit.eventName);
    //         array = v8::Array::New(iso, event->emit.argc);
    //         args[1] = array;
    //         if (event->emit.argc)
    //         {
    //             int i = 0;
    //             while (i < event->emit.argc)
    //             {
    //                 array->Set(context, i, v8::String::NewFromUtf8(iso, event->emit.argv[i]).ToLocalChecked());
    //                 free(event->emit.argv[i]);
    //                 i++;
    //             }
    //             free(event->emit.argv);
    //         }
    //         cb = v8::Local<v8::Value>::New(iso, thread->nodeDispatchEvents);
    //         cb->ToObject(context).ToLocalChecked()->CallAsFunction(context, iso->GetCurrentContext()->Global(), 2, args);
    //
    //         event->eventType = kEventTypeEmpty;
    //         qPush(event, qitemsStore);
    //     }
    //     else
    //     {
    //         assert(0);
    //     }
    // }
    //
    // if (thread->sigkill && thread->ended)
    // {
    //     TAGG_DEBUG && printf("UV CALLBACK FOR THREAD %ld CALLED cleanUpAfterThread()\n", thread->id);
    //     //pthread_cancel(thread->thread);
    //     thread->destroyed = 1;
    //     cleanUpAfterThread(thread);
    // }
    //
    // TAGG_DEBUG && printf("UV CALLBACK FOR THREAD %ld END\n", thread->id);
}