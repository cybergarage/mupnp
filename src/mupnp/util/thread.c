/******************************************************************
 *
 * mUPnP for C
 *
 * Copyright (C) Satoshi Konno 2005
 * Copyright (C) 2006 Nokia Corporation. All rights reserved.
 *
 * This is licensed under BSD-style license, see file COPYING.
 *
 ******************************************************************/

#include <string.h>
#if defined(ESP_PLATFORM)
#include "sdkconfig.h"
#endif
#if !defined(WIN32) && !defined(WINCE) && !defined(ESP_PLATFORM)
#include <signal.h>
#endif

#include <mupnp/util/log.h>
#include <mupnp/util/thread.h>
#include <mupnp/util/time.h>

/* Private function prototypes */
#if !defined(ESP_PLATFORM)
static void sig_handler(int sign);
#endif

/****************************************
 * Thread Function
 ****************************************/

#if defined(WIN32) && !defined(WINCE) && !defined(ITRON)
static DWORD WINAPI Win32ThreadProc(LPVOID lpParam)
{
  mUpnpThread* thread;

  mupnp_log_debug_l4("Entering...\n");

  thread = (mUpnpThread*)lpParam;
  if (thread->action != NULL)
    thread->action(thread);

  return 0;
}
#elif defined(WINCE)
static DWORD WINAPI Win32ThreadProc(LPVOID lpParam)
{
  mUpnpThread* thread = (mUpnpThread*)lpParam;

  // Theo Beisch: make sure we're runnable
  // thread->runnableFlag = true;
  //(moved to start() with Visa Smolander's mod)

  thread->isRunning = true;

#if defined DEBUG_LOCKS
  memdiags_tlist_addthread(thread);
#endif

  if (thread->action != NULL) {
#if defined DEBUG
    printf("#real Thr=%X hNd=%X lpP=%X %s start\n", thread, thread->hThread, lpParam, thread->friendlyName);
#endif
    thread->action(thread);
  }
#if defined DEBUG
  printf("** REAL ExitThread(0)*\n");
  printf("*2 Thread ret4close 0x%Xh\n", GetCurrentThreadId());
  printf("*3 Thread %X %s \n", thread, thread->friendlyName);
#endif
  thread->isRunning = false;
  if (thread->deletePending) {
    mupnp_thread_delete(thread);
  }
  // proper friendly thread exit for WINCE
  mupnp_thread_exit(0);
  // dummy - compiler wants a return statement
  return 0;
}
#elif defined(BTRON)
static VOID BTronTaskProc(W param)
{
  mupnp_log_debug_l4("Entering...\n");

  mUpnpThread* thread = (mUpnpThread*)param;
  if (thread->action != NULL)
    thread->action(thread);
  ext_tsk();

  mupnp_log_debug_l4("Leaving...\n");
}
#elif defined(ITRON)
static TASK ITronTaskProc(int param)
{
  mupnp_log_debug_l4("Entering...\n");

  T_RTSK rtsk;
  mUpnpThread* thread;
  if (ref_tsk(TSK_SELF, &rtsk) != E_OK)
    return;
  thread = (mUpnpThread*)rtsk.exinf;
  if (thread->action != NULL)
    thread->action(thread);
  exd_tsk();

  mupnp_log_debug_l4("Leaving...\n");
}
#elif defined(TENGINE) && !defined(PROCESS_BASE)
static VOID TEngineTaskProc(INT stacd, VP param)
{
  mupnp_log_debug_l4("Entering...\n");

  mUpnpThread* thread = (mUpnpThread*)param;
  if (thread->action != NULL)
    thread->action(thread);
  tk_exd_tsk();

  mupnp_log_debug_l4("Leaving...\n");
}
#elif defined(TENGINE) && defined(PROCESS_BASE)
static VOID TEngineProcessBasedTaskProc(W param)
{
  mupnp_log_debug_l4("Entering...\n");

  mUpnpThread* thread = (mUpnpThread*)param;
  if (thread->action != NULL)
    thread->action(thread);
  b_ext_tsk();

  mupnp_log_debug_l4("Leaving...\n");
}
#else

