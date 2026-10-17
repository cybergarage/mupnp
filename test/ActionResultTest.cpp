#include <boost/test/unit_test.hpp>
#include <cstring>
#include <mupnp/control/control.h>
#include <mupnp/device.h>
#include <string>

static const char* scpd = "<scpd><actionList><action><name>Read</name><argumentList>"
                          "<argument><name>Input</name><direction>in</direction></argument>"
                          "<argument><name>Output</name><direction>out</direction></argument>"
                          "</argumentList></action></actionList></scpd>";

struct ActionFixture {
  mUpnpDevice* device = mupnp_device_new();
  mUpnpService* service;
  mUpnpAction* action;
  ActionFixture()
  {
    const char* xml = "<root><device><UDN>uuid:root</UDN><serviceList><service><serviceType>urn:test:1</serviceType><serviceId>test</serviceId><controlURL>/control</controlURL></service></serviceList></device></root>";
    BOOST_REQUIRE(mupnp_device_parsedescription(device, xml, strlen(xml)));
    service = mupnp_device_getservices(device);
    BOOST_REQUIRE(mupnp_service_parsedescription(service, scpd, strlen(scpd)));
    action = mupnp_service_getactionbyname(service, "Read");
    BOOST_REQUIRE(action);
    mupnp_action_setargumentvaluebyname(action, "Output", "old");
    mupnp_action_setstatuscode(action, 999);
  }
  ~ActionFixture()
  {
    if (device)
      mupnp_device_delete(device);
  }
  mUpnpActionResult* decode(const std::string& body, int status = 200)
  {
    return mupnp_action_result_fromresponse(action, status, body.data(), body.size());
  }
};

static std::string envelope(const std::string& body)
{
  return "<s:Envelope xmlns:s='http://schemas.xmlsoap.org/soap/envelope/'><s:Header/><s:Body>" + body + "</s:Body></s:Envelope>";
}

BOOST_FIXTURE_TEST_CASE(ActionResultsAreFresh, ActionFixture)
{
  auto first = decode(envelope("<u:ReadResponse xmlns:u='urn:test:1'><Output>new</Output></u:ReadResponse>"));
  BOOST_REQUIRE(first);
  BOOST_CHECK_EQUAL(mupnp_action_result_getcode(first), MUPNP_ACTION_RESULT_SUCCESS);
  BOOST_CHECK_EQUAL(mupnp_action_result_getoutput(first, "Output"), "new");
  for (int i = 0; i < 20; ++i) {
    auto missing = decode(envelope("<u:ReadResponse xmlns:u='urn:test:1'/>"));
    BOOST_REQUIRE(missing);
    BOOST_CHECK_EQUAL(mupnp_action_result_getcode(missing), MUPNP_ACTION_RESULT_INCOMPLETE);
    BOOST_CHECK(!mupnp_action_result_getoutput(missing, "Output"));
    auto empty = decode(envelope("<u:ReadResponse xmlns:u='urn:test:1'><Output/></u:ReadResponse>"));
    BOOST_REQUIRE(empty);
    BOOST_CHECK_EQUAL(mupnp_action_result_getcode(empty), MUPNP_ACTION_RESULT_SUCCESS);
    BOOST_REQUIRE(mupnp_action_result_getoutput(empty, "Output"));
    BOOST_CHECK_EQUAL(mupnp_action_result_getoutput(empty, "Output"), "");
    mupnp_action_result_delete(empty);
    mupnp_action_result_delete(missing);
  }
  BOOST_CHECK_EQUAL(mupnp_action_getargumentvaluebyname(action, "Output"), "old");
  BOOST_CHECK_EQUAL(mupnp_action_getstatuscode(action), 999);
  mupnp_device_delete(device);
  device = NULL;
  BOOST_CHECK_EQUAL(mupnp_action_result_getoutput(first, "Output"), "new");
  mupnp_action_result_delete(first);
}

