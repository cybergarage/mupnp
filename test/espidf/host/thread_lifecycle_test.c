#include <assert.h>
#include <errno.h>
#include <stdatomic.h>
#include <stdio.h>
#include <unistd.h>

#include <esp_timer.h>
#include <mupnp/util/cond.h>
#include <mupnp/util/mutex.h>
#include <mupnp/util/thread.h>

static atomic_int allocations;
static atomic_int entered;
static atomic_int finished;
static atomic_bool releaseWait;
static atomic_bool failCreate;
static mUpnpCond* condition;
static mUpnpMutex* conditionMutex;

void* __real_malloc(size_t);
void __real_free(void*);

void* __wrap_malloc(size_t size)
{
  void* value = __real_malloc(size);
  if (value != NULL)
    atomic_fetch_add(&allocations, 1);
  return value;
}

void __wrap_free(void* value)
{
  if (value != NULL)
    atomic_fetch_sub(&allocations, 1);
  __real_free(value);
}

int __real_pthread_create(pthread_t*, const pthread_attr_t*, void* (*)(void*), void*);

int __wrap_pthread_create(pthread_t* thread, const pthread_attr_t* attr, void* (*action)(void*), void* arg)
{
  if (atomic_exchange(&failCreate, false))
    return EAGAIN;
  return __real_pthread_create(thread, attr, action, arg);
}

static void wait_for_count(atomic_int* value, int count)
{
  int64_t deadline = esp_timer_get_time() + 5000000;
  while (atomic_load(value) < count && esp_timer_get_time() < deadline)
    usleep(1000);
  assert(atomic_load(value) >= count);
}

static void sleeping_worker(mUpnpThread* thread)
{
  assert(mupnp_thread_self() == thread);
  atomic_fetch_add(&entered, 1);
  /* Longer than the old useconds_t multiplication could represent. */
  mupnp_sleep(24L * 60 * 60 * 1000);
  assert(!mupnp_thread_isrunnable(thread));
  atomic_fetch_add(&finished, 1);
}

static void returning_worker(mUpnpThread* thread)
{
  assert(mupnp_thread_self() == thread);
  atomic_fetch_add(&finished, 1);
}

static void deleting_worker(mUpnpThread* thread)
{
  assert(mupnp_thread_delete(thread));
  /* The wrapper must retain the object until this action has returned. */
  assert(mupnp_thread_self() == thread);
  assert(!mupnp_thread_isrunnable(thread));
  atomic_fetch_add(&finished, 1);
}

static void condition_worker(mUpnpThread* thread)
{
  assert(mupnp_thread_self() == thread);
  mupnp_mutex_lock(conditionMutex);
  atomic_fetch_add(&entered, 1);
  /* Force stop's signal to arrive before the condition wait starts. */
  while (!atomic_load(&releaseWait))
    usleep(1000);
  assert(mupnp_cond_wait(condition, conditionMutex, 0));
  mupnp_mutex_unlock(conditionMutex);
  atomic_fetch_add(&finished, 1);
}

static void* stop_worker(void* arg)
{
  assert(mupnp_thread_stop(arg));
  return NULL;
}

static void* stop_condition_worker(void* arg)
{
  assert(mupnp_thread_stop_with_cond(arg, condition));
  return NULL;
}