/* Key used to store self reference in (p)thread global storage */
static pthread_key_t mupnpThreadSelfRef;
static pthread_once_t mupnpThreadMykeycreated = PTHREAD_ONCE_INIT;
#if defined(ESP_PLATFORM)
static int mupnpThreadKeyResult;
#endif

static void mupnp_thread_createkey(void)
{
#if defined(ESP_PLATFORM)
  mupnpThreadKeyResult = pthread_key_create(&mupnpThreadSelfRef, NULL);
#else
  pthread_key_create(&mupnpThreadSelfRef, NULL);
#endif
}

mUpnpThread* mupnp_thread_self(void)
{
#if defined(ESP_PLATFORM)
  /* Socket helpers also call this from app_main and other non-mUPnP tasks. */
  if (pthread_once(&mupnpThreadMykeycreated, mupnp_thread_createkey) != 0 || mupnpThreadKeyResult != 0)
    return NULL;
#else
  pthread_once(&mupnpThreadMykeycreated, mupnp_thread_createkey);
#endif
  return (mUpnpThread*)pthread_getspecific(mupnpThreadSelfRef);
}

static void* posix_thread_proc(void* param)
{
  mupnp_log_debug_l4("Entering...\n");

#if !defined(ESP_PLATFORM)
  sigset_t set;
#endif
  mUpnpThread* thread = (mUpnpThread*)param;

#if !defined(ESP_PLATFORM)
  /* SIGQUIT is used in thread deletion routine
   * to force accept and recvmsg to return during thread
   * termination process. */
  sigfillset(&set);
  sigdelset(&set, SIGQUIT);
  pthread_sigmask(SIG_SETMASK, &set, NULL);

#endif

#if defined(ESP_PLATFORM)
  /* Do not run before start has stored the pthread handle and lifecycle state. */
  pthread_mutex_lock(&thread->stateMutex);
  pthread_mutex_unlock(&thread->stateMutex);
  if (pthread_setspecific(mupnpThreadSelfRef, param) == 0 && thread->action != NULL)
    thread->action(thread);

  pthread_mutex_lock(&thread->stateMutex);
  thread->runnableFlag = false;
  thread->threadRunning = false;
  bool deletePending = thread->deletePending;
  pthread_cond_broadcast(&thread->stateCond);
  pthread_mutex_unlock(&thread->stateMutex);

  pthread_setspecific(mupnpThreadSelfRef, NULL);
  if (deletePending) {
    /* Self-owned notification workers have no external thread to join them. */
    pthread_detach(pthread_self());
    pthread_cond_destroy(&thread->stateCond);
    pthread_mutex_destroy(&thread->stateMutex);
    free(thread);
  }
#else
  pthread_once(&mupnpThreadMykeycreated, mupnp_thread_createkey);
  pthread_setspecific(mupnpThreadSelfRef, param);

  if (thread->action != NULL)
    thread->action(thread);

  pthread_mutex_lock(&thread->stateMutex);
  thread->isRunning = false;
  thread->runnableFlag = false;
  bool destroy = thread->deletePending && !thread->stopWaiting;
  pthread_cond_broadcast(&thread->completion);
  pthread_mutex_unlock(&thread->stateMutex);
  pthread_setspecific(mupnpThreadSelfRef, NULL);
  if (destroy) {
    pthread_cond_destroy(&thread->completion);
    pthread_mutex_destroy(&thread->stateMutex);
    free(thread);
  }

#endif

  mupnp_log_debug_l4("Leaving...\n");

  return 0;
}
#endif

/****************************************
 * mupnp_thread_new
 ****************************************/

