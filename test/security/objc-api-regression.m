/* Objective-C wrapper API regressions (P0). Run via verify-macos.py.
 *
 * Build the wrapper sources together with this file and define
 * MUPNP_OBJC_TEST_HOOKS so that native allocation and deletion can be
 * observed. The HTTP fixture listens on 127.0.0.1 only; no LAN device or
 * multicast response is required. */
#include <mupnp/upnp.h>

#import "CGUpnpAction.h"
#import "CGUpnpControlPoint.h"
#import "CGUpnpDevice.h"
#import "CGUpnpIcon.h"
#import "CGUpnpService.h"
#import "CGUpnpStateVariable.h"
#import <Foundation/Foundation.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

extern mUpnpDevice* upnp_test_device_new(void);

extern mUpnpControlPoint* (*cg_upnp_test_controlpoint_new)(void);
extern void (*cg_upnp_test_controlpoint_delete)(mUpnpControlPoint*);
extern mUpnpDevice* (*cg_upnp_test_device_new)(void);
extern void (*cg_upnp_test_device_delete)(mUpnpDevice*);

static int failures = 0;

#define CHECK(cond)                                                   \
  do {                                                                \
    if (!(cond)) {                                                    \
      fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
      failures++;                                                     \
    }                                                                 \
  } while (0)

/****************************************
 * Native allocation hooks
 ****************************************/

static int controlPointNewCount = 0;
static int controlPointDeleteCount = 0;
static int deviceNewCount = 0;
static int deviceDeleteCount = 0;
static int wrapperDeallocCount = 0;

static mUpnpControlPoint* failing_controlpoint_new(void)
{
  return NULL;
}

static mUpnpControlPoint* counting_controlpoint_new(void)
{
  controlPointNewCount++;
  return mupnp_controlpoint_new();
}

static void counting_controlpoint_delete(mUpnpControlPoint* cp)
{
  controlPointDeleteCount++;
  mupnp_controlpoint_delete(cp);
}

static mUpnpDevice* failing_device_new(void)
{
  return NULL;
}

static mUpnpDevice* counting_device_new(void)
{
  deviceNewCount++;
  return mupnp_device_new();
}

static void counting_device_delete(mUpnpDevice* dev)
{
  deviceDeleteCount++;
  mupnp_device_delete(dev);
}

static void reset_counters(void)
{
  controlPointNewCount = controlPointDeleteCount = 0;
  deviceNewCount = deviceDeleteCount = 0;
  wrapperDeallocCount = 0;
  cg_upnp_test_controlpoint_new = counting_controlpoint_new;
  cg_upnp_test_controlpoint_delete = counting_controlpoint_delete;
  cg_upnp_test_device_new = counting_device_new;
  cg_upnp_test_device_delete = counting_device_delete;
}

static void clear_hooks(void)
{
  cg_upnp_test_controlpoint_new = NULL;
  cg_upnp_test_controlpoint_delete = NULL;
  cg_upnp_test_device_new = NULL;
  cg_upnp_test_device_delete = NULL;
}

@interface CountingControlPoint : CGUpnpControlPoint
@end

@implementation CountingControlPoint
- (void)dealloc
{
  wrapperDeallocCount++;
  [super dealloc];
}
@end

/* Fails before the native control point is started. */
@interface StartFailingControlPoint : CountingControlPoint
@end

@implementation StartFailingControlPoint
- (BOOL)start
{
  return NO;
}
@end

/* Starts the native control point, then reports failure. */
@interface LateStartFailingControlPoint : CountingControlPoint
@end

@implementation LateStartFailingControlPoint
- (BOOL)start
{
  [super start];
  return NO;
}
@end

@interface CountingDevice : CGUpnpDevice
@end

@implementation CountingDevice
- (void)dealloc
{
  wrapperDeallocCount++;
  [super dealloc];
}
@end

/****************************************
 * Descriptions
 ****************************************/