BOOST_FIXTURE_TEST_CASE(ActionFaultAndFailureLayers, ActionFixture)
{
  auto fault = decode(envelope("<s:Fault><faultcode>s:Client</faultcode><faultstring>UPnPError</faultstring><detail><UPnPError xmlns='urn:schemas-upnp-org:control-1-0'><errorCode>713</errorCode><errorDescription>NoSuchEntryInArray</errorDescription></UPnPError></detail></s:Fault>"), 500);
  BOOST_REQUIRE(fault);
  BOOST_CHECK_EQUAL(mupnp_action_result_getcode(fault), MUPNP_ACTION_RESULT_SOAP_FAULT);
  BOOST_CHECK_EQUAL(mupnp_action_result_gethttpstatuscode(fault), 500);
  BOOST_CHECK_EQUAL(mupnp_action_result_getupnperrorcode(fault), 713);
  BOOST_CHECK_EQUAL(mupnp_action_result_getfaultcode(fault), "s:Client");
  BOOST_CHECK_EQUAL(mupnp_action_result_getfaultstring(fault), "UPnPError");
  BOOST_CHECK_EQUAL(mupnp_action_result_geterrordescription(fault), "NoSuchEntryInArray");
  BOOST_CHECK(!mupnp_action_result_getoutput(fault, "Output"));
  mupnp_action_result_delete(fault);
  auto soap = decode(envelope("<s:Fault><faultcode>s:Server</faultcode><faultstring>failure</faultstring></s:Fault>"));
  BOOST_CHECK_EQUAL(mupnp_action_result_getcode(soap), MUPNP_ACTION_RESULT_SOAP_FAULT);
  BOOST_CHECK_EQUAL(mupnp_action_result_getupnperrorcode(soap), 0);
  mupnp_action_result_delete(soap);
  for (auto status : { 200, 404, 0 }) {
    auto result = decode("", status);
    BOOST_REQUIRE(result);
    BOOST_CHECK_EQUAL(mupnp_action_result_getcode(result), status == 0 ? MUPNP_ACTION_RESULT_TRANSPORT_ERROR : status == 404 ? MUPNP_ACTION_RESULT_HTTP_ERROR
                                                                                                                             : MUPNP_ACTION_RESULT_PARSE_ERROR);
    mupnp_action_result_delete(result);
  }
  for (auto xml : { "<broken", "<Envelope><Body><ReadResponse/></Body></Envelope>", "<s:Envelope xmlns:s='http://schemas.xmlsoap.org/soap/envelope/'><s:Body/></s:Envelope>" }) {
    auto result = decode(xml);
    BOOST_CHECK_EQUAL(mupnp_action_result_getcode(result), MUPNP_ACTION_RESULT_PARSE_ERROR);
    mupnp_action_result_delete(result);
  }
  for (auto body : { "<u:OtherResponse xmlns:u='urn:test:1'/>", "<u:ReadResponse xmlns:u='urn:wrong'/>", "<u:ReadResponse xmlns:u='urn:test:1'><Output>a</Output><Output>b</Output></u:ReadResponse>", "<u:ReadResponse xmlns:u='urn:test:1'><Input>wrong</Input></u:ReadResponse>", "<u:ReadResponse xmlns:u='urn:test:1'><Output><nested/></Output></u:ReadResponse>" }) {
    auto result = decode(envelope(body));
    BOOST_CHECK_EQUAL(mupnp_action_result_getcode(result), MUPNP_ACTION_RESULT_PARSE_ERROR);
    BOOST_CHECK_EQUAL(mupnp_dictionary_size(mupnp_action_result_getoutputs(result)), 0);
    mupnp_action_result_delete(result);
  }
}

BOOST_FIXTURE_TEST_CASE(ActionStrictInputsRejectBeforeIO, ActionFixture)
{
  auto inputs = mupnp_dictionary_new();
  for (const char* name : { "Output", "Unknown" }) {
    mupnp_dictionary_setvalue(inputs, name, "value");
    auto result = mupnp_action_postwitharguments(action, inputs);
    BOOST_REQUIRE(result);
    BOOST_CHECK_EQUAL(mupnp_action_result_getcode(result), MUPNP_ACTION_RESULT_INVALID_ARGUMENTS);
    BOOST_CHECK_EQUAL(mupnp_action_result_gethttpstatuscode(result), 0);
    mupnp_action_result_delete(result);
    mupnp_dictionary_clear(inputs);
  }
  auto result = mupnp_action_postwitharguments(action, NULL);
  BOOST_CHECK_EQUAL(mupnp_action_result_getcode(result), MUPNP_ACTION_RESULT_INVALID_ARGUMENTS);
  mupnp_action_result_delete(result);
  mupnp_dictionary_setvalue(inputs, "Input", "x");
  auto duplicate = mupnp_dictionary_element_new();
  mupnp_dictionary_element_setkey(duplicate, "Input");
  mupnp_dictionary_element_setvalue(duplicate, "duplicate");
  mupnp_dictionary_add(inputs, duplicate);
  result = mupnp_action_postwitharguments(action, inputs);
  BOOST_CHECK_EQUAL(mupnp_action_result_getcode(result), MUPNP_ACTION_RESULT_INVALID_ARGUMENTS);
  mupnp_action_result_delete(result);
  mupnp_dictionary_element_delete(duplicate);
  mupnp_xml_node_setchildnode(mupnp_argument_getargumentnode(mupnp_action_getarguments(action)), "direction", "invalid");
  result = mupnp_action_postwitharguments(action, inputs);
  BOOST_CHECK_EQUAL(mupnp_action_result_getcode(result), MUPNP_ACTION_RESULT_INVALID_ARGUMENTS);
  mupnp_action_result_delete(result);
  mupnp_dictionary_delete(inputs);
}

BOOST_FIXTURE_TEST_CASE(ActionEmptyDeclaredOutputs, ActionFixture)
{
  mupnp_argumentlist_clear(mupnp_action_getargumentlist(action));
  auto result = decode(envelope("<ReadResponse xmlns='urn:test:1'/>"));
  BOOST_REQUIRE(result);
  BOOST_CHECK_EQUAL(mupnp_action_result_getcode(result), MUPNP_ACTION_RESULT_SUCCESS);
  BOOST_CHECK_EQUAL(mupnp_dictionary_size(mupnp_action_result_getoutputs(result)), 0);
  mupnp_action_result_delete(result);
}