mUpnpThread* mupnp_thread_new(void)
{
  mUpnpThread* thread;

  mupnp_log_debug_l4("Entering...\n");

  thread = (mUpnpThread*)malloc(sizeof(mUpnpThread));

  mupnp_log_debug_s("Creating thread data into %p\n", thread);

  if (NULL != thread) {
    mupnp_list_node_init((mUpnpList*)thread);

    thread->runnableFlag = false;
    thread->action = NULL;
    thread->userData = NULL;
#if defined(ESP_PLATFORM)
    thread->threadStarted = false;
    thread->threadRunning = false;
    thread->joinInProgress = false;
    thread->deletePending = false;
    if (pthread_mutex_init(&thread->stateMutex, NULL) != 0) {
      free(thread);
      return NULL;
    }
    if (pthread_cond_init(&thread->stateCond, NULL) != 0) {
      pthread_mutex_destroy(&thread->stateMutex);
      free(thread);
      return NULL;
    }
#endif
#if !defined(WIN32) && !defined(WINCE) && !defined(BTRON) && !defined(ITRON) && !defined(TENGINE) && !defined(ESP_PLATFORM)
    if (pthread_mutex_init(&thread->stateMutex, NULL) != 0) {
      free(thread);
      return NULL;
    }
    if (pthread_cond_init(&thread->completion, NULL) != 0) {
      pthread_mutex_destroy(&thread->stateMutex);
      free(thread);
      return NULL;
    }
    thread->isRunning = false;
    thread->stopWaiting = false;
    thread->deletePending = false;
#endif
  }

#if defined(WINCE)
  thread->hThread = NULL;
  // WINCE trial result: default sleep value to keep system load down
  thread->sleep = MUPNP_THREAD_MIN_SLEEP;
  thread->isRunning = false;
  thread->deletePending = false;
#if defined DEBUG
  strcpy(thread->friendlyName, "-");
#endif // DEBUG
#endif // WINCE
  mupnp_log_debug_l4("Leaving...\n");

  return thread;
}

/****************************************
 * mupnp_thread_delete
 ****************************************/

bool mupnp_thread_delete(mUpnpThread* thread)
{
#if defined(ESP_PLATFORM)
  if (thread == NULL)
    return false;
  if (mupnp_thread_self() == thread) {
    pthread_mutex_lock(&thread->stateMutex);
    /* An external joiner owns cleanup if shutdown has already begun. */
    if (thread->joinInProgress) {
      pthread_mutex_unlock(&thread->stateMutex);
      return false;
    }
    thread->runnableFlag = false;
    thread->deletePending = true;
    pthread_cond_broadcast(&thread->stateCond);
    pthread_mutex_unlock(&thread->stateMutex);
    mupnp_thread_remove(thread);
    return true;
  }
  if (!mupnp_thread_stop(thread))
    return false;
  mupnp_thread_remove(thread);
  pthread_cond_destroy(&thread->stateCond);
  pthread_mutex_destroy(&thread->stateMutex);
  free(thread);
  return true;
}
#else
#if defined WINCE
  bool stop = false;

  mupnp_log_debug_l4("Entering...\n");

  if ((thread->hThread == NULL) || ((thread->isRunning) && (stop = mupnp_thread_stop(thread) == true)) || (thread->isRunning == false)) {

#if defined DEBUG
    printf("***** Delete entered handle=%x isRunning=%d stopResult=%d \n", thread->hThread, thread->isRunning, stop);
    printf("***** Delete and free for Thread %X %s\n", thread, thread->friendlyName);
#endif

    if (thread->hThread != NULL)
      CloseHandle(thread->hThread);
    mupnp_list_remove((mUpnpList*)thread);
#if defined DEBUG_MEM
    memdiags_tlist_removethread(thread);
#endif
    free(thread);
    return true;
  }

#if defined DEBUG
  printf("***** Stop failed for Thread %X %s - marking thread for selfDelete\n", thread, thread->friendlyName);
#endif
  // setting this will cause the real thread exit to call delete() again
  thread->deletePending = true;

  mupnp_log_debug_l4("Leaving...\n");

  return false;
} // WINCE
#else // all except WINCE

  mupnp_log_debug_l4("Entering...\n");

  if (!thread)
    return false;