static NSString* device_description(NSString* friendlyName, int port)
{
  return [NSString stringWithFormat:
                       @"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                        "<root xmlns=\"urn:schemas-upnp-org:device-1-0\">\n"
                        "<specVersion><major>1</major><minor>0</minor></specVersion>\n"
                        "<URLBase>http://127.0.0.1:%d/</URLBase>\n"
                        "<device>\n"
                        "<deviceType>urn:schemas-upnp-org:device:BinaryLight:1</deviceType>\n"
                        "<friendlyName>%@</friendlyName>\n"
                        "<manufacturer>CyberGarage</manufacturer>\n"
                        "<modelName>Regression Light</modelName>\n"
                        "<UDN>uuid:objc-api-regression</UDN>\n"
                        "<serviceList><service>\n"
                        "<serviceType>urn:schemas-upnp-org:service:SwitchPower:1</serviceType>\n"
                        "<serviceId>urn:upnp-org:serviceId:SwitchPower.1</serviceId>\n"
                        "<SCPDURL>/scpd.xml</SCPDURL>\n"
                        "<controlURL>/control</controlURL>\n"
                        "<eventSubURL>/event</eventSubURL>\n"
                        "</service></serviceList>\n"
                        "</device>\n"
                        "</root>\n",
                   port,
                   friendlyName];
}

static NSString* const kServiceType = @"urn:schemas-upnp-org:service:SwitchPower:1";
static NSString* const kAllowedValue = @"日本語の値🎌";

static NSString* service_description_with_allowed_value(NSString* allowedValue)
{
  return [NSString stringWithFormat:
                       @"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                        "<scpd xmlns=\"urn:schemas-upnp-org:service-1-0\">\n"
                        "<specVersion><major>1</major><minor>0</minor></specVersion>\n"
                        "<actionList><action>\n"
                        "<name>SetTarget</name>\n"
                        "<argumentList>\n"
                        "<argument><name>NewTargetValue</name><direction>in</direction>"
                        "<relatedStateVariable>Target</relatedStateVariable></argument>\n"
                        "<argument><name>Result</name><direction>out</direction>"
                        "<relatedStateVariable>Target</relatedStateVariable></argument>\n"
                        "</argumentList>\n"
                        "</action></actionList>\n"
                        "<serviceStateTable>\n"
                        "<stateVariable sendEvents=\"no\"><name>Target</name><dataType>string</dataType>"
                        "<allowedValueList><allowedValue>%@</allowedValue><allowedValue>ASCII</allowedValue>"
                        "</allowedValueList></stateVariable>\n"
                        "</serviceStateTable>\n"
                        "</scpd>\n",
                   allowedValue];
}

static NSString* service_description(void)
{
  return service_description_with_allowed_value(kAllowedValue);
}

/****************************************
 * Local HTTP fixture
 ****************************************/

typedef enum {
  FixtureReplySuccess,
  FixtureReplyFault,
} FixtureReply;

typedef struct {
  int listenSocket;
  int port;
  volatile int requestCount;
  volatile FixtureReply reply;
  volatile int stopping;
  char lastBody[8192];
  pthread_mutex_t mutex;
  pthread_t thread;
} HttpFixture;

static const char* kSuccessBody = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                                  "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                                  "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                                  "<s:Body><u:SetTargetResponse xmlns:u=\"urn:schemas-upnp-org:service:SwitchPower:1\">"
                                  "<Result>結果OK</Result>"
                                  "</u:SetTargetResponse></s:Body></s:Envelope>";

static const char* kFaultBody = "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
                                "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
                                "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
                                "<s:Body><s:Fault><faultcode>s:Client</faultcode><faultstring>UPnPError</faultstring>"
                                "<detail><UPnPError xmlns=\"urn:schemas-upnp-org:control-1-0\">"
                                "<errorCode>401</errorCode><errorDescription>Invalid Action</errorDescription>"
                                "</UPnPError></detail></s:Fault></s:Body></s:Envelope>";