int main(void)
{
  alarm(20);
  /* First use occurs on a non-library thread, before the TLS key exists. */
  assert(mupnp_thread_self() == NULL);
  assert(!mupnp_thread_start(NULL));
  assert(!mupnp_thread_stop(NULL));
  assert(!mupnp_thread_delete(NULL));
  mUpnpThread* thread = mupnp_thread_new();
  assert(thread != NULL);
  assert(!mupnp_thread_start(thread));
  assert(!mupnp_thread_isrunning(thread));
  assert(mupnp_thread_stop(thread));
  assert(mupnp_thread_delete(thread));

  thread = mupnp_thread_new();
  mupnp_thread_setaction(thread, sleeping_worker);
  atomic_store(&failCreate, true);
  assert(!mupnp_thread_start(thread));
  assert(!mupnp_thread_isrunning(thread));
  assert(!mupnp_thread_isrunnable(thread));
  for (int iteration = 0; iteration < 100; iteration++) {
    assert(mupnp_thread_start(thread));
    wait_for_count(&entered, iteration + 1);
    assert(mupnp_thread_isrunning(thread));
    assert(!mupnp_thread_start(thread));
    int64_t started = esp_timer_get_time();
    assert(mupnp_thread_stop(thread));
    assert(esp_timer_get_time() - started < 500000);
    assert(atomic_load(&finished) == iteration + 1);
    assert(!mupnp_thread_isrunning(thread));
  }
  assert(mupnp_thread_delete(thread));

  /* A naturally completed joinable worker is joined before reuse/deletion. */
  atomic_store(&finished, 0);
  thread = mupnp_thread_new();
  mupnp_thread_setaction(thread, returning_worker);
  for (int iteration = 0; iteration < 100; iteration++) {
    assert(mupnp_thread_start(thread));
    wait_for_count(&finished, iteration + 1);
    while (mupnp_thread_isrunning(thread))
      usleep(1000);
  }
  assert(mupnp_thread_delete(thread));

  /* Concurrent external stop requests must not attempt a double join. */
  atomic_store(&entered, 0);
  thread = mupnp_thread_new();
  mupnp_thread_setaction(thread, sleeping_worker);
  assert(mupnp_thread_start(thread));
  wait_for_count(&entered, 1);
  pthread_t stoppers[8];
  for (int index = 0; index < 8; index++)
    assert(pthread_create(&stoppers[index], NULL, stop_worker, thread) == 0);
  for (int index = 0; index < 8; index++)
    assert(pthread_join(stoppers[index], NULL) == 0);
  assert(mupnp_thread_delete(thread));

  /* A lost condition signal must not hang cooperative stop/join. */
  condition = mupnp_cond_new();
  conditionMutex = mupnp_mutex_new();
  assert(condition != NULL && conditionMutex != NULL);
  atomic_store(&entered, 0);
  atomic_store(&finished, 0);
  thread = mupnp_thread_new();
  mupnp_thread_setaction(thread, condition_worker);
  assert(mupnp_thread_start(thread));
  wait_for_count(&entered, 1);
  pthread_t stopper;
  assert(pthread_create(&stopper, NULL, stop_condition_worker, thread) == 0);
  while (mupnp_thread_isrunnable(thread))
    usleep(1000);
  int64_t started = esp_timer_get_time();
  atomic_store(&releaseWait, true);
  assert(pthread_join(stopper, NULL) == 0);
  assert(esp_timer_get_time() - started < 500000);
  assert(atomic_load(&finished) == 1);
  assert(mupnp_thread_delete(thread));

  /* Also stop a worker already inside the indefinite condition wait. */
  atomic_store(&entered, 0);
  thread = mupnp_thread_new();
  mupnp_thread_setaction(thread, condition_worker);
  assert(mupnp_thread_start(thread));
  wait_for_count(&entered, 1);
  usleep(20000);
  started = esp_timer_get_time();
  assert(mupnp_thread_stop_with_cond(thread, condition));
  assert(esp_timer_get_time() - started < 500000);
  assert(mupnp_thread_delete(thread));

  /* Polling must not shorten the caller's requested timeout. */
  mupnp_mutex_lock(conditionMutex);
  started = esp_timer_get_time();
  assert(mupnp_cond_wait(condition, conditionMutex, 1));
  assert(esp_timer_get_time() - started >= 950000);
  assert(esp_timer_get_time() - started < 2000000);
  mupnp_mutex_unlock(conditionMutex);
  assert(mupnp_cond_delete(condition));
  assert(mupnp_mutex_delete(conditionMutex));

  /* Self-deleting notification workers must not leak joinable pthreads. */
  atomic_store(&finished, 0);
  for (int iteration = 0; iteration < 200; iteration++) {
    thread = mupnp_thread_new();
    assert(thread != NULL);
    mupnp_thread_setaction(thread, deleting_worker);
    assert(mupnp_thread_start(thread));
    wait_for_count(&finished, iteration + 1);
  }
  int64_t deadline = esp_timer_get_time() + 5000000;
  while (atomic_load(&allocations) != 0 && esp_timer_get_time() < deadline)
    usleep(1000);
  assert(atomic_load(&allocations) == 0);
  usleep(100000);
  started = esp_timer_get_time();
  mupnp_sleep(50);
  assert(esp_timer_get_time() - started >= 45000);
  puts("ESP-IDF pthread lifecycle host tests passed");
  return 0;
}
