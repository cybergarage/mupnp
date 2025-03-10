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
#if !defined(WIN32) && !defined(WINCE)
#include <signal.h>
#endif

#include <mupnp/util/log.h>
#include <mupnp/util/thread.h>
#include <mupnp/util/time.h>

/* Private function prototypes */
static void sig_handler(int sign);

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
static pthread_key_t mupnp_thread_self_ref;
static pthread_once_t mupnp_thread_mykeycreated = PTHREAD_ONCE_INIT;

static void mupnp_thread_createkey(void)
{
  pthread_key_create(&mupnp_thread_self_ref, NULL);
}

mUpnpThread* mupnp_thread_self(void)
{
  return (mUpnpThread*)pthread_getspecific(mupnp_thread_self_ref);
}

static void* PosixThreadProc(void* param)
{
  mupnp_log_debug_l4("Entering...\n");

  sigset_t set;
  struct sigaction actions;
  mUpnpThread* thread = (mUpnpThread*)param;

  /* SIGQUIT is used in thread deletion routine
   * to force accept and recvmsg to return during thread
   * termination process. */
  sigfillset(&set);
  sigdelset(&set, SIGQUIT);
  pthread_sigmask(SIG_SETMASK, &set, NULL);

  memset(&actions, 0, sizeof(actions));
  sigemptyset(&actions.sa_mask);
  actions.sa_flags = 0;
  actions.sa_handler = sig_handler;
  sigaction(SIGQUIT, &actions, NULL);

  pthread_once(&mupnp_thread_mykeycreated, mupnp_thread_createkey);
  pthread_setspecific(mupnp_thread_self_ref, param);

  if (thread->action != NULL)
    thread->action(thread);

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

  if (thread->runnableFlag == true)
    mupnp_thread_stop(thread);

  mupnp_thread_remove(thread);

  free(thread);

  mupnp_log_debug_l4("Leaving...\n");

  return true;
}
#endif

/****************************************
 * mupnp_thread_start
 ****************************************/

bool mupnp_thread_start(mUpnpThread* thread)
{
  mupnp_log_debug_l4("Entering...\n");

  /**** Thanks for Visa Smolander (09/11/2005) ****/
  thread->runnableFlag = true;

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
    pthread_attr_t thread_attr;
    if (pthread_attr_init(&thread_attr) != 0) {
      thread->runnableFlag = false;
      return false;
    }
    /* MODIFICATION Fabrice Fontaine Orange 24/04/2007
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE); */
    /* Bug correction : Threads used to answer UPnP requests are created */
    /* in joinable state but the main thread doesn't call pthread_join on them. */
    /* So, they are kept alive until the end of the program. By creating them */
    /* in detached state, they are correctly clean up */
    if (pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED) != 0) {
      thread->runnableFlag = false;
      pthread_attr_destroy(&thread_attr);
      return false;
    }
#ifdef STACK_SIZE
    /* Optimization : not we can set STACK_SIZE attribute at compilation time */
    /* to specify thread stack size */
    if (pthread_attr_setstacksize(&thread_attr, STACK_SIZE) != 0) {
      thread->runnableFlag = false;
      pthread_attr_destroy(&thread_attr);
      return false;
    }
#endif
    /* MODIFICATION END Fabrice Fontaine Orange 24/04/2007 */
    if (pthread_create(&thread->pThread, &thread_attr, PosixThreadProc, thread) != 0) {
      thread->runnableFlag = false;
      pthread_attr_destroy(&thread_attr);
      return false;
    }
    pthread_attr_destroy(&thread_attr);
#endif

  mupnp_log_debug_l4("Leaving...\n");

  return true;
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
#if defined(WINCE)
  int i, j;
  bool result;
  DWORD dwExitCode;
#endif

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

  mupnp_thread_stop(thread);
  mupnp_thread_start(thread);
  return true;

  mupnp_log_debug_l4("Leaving...\n");
}

/****************************************
 * mupnp_thread_isrunnable
 ****************************************/

bool mupnp_thread_isrunnable(mUpnpThread* thread)
{
  mupnp_log_debug_l4("Entering...\n");

#if !defined(WIN32) && !defined(WINCE) && !defined(ITRON) && !defined(BTRON) && !defined(TENGINE) && !defined(PROCESS_BASE)
  pthread_testcancel();
#endif

  return thread->runnableFlag;

  mupnp_log_debug_l4("Leaving...\n");
}

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

static void sig_handler(int sign)
{
  mupnp_log_debug_s("Got signal %d.\n", sign);
  return;
}