#if !defined(WIN32) && !defined(BTRON) && !defined(ITRON) && !defined(TENGINE)
  if (mupnp_thread_self() == thread) {
    pthread_mutex_lock(&thread->stateMutex);
    thread->deletePending = true;
    thread->runnableFlag = false;
    pthread_mutex_unlock(&thread->stateMutex);
    return true;
  }
  mupnp_thread_stop(thread);
  pthread_cond_destroy(&thread->completion);
  pthread_mutex_destroy(&thread->stateMutex);
#else
  if (thread->runnableFlag)
    mupnp_thread_stop(thread);
#endif

  mupnp_thread_remove(thread);

  free(thread);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}
#endif

#endif

/****************************************
 * mupnp_thread_start
 ****************************************/

bool mupnp_thread_start(mUpnpThread* thread)
{
#if defined(ESP_PLATFORM)
  pthread_attr_t threadAttr;
  int result;

  if (thread == NULL || thread->action == NULL)
    return false;
  if (pthread_once(&mupnpThreadMykeycreated, mupnp_thread_createkey) != 0 || mupnpThreadKeyResult != 0)
    return false;

  pthread_mutex_lock(&thread->stateMutex);
  if (thread->threadRunning || thread->joinInProgress || thread->deletePending) {
    pthread_mutex_unlock(&thread->stateMutex);
    return false;
  }
  /* A naturally completed worker still owns a joinable pthread handle. */
  if (thread->threadStarted) {
    if (pthread_join(thread->pThread, NULL) != 0) {
      pthread_mutex_unlock(&thread->stateMutex);
      return false;
    }
    thread->threadStarted = false;
  }
  result = pthread_attr_init(&threadAttr);
  if (result == 0) {
    result = pthread_attr_setstacksize(&threadAttr, CONFIG_MUPNP_THREAD_STACK_SIZE);
    if (result == 0) {
      thread->runnableFlag = true;
      thread->threadRunning = true;
      result = pthread_create(&thread->pThread, &threadAttr, posix_thread_proc, thread);
      thread->threadStarted = (result == 0);
      if (result != 0) {
        thread->runnableFlag = false;
        thread->threadRunning = false;
      }
    }
    pthread_attr_destroy(&threadAttr);
  }
  pthread_mutex_unlock(&thread->stateMutex);
  return result == 0;
#else

  mupnp_log_debug_l4("Entering...\n");

  /**** Thanks for Visa Smolander (09/11/2005) ****/
#if defined(WIN32) || defined(WINCE) || defined(BTRON) || defined(ITRON) || defined(TENGINE)
  thread->runnableFlag = true;
#endif

#if defined(WIN32) && !defined(WINCE) && !defined(ITRON)
  thread->hThread = CreateThread(NULL, 0, Win32ThreadProc, (LPVOID)thread, 0, &thread->threadID);
#elif defined(WINCE)
  {

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = true;
    saAttr.lpSecurityDescriptor = NULL;
    thread->deletePending = false;
    thread->hThread = CreateThread(&saAttr, 0, Win32ThreadProc, (LPVOID)thread, 0, &thread->threadID);
  }
#elif defined(BTRON)
    P_STATE pstate;
    prc_sts(0, &pstate, NULL);
    thread->taskID = cre_tsk(BTronTaskProc, pstate.priority, (W)thread);
    if (thread->taskID < 0) {
      thread->runnableFlag = false;
      return false;
    }
#elif defined(ITRON)
    T_CTSK ctsk = { TA_HLNG, (VP_INT)thread, ITronTaskProc, 6, 512, NULL, NULL };
    thread->taskID = acre_tsk(&ctsk);
    if (thread->taskID < 0) {
      thread->runnableFlag = false;
      return false;
    }
    if (sta_tsk(thread->taskID, 0) != E_OK) {
      thread->runnableFlag = false;
      del_tsk(thread->taskID);
      return false;
    }
#elif defined(TENGINE) && !defined(PROCESS_BASE)
    T_CTSK ctsk = { (VP)thread, TA_HLNG, TEngineTaskProc, 10, 2048 };
    thread->taskID = tk_cre_tsk(&ctsk);
    if (thread->taskID < E_OK) {
      thread->runnableFlag = false;
      return false;
    }
    if (tk_sta_tsk(thread->taskID, 0) < E_OK) {
      thread->runnableFlag = false;
      tk_del_tsk(thread->taskID);
      return false;
    }
#elif defined(TENGINE) && defined(PROCESS_BASE)
    P_STATE pstate;
    b_prc_sts(0, &pstate, NULL);
    thread->taskID = b_cre_tsk(TEngineProcessBasedTaskProc, pstate.priority, (W)thread);
    if (thread->taskID < 0) {
      thread->runnableFlag = false;
      return false;
    }
#else
    pthread_attr_t threadAttr;
    pthread_mutex_lock(&thread->stateMutex);
    if (thread->isRunning) {
      pthread_mutex_unlock(&thread->stateMutex);
      return false;
    }
#if !defined(ESP_PLATFORM)
    struct sigaction actions;
    memset(&actions, 0, sizeof(actions));
    sigemptyset(&actions.sa_mask);
    actions.sa_handler = sig_handler;
    if (sigaction(SIGQUIT, &actions, NULL) != 0) {
      pthread_mutex_unlock(&thread->stateMutex);
      return false;
    }
#endif
    thread->stopWaiting = false;
    thread->deletePending = false;
    thread->isRunning = true;
    thread->runnableFlag = true;
    if (pthread_attr_init(&threadAttr) != 0) {
      thread->isRunning = false;
      thread->runnableFlag = false;
      pthread_mutex_unlock(&thread->stateMutex);
      return false;
    }
    int result = pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);