static void fixture_handle(HttpFixture* fixture, int client)
{
  char buf[16384];
  size_t used = 0;
  char* headerEnd = NULL;
  while (used < sizeof(buf) - 1) {
    ssize_t n = recv(client, buf + used, sizeof(buf) - 1 - used, 0);
    if (n <= 0)
      break;
    used += (size_t)n;
    buf[used] = '\0';
    headerEnd = strstr(buf, "\r\n\r\n");
    if (!headerEnd)
      continue;
    size_t contentLength = 0;
    for (char* line = strstr(buf, "\r\n"); line && line < headerEnd; line = strstr(line + 2, "\r\n")) {
      if (strncasecmp(line + 2, "Content-Length:", 15) == 0)
        contentLength = (size_t)strtoul(line + 2 + 15, NULL, 10);
    }
    if (used >= (size_t)(headerEnd + 4 - buf) + contentLength)
      break;
  }

  pthread_mutex_lock(&fixture->mutex);
  fixture->requestCount++;
  const char* body = headerEnd ? headerEnd + 4 : "";
  snprintf(fixture->lastBody, sizeof(fixture->lastBody), "%s", body);
  FixtureReply reply = fixture->reply;
  pthread_mutex_unlock(&fixture->mutex);

  const char* replyBody = (reply == FixtureReplySuccess) ? kSuccessBody : kFaultBody;
  char header[512];
  int headerLen = snprintf(header, sizeof(header), "HTTP/1.1 %s\r\nContent-Type: text/xml; charset=\"utf-8\"\r\n"
                                                   "Content-Length: %zu\r\nConnection: close\r\n\r\n",
      (reply == FixtureReplySuccess) ? "200 OK" : "500 Internal Server Error",
      strlen(replyBody));
  send(client, header, (size_t)headerLen, 0);
  send(client, replyBody, strlen(replyBody), 0);
  close(client);
}

static void* fixture_main(void* arg)
{
  HttpFixture* fixture = arg;
  for (;;) {
    int client = accept(fixture->listenSocket, NULL, NULL);
    if (client < 0)
      return NULL;
    if (fixture->stopping) {
      close(client);
      return NULL;
    }
    fixture_handle(fixture, client);
  }
}

static BOOL fixture_start(HttpFixture* fixture)
{
  memset(fixture, 0, sizeof(*fixture));
  pthread_mutex_init(&fixture->mutex, NULL);
  fixture->listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (fixture->listenSocket < 0)
    return NO;
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = 0;
  if (bind(fixture->listenSocket, (struct sockaddr*)&addr, sizeof(addr)) != 0)
    return NO;
  if (listen(fixture->listenSocket, 8) != 0)
    return NO;
  socklen_t len = sizeof(addr);
  getsockname(fixture->listenSocket, (struct sockaddr*)&addr, &len);
  fixture->port = ntohs(addr.sin_port);
  return pthread_create(&fixture->thread, NULL, fixture_main, fixture) == 0;
}

/* accept() is not interrupted by close() on every platform, so wake the
 * fixture thread with a final connection before joining it. */
static void fixture_stop(HttpFixture* fixture)
{
  fixture->stopping = 1;
  int s = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  addr.sin_port = htons((uint16_t)fixture->port);
  connect(s, (struct sockaddr*)&addr, sizeof(addr));
  pthread_join(fixture->thread, NULL);
  close(s);
  close(fixture->listenSocket);
  pthread_mutex_destroy(&fixture->mutex);
}

static int fixture_count(HttpFixture* fixture)
{
  pthread_mutex_lock(&fixture->mutex);
  int count = fixture->requestCount;
  pthread_mutex_unlock(&fixture->mutex);
  return count;
}

static BOOL fixture_last_body_contains(HttpFixture* fixture, const char* text)
{
  pthread_mutex_lock(&fixture->mutex);
  BOOL found = strstr(fixture->lastBody, text) != NULL;
  pthread_mutex_unlock(&fixture->mutex);
  return found;
}

static int unused_local_port(void)
{
  int s = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  bind(s, (struct sockaddr*)&addr, sizeof(addr));
  socklen_t len = sizeof(addr);
  getsockname(s, (struct sockaddr*)&addr, &len);
  close(s);
  return ntohs(addr.sin_port);
}

/****************************************
 * T01 StateVariable initialization
 ****************************************/