#ifdef STACK_SIZE
    if (result == 0)
      result = pthread_attr_setstacksize(&threadAttr, STACK_SIZE);
#endif
    if (result == 0)
      result = pthread_create(&thread->pThread, &threadAttr, posix_thread_proc, thread);
    pthread_attr_destroy(&threadAttr);
    if (result != 0) {
      thread->isRunning = false;
      thread->runnableFlag = false;
    }
    pthread_mutex_unlock(&thread->stateMutex);
    if (result != 0)
      return false;
#endif

  mupnp_log_debug_l4("Leaving...\n");

  return true;
#endif
}

/****************************************
 * mupnp_thread_stop
 ****************************************/

bool mupnp_thread_stop(mUpnpThread* thread)
{
  return mupnp_thread_stop_with_cond(thread, NULL);
}

bool mupnp_thread_stop_with_cond(mUpnpThread* thread, mUpnpCond* cond)
{
#if defined(ESP_PLATFORM)
  int result = 0;
  if (thread == NULL)
    return false;
  pthread_mutex_lock(&thread->stateMutex);
  thread->runnableFlag = false;
  pthread_cond_broadcast(&thread->stateCond);
  if (cond != NULL)
    mupnp_cond_signal(cond);
  if (mupnp_thread_self() == thread) {
    pthread_mutex_unlock(&thread->stateMutex);
    return true;
  }
  /* Self-deletion transfers lifetime to the wrapper; never join/free it twice. */
  if (thread->deletePending) {
    pthread_mutex_unlock(&thread->stateMutex);
    return false;
  }
  while (thread->joinInProgress)
    pthread_cond_wait(&thread->stateCond, &thread->stateMutex);
  /* A restart could acquire the mutex between two concurrent stop requests. */
  thread->runnableFlag = false;
  pthread_cond_broadcast(&thread->stateCond);
  if (thread->threadStarted) {
    thread->joinInProgress = true;
    pthread_mutex_unlock(&thread->stateMutex);
    result = pthread_join(thread->pThread, NULL);
    pthread_mutex_lock(&thread->stateMutex);
    if (result == 0)
      thread->threadStarted = false;
    thread->joinInProgress = false;
    pthread_cond_broadcast(&thread->stateCond);
  }
  pthread_mutex_unlock(&thread->stateMutex);
  return result == 0;
#else

#if defined(WINCE)
  int i, j;
  bool result;
  DWORD dwExitCode;
#endif

#if !defined(WIN32) && !defined(WINCE) && !defined(BTRON) && !defined(ITRON) && !defined(TENGINE)
  if (!thread)
    return false;
  pthread_mutex_lock(&thread->stateMutex);
  thread->runnableFlag = false;
  if (cond)
    mupnp_cond_signal(cond);
  if (mupnp_thread_self() != thread) {
    thread->stopWaiting = true;
    while (thread->isRunning) {
      if (cond)
        mupnp_cond_signal(cond);
#if !defined(ESP_PLATFORM)
      pthread_kill(thread->pThread, SIGQUIT);
#endif
      /* Repeat the wakeup: the action may enter I/O just after a signal. */
      struct timespec until;
      clock_gettime(CLOCK_REALTIME, &until);
      until.tv_nsec += 100000000;
      if (until.tv_nsec >= 1000000000) {
        until.tv_sec++;
        until.tv_nsec -= 1000000000;
      }
      pthread_cond_timedwait(&thread->completion, &thread->stateMutex, &until);
    }
  }
  pthread_mutex_unlock(&thread->stateMutex);
  return true;
#else
  mupnp_log_debug_l4("Entering...\n");

  mupnp_log_debug_s("Stopping thread %p\n", thread);

  if (thread->runnableFlag == true) {
    thread->runnableFlag = false;
    if (cond != NULL) {
      mupnp_cond_signal(cond);
    }
#if defined(WIN32) && !defined(WINCE) && !defined(ITRON)
    TerminateThread(thread->hThread, 0);
    WaitForSingleObject(thread->hThread, INFINITE);
// tb: this will create a deadlock if the thread is on a blocking socket
#elif defined(WINCE)
    // Theo Beisch: while the above code apparently works under WIN32 (NT/XP)
    // TerminateThread as brute force is not recommended by M$
    // (see API) and actually crashes WCE
    // WINCE provides no safe means of terminating a thread,
    // so we can only mark the mupnp_thread (context) for later deletion and
    // do the delete(thread) cleanup on return of the Win32ThreadProc.
    // Accordingly we simulate the OK exit here as a "look ahead" (what a hack ;-) )
    for (i = 0; i < MUPNP_THREAD_SHUTDOWN_ATTEMPTS; ++i) {
#if defined(DEBUG)
      printf("# thread stop mainloop %X %s %d. try\n", thread, thread->friendlyName, i + 1);
#endif
      j = 0;
      if (result = GetExitCodeThread(thread->hThread, &dwExitCode)) {
        if (dwExitCode != STILL_ACTIVE) {
#if defined(DEBUG)
          printf("Thread %X %s ended graceful: xCode=%d\n", thread, thread->friendlyName, dwExitCode);
#endif
          return true;
        }
      }
      mupnp_wait(MUPNP_THREAD_MIN_SLEEP);
    }
// ok - if everything up to here failed
#if defined DEBUG
    if (dwExitCode) {
      printf("Thread %X - %s has not yet terminated - exit code %x \n", thread, thread->friendlyName, dwExitCode);
    }
#endif

    if (dwExitCode)
      return false;

    return true;
// end WINCE
#elif defined(BTRON)
    ter_tsk(thread->taskID);
#elif defined(ITRON)
    ter_tsk(thread->taskID);
    del_tsk(thread->taskID);
#elif defined(TENGINE) && !defined(PROCESS_BASE)
    tk_ter_tsk(thread->taskID);
    tk_del_tsk(thread->taskID);
#elif defined(TENGINE) && defined(PROCESS_BASE)
    b_ter_tsk(thread->taskID);
#else
    mupnp_log_debug_s("Killing thread %p\n", thread);
    pthread_kill(thread->pThread, 0);
    /* MODIFICATION Fabrice Fontaine Orange 24/04/2007
              mupnp_log_debug_s("Thread %p signalled, joining.\n", thread);
              pthread_join(thread->pThread, NULL);
              mupnp_log_debug_s("Thread %p joined.\n", thread); */
    /* Now we wait one second for thread termination instead of using pthread_join */
    mupnp_sleep(MUPNP_THREAD_MIN_SLEEP);
/* MODIFICATION END Fabrice Fontaine Orange 24/04/2007 */
#endif
  }

  mupnp_log_debug_l4("Leaving...\n");

  return true;
#endif
#endif
}