static void test_state_variable_without_native_object(void)
{
  @autoreleasepool {
    CGUpnpStateVariable* byInit = [[CGUpnpStateVariable alloc] init];
    CGUpnpStateVariable* byNull = [[CGUpnpStateVariable alloc] initWithCObject:NULL];
    for (CGUpnpStateVariable* variable in @[ byInit, byNull ]) {
      CHECK(variable != nil);
      CHECK(variable.cObject == NULL);
      CHECK([variable name] == nil);
      CHECK([variable value] == nil);
      CHECK([[variable allowedValues] count] == 0);
      CHECK(![variable isAllowedValue:@"anything"]);
      CHECK(![variable query]);
      CHECK([variable statusCode] == 0);
    }
    [byInit release];
    [byNull release];
  }
}

static void test_state_variable_keeps_native_userdata(void)
{
  @autoreleasepool {
    mUpnpDevice* native = upnp_test_device_new();
    mUpnpStateVariable* nativeVariable = mupnp_service_getstatevariables(mupnp_device_getservices(native));
    void* marker = (void*)0x5678;
    mupnp_statevariable_setuserdata(nativeVariable, marker);

    CGUpnpStateVariable* borrowed = [[CGUpnpStateVariable alloc] initWithCObject:nativeVariable];
    CHECK(mupnp_statevariable_getuserdata(nativeVariable) == marker);
    CHECK([borrowed name] != nil);
    [borrowed release];
    CHECK(mupnp_statevariable_getuserdata(nativeVariable) == marker);

    /* Variables listed by a service wrapper are borrowed the same way. */
    @autoreleasepool {
      CGUpnpDevice* device = [[CGUpnpDevice alloc] initWithCObject:native];
      CGUpnpService* service = [[device services] objectAtIndex:0];
      CHECK([[service stateVariables] count] > 0);
      CHECK(mupnp_statevariable_getuserdata(nativeVariable) == marker);
      [device release];
    }
    CHECK(mupnp_statevariable_getuserdata(nativeVariable) == marker);
    mupnp_device_delete(native);
  }
}

static void test_state_variable_without_value(void)
{
  @autoreleasepool {
    mUpnpStateVariable* native = mupnp_statevariable_new();
    CGUpnpStateVariable* variable = [[CGUpnpStateVariable alloc] initWithCObject:native];
    CHECK([variable name] == nil);
    CHECK([variable value] == nil);
    CHECK(![variable isAllowedValue:nil]);
    CHECK(mupnp_statevariable_getuserdata(native) == NULL);
    [variable release];
    mupnp_statevariable_delete(native);
  }
}

/****************************************
 * T01 Initialization failures
 ****************************************/

static void test_control_point_allocation_failure(void)
{
  reset_counters();
  cg_upnp_test_controlpoint_new = failing_controlpoint_new;
  for (int i = 0; i < 16; i++) {
    @autoreleasepool {
      CGUpnpControlPoint* cp = [[CountingControlPoint alloc] init];
      CHECK(cp == nil);
    }
  }
  CHECK(wrapperDeallocCount == 16);
  CHECK(controlPointDeleteCount == 0);
  clear_hooks();
}

static void test_control_point_start_failure(void)
{
  reset_counters();
  for (int i = 0; i < 4; i++) {
    @autoreleasepool {
      CGUpnpControlPoint* cp = [[StartFailingControlPoint alloc] init];
      CHECK(cp == nil);
    }
  }
  CHECK(controlPointNewCount == 4);
  CHECK(controlPointDeleteCount == 4);
  CHECK(wrapperDeallocCount == 4);

  reset_counters();
  for (int i = 0; i < 2; i++) {
    @autoreleasepool {
      CGUpnpControlPoint* cp = [[LateStartFailingControlPoint alloc] init];
      CHECK(cp == nil);
    }
  }
  CHECK(controlPointNewCount == 2);
  CHECK(controlPointDeleteCount == 2);
  CHECK(wrapperDeallocCount == 2);
  clear_hooks();
}

static void test_control_point_success_releases_once(void)
{
  reset_counters();
  @autoreleasepool {
    CGUpnpControlPoint* cp = [[CountingControlPoint alloc] init];
    CHECK(cp != nil);
    CHECK([cp isRunning]);
    [cp release];
  }
  CHECK(controlPointNewCount == 1);
  CHECK(controlPointDeleteCount == 1);
  CHECK(wrapperDeallocCount == 1);
  clear_hooks();
}

static void test_device_initialization_failures(void)
{
  reset_counters();
  cg_upnp_test_device_new = failing_device_new;
  for (int i = 0; i < 16; i++) {
    @autoreleasepool {
      CHECK([[CountingDevice alloc] init] == nil);
      CHECK([[CountingDevice alloc] initWithXMLDescription:device_description(@"Light", 80)] == nil);
    }
  }
  CHECK(wrapperDeallocCount == 32);
  CHECK(deviceDeleteCount == 0);

  reset_counters();
  NSArray* invalid = @[ @"", @"<root><device>", @"not xml at all", @"<root xmlns=\"urn:schemas-upnp-org:device-1-0\"></root>" ];
  for (int i = 0; i < 4; i++) {
    for (NSString* xml in invalid) {
      @autoreleasepool {
        CHECK([[CountingDevice alloc] initWithXMLDescription:xml] == nil);
      }
    }
    @autoreleasepool {
      CHECK([[CountingDevice alloc] initWithXMLDescription:nil] == nil);
    }
  }
  CHECK(deviceNewCount == 20);
  CHECK(deviceDeleteCount == 20);
  CHECK(wrapperDeallocCount == 20);

  reset_counters();
  @autoreleasepool {
    CGUpnpDevice* device = [[CountingDevice alloc] initWithXMLDescription:device_description(@"Light", 80)];
    CHECK(device != nil);
    [device release];
  }
  CHECK(deviceNewCount == 1);
  CHECK(deviceDeleteCount == 1);
  CHECK(wrapperDeallocCount == 1);
  clear_hooks();
}

/****************************************
 * T02 XML descriptions
 ****************************************/

static void test_device_description_utf8(void)
{
  NSArray* names = @[ @"Living Room Light", @"リビングの照明", @"照明💡ランプ🏠", @"Ünïcödé Light" ];
  for (NSString* name in names) {
    @autoreleasepool {
      NSString* xml = device_description(name, 80);
      CHECK([xml lengthOfBytesUsingEncoding:NSUTF8StringEncoding] >= [xml length]);

      CGUpnpDevice* created = [[CGUpnpDevice alloc] initWithXMLDescription:xml];
      CHECK(created != nil);
      CHECK([[created friendlyName] isEqualToString:name]);
      CHECK([[created modelName] isEqualToString:@"Regression Light"]);
      [created release];

      CGUpnpDevice* parsed = [[CGUpnpDevice alloc] init];
      CHECK([parsed parseXMLDescription:xml]);
      CHECK([[parsed friendlyName] isEqualToString:name]);
      CHECK([[parsed udn] isEqualToString:@"uuid:objc-api-regression"]);
      [parsed release];
    }
  }
}

static void test_device_description_rejects_invalid_input(void)
{
  @autoreleasepool {
    CGUpnpDevice* device = [[[CGUpnpDevice alloc] initWithXMLDescription:device_description(@"照明", 80)] autorelease];
    CHECK(device != nil);

    /* Rejected before reaching the native parser: the description is kept. */
    CHECK(![device parseXMLDescription:nil]);
    CHECK(![device parseXMLDescription:@""]);
    CHECK(![device parseXMLDescription:(NSString*)[NSNumber numberWithInt:1]]);
    unichar loneSurrogate = 0xD800;
    CHECK(![device parseXMLDescription:[NSString stringWithCharacters:&loneSurrogate length:1]]);
    CHECK([[device friendlyName] isEqualToString:@"照明"]);

    /* Rejected by the native parser. */
    CHECK(![device parseXMLDescription:@"<root><device><friendlyName>壊れた"]);
    CHECK(![device parseXMLDescription:@"plain text"]);

    CGUpnpDevice* empty = [[[CGUpnpDevice alloc] init] autorelease];
    CHECK(![empty parseXMLDescription:@"<root/>"]);
  }
}