/****************************************
 * mupnp_thread_sleep
 ****************************************/
// Theo Beisch
// Added this to improve external thread control
// by having a finer timer tick granularity

#if defined(WINCE)
void mupnp_thread_sleep(mUpnpThread* thread)
{
  mUpnpTime until;
#if defined DEBUG_MEM
  printf("###### Going to sleep - elapsed since last sleep = %d\n", memdiags_getelapsedtime(thread->hThread));
#endif
  until = mupnp_getcurrentsystemtime() + (thread->sleep) / 1000;
  while ((mupnp_getcurrentsystemtime() < until) && thread->runnableFlag) {
    mupnp_wait(0);
  }
}
#endif

/****************************************
 * mupnp_thread_exit (friendly exit)
 ****************************************/
// Theo Beisch
// to be called from the thread's loop only

#if defined(WINCE)
VOID mupnp_thread_exit(DWORD exitCode)
{
  ExitThread(exitCode);
}
#endif

/****************************************
 * mupnp_thread_restart
 ****************************************/

bool mupnp_thread_restart(mUpnpThread* thread)
{
  mupnp_log_debug_l4("Entering...\n");

#if defined(ESP_PLATFORM)
  return mupnp_thread_stop(thread) && mupnp_thread_start(thread);
#else
  mupnp_thread_stop(thread);
  mupnp_thread_start(thread);
#endif
  return true;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_thread_isrunnable
 ****************************************/

bool mupnp_thread_isrunnable(mUpnpThread* thread)
{
#if defined(ESP_PLATFORM)
  bool runnable;
  if (thread == NULL)
    return false;
  pthread_mutex_lock(&thread->stateMutex);
  runnable = thread->runnableFlag;
  pthread_mutex_unlock(&thread->stateMutex);
  return runnable;
#else
  mupnp_log_debug_l4("Entering...\n");

#if !defined(WIN32) && !defined(WINCE) && !defined(ITRON) && !defined(BTRON) && !defined(TENGINE) && !defined(PROCESS_BASE)
  pthread_testcancel();
#endif

#if !defined(WIN32) && !defined(WINCE) && !defined(BTRON) && !defined(ITRON) && !defined(TENGINE)
  pthread_mutex_lock(&thread->stateMutex);
  bool runnable = thread->runnableFlag;
  pthread_mutex_unlock(&thread->stateMutex);
  return runnable;
#else
  return thread->runnableFlag;
#endif

  mupnp_log_debug_l4("Leaving...\n");
#endif
}

#if defined(ESP_PLATFORM)
bool mupnp_thread_isrunning(mUpnpThread* thread)
{
  bool running;
  if (thread == NULL)
    return false;
  pthread_mutex_lock(&thread->stateMutex);
  running = thread->threadRunning;
  pthread_mutex_unlock(&thread->stateMutex);
  return running;
}
#endif

/****************************************
 * mupnp_thread_setaction
 ****************************************/

void mupnp_thread_setaction(mUpnpThread* thread, MUPNP_THREAD_FUNC func)
{
  mupnp_log_debug_l4("Entering...\n");

  thread->action = func;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_thread_setuserdata
 ****************************************/

void mupnp_thread_setuserdata(mUpnpThread* thread, void* value)
{
  mupnp_log_debug_l4("Entering...\n");

  thread->userData = value;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_thread_getuserdata
 ****************************************/

void* mupnp_thread_getuserdata(mUpnpThread* thread)
{
  mupnp_log_debug_l4("Entering...\n");

  return thread->userData;

  mupnp_log_debug_l4("Leaving...\n");
}

/* Private helper functions */

#if !defined(ESP_PLATFORM)
static void sig_handler(int sign)
{
  (void)sign;
  return;
}
#endif