static void test_service_description_utf8(void)
{
  @autoreleasepool {
    CGUpnpDevice* device = [[[CGUpnpDevice alloc] initWithXMLDescription:device_description(@"Light", 80)] autorelease];
    CGUpnpService* service = [device getServiceForType:kServiceType];
    CHECK(service != nil);
    CHECK(![service parseXMLDescription:nil]);
    CHECK(![service parseXMLDescription:@""]);
    CHECK(![service parseXMLDescription:@"<scpd><actionList>"]);
    CHECK([service parseXMLDescription:service_description()]);

    CGUpnpStateVariable* target = [service getStateVariableForName:@"Target"];
    CHECK(target != nil);
    CHECK([[target allowedValues] containsObject:kAllowedValue]);
    CHECK([service getActionForName:@"SetTarget"] != nil);

    CGUpnpService* detached = [[[CGUpnpService alloc] init] autorelease];
    CHECK(![detached parseXMLDescription:service_description()]);
  }
}

/****************************************
 * T03 Action arguments
 ****************************************/

static CGUpnpAction* action_for_port(int port, CGUpnpDevice** deviceOut)
{
  CGUpnpDevice* device = [[[CGUpnpDevice alloc] initWithXMLDescription:device_description(@"Light", port)] autorelease];
  CGUpnpService* service = [device getServiceForType:kServiceType];
  /* ASCII-only SCPD so that the action checks do not depend on the UTF-8 fix. */
  if (![service parseXMLDescription:service_description_with_allowed_value(@"OnlyASCII")])
    return nil;
  if (deviceOut)
    *deviceOut = device;
  return [service getActionForName:@"SetTarget"];
}

static void test_action_rejects_invalid_arguments(HttpFixture* fixture)
{
  @autoreleasepool {
    CGUpnpAction* action = action_for_port(fixture->port, NULL);
    CHECK(action != nil);
    CHECK([action setArgumentValue:@"initial" forName:@"NewTargetValue"]);
    int before = fixture_count(fixture);

    CHECK(![action postWithArguments:@ { @"Unknown" : @"1" }]);
    CHECK((![action postWithArguments:@ { @"NewTargetValue" : @"changed", @"Unknown" : @"1" }]));
    CHECK(![action postWithArguments:@{@"NewTargetValue" : @1}]);
    CHECK(![action postWithArguments:@ { @"NewTargetValue" : [NSNull null] }]);
    CHECK(![action postWithArguments:@{@1 : @"value"}]);
    CHECK(![action postWithArguments:@ { @"@count" : @"value" }]);
    CHECK(![action postWithArguments:(NSDictionary*)@[ @"NewTargetValue" ]]);

    CHECK(fixture_count(fixture) == before);
    CHECK([[action argumentValueForName:@"NewTargetValue"] isEqualToString:@"initial"]);
    CHECK([action argumentValueForName:@"Result"] == nil);

    CGUpnpAction* detached = [[[CGUpnpAction alloc] init] autorelease];
    CHECK(![detached postWithArguments:@ {}]);
    CHECK(fixture_count(fixture) == before);
  }
}

static void test_action_posts_valid_arguments(HttpFixture* fixture)
{
  @autoreleasepool {
    CGUpnpAction* action = action_for_port(fixture->port, NULL);
    CHECK(action != nil);
    fixture->reply = FixtureReplySuccess;

    int before = fixture_count(fixture);
    CHECK([action postWithArguments:@ { @"NewTargetValue" : @"値✓" }]);
    CHECK(fixture_count(fixture) == before + 1);
    CHECK(fixture_last_body_contains(fixture, "<NewTargetValue>値✓</NewTargetValue>"));
    CHECK([[action argumentValueForName:@"Result"] isEqualToString:@"結果OK"]);
    CHECK([action statusCode] == 0);

    /* Consecutive calls each send one request; a nil dictionary keeps the
     * legacy behavior of posting the current argument values. */
    CHECK([action postWithArguments:@ { @"NewTargetValue" : @"second" }]);
    CHECK(fixture_last_body_contains(fixture, "<NewTargetValue>second</NewTargetValue>"));
    CHECK([action postWithArguments:nil]);
    CHECK(fixture_last_body_contains(fixture, "<NewTargetValue>second</NewTargetValue>"));
    CHECK([action postWithArguments:@ {}]);
    CHECK(fixture_count(fixture) == before + 4);

    /* Output arguments may still be listed, as before. */
    CHECK(([action postWithArguments:@ { @"NewTargetValue" : @"third", @"Result" : @"" }]));
    CHECK(fixture_count(fixture) == before + 5);
  }
}

static void test_action_reports_fault_and_connection_failure(HttpFixture* fixture)
{
  @autoreleasepool {
    CGUpnpAction* action = action_for_port(fixture->port, NULL);
    fixture->reply = FixtureReplyFault;
    int before = fixture_count(fixture);
    CHECK(![action postWithArguments:@ { @"NewTargetValue" : @"fault" }]);
    CHECK(fixture_count(fixture) == before + 1);
    CHECK([action statusCode] == 401);
    fixture->reply = FixtureReplySuccess;

    CGUpnpAction* unreachable = action_for_port(unused_local_port(), NULL);
    CHECK(unreachable != nil);
    CHECK(![unreachable postWithArguments:@ { @"NewTargetValue" : @"nobody" }]);
    CHECK(fixture_count(fixture) == before + 1);
  }
}

/****************************************
 * main
 ****************************************/

static HttpFixture httpFixture;

static void run_action_rejects_invalid_arguments(void)
{
  test_action_rejects_invalid_arguments(&httpFixture);
}

static void run_action_posts_valid_arguments(void)
{
  test_action_posts_valid_arguments(&httpFixture);
}

static void run_action_reports_fault_and_connection_failure(void)
{
  test_action_reports_fault_and_connection_failure(&httpFixture);
}

typedef struct {
  const char* name;
  void (*run)(void);
} RegressionTest;

static const RegressionTest tests[] = {
  { "StateVariableWithoutNativeObject", test_state_variable_without_native_object },
  { "StateVariableKeepsNativeUserData", test_state_variable_keeps_native_userdata },
  { "StateVariableWithoutValue", test_state_variable_without_value },
  { "ControlPointAllocationFailure", test_control_point_allocation_failure },
  { "ControlPointStartFailure", test_control_point_start_failure },
  { "ControlPointSuccessReleasesOnce", test_control_point_success_releases_once },
  { "DeviceInitializationFailures", test_device_initialization_failures },
  { "DeviceDescriptionUTF8", test_device_description_utf8 },
  { "DeviceDescriptionRejectsInvalidInput", test_device_description_rejects_invalid_input },
  { "ServiceDescriptionUTF8", test_service_description_utf8 },
  { "ActionRejectsInvalidArguments", run_action_rejects_invalid_arguments },
  { "ActionPostsValidArguments", run_action_posts_valid_arguments },
  { "ActionReportsFaultAndConnectionFailure", run_action_reports_fault_and_connection_failure },
};

/* Usage: objc-api-regression [TestName ...]; runs every test by default. */
int main(int argc, char* argv[])
{
  if (!fixture_start(&httpFixture)) {
    fprintf(stderr, "FAIL: cannot start the local HTTP fixture\n");
    return 1;
  }
  for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
    BOOL selected = (argc < 2);
    for (int n = 1; n < argc; n++) {
      if (strcmp(argv[n], tests[i].name) == 0)
        selected = YES;
    }
    if (!selected)
      continue;
    int before = failures;
    @autoreleasepool {
      tests[i].run();
    }
    printf("%s: %s\n", tests[i].name, (failures == before) ? "PASS" : "FAIL");
    fflush(stdout);
  }
  fixture_stop(&httpFixture);

  if (failures) {
    fprintf(stderr, "objc-api-regression: %d failure(s)\n", failures);
    return 1;
  }
  printf("objc-api-regression: PASS\n");
  return 0;
}
