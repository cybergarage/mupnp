/* Shared bounded traversal for the C media-directory examples. */
#ifndef _MUPNP_EXAMPLE_CONTENT_DIRECTORY_H_
#define _MUPNP_EXAMPLE_CONTENT_DIRECTORY_H_

#include <mupnp/upnp.h>

typedef struct _mUpnpExampleBrowseBudget {
  mUpnpDictionary* visited;
  size_t requests;
} mUpnpExampleBrowseBudget;

static void mupnp_example_browse_directory(mUpnpAction* action, int depth, const char* objectId, mUpnpExampleBrowseBudget* budget);

static void mupnp_example_print_children(mUpnpAction* action, int depth, mUpnpXmlNode* directory, mUpnpExampleBrowseBudget* budget)
{
  for (mUpnpXmlNode* node = mupnp_xml_node_getchildnodes(directory); node; node = mupnp_xml_node_next(node)) {
    const char* id = mupnp_xml_node_getattributevalue(node, "id");
    const char* title = mupnp_xml_node_getchildnodevalue(node, "dc:title");
    const char* label = title ? title : "";
    if (mupnp_xml_node_isname(node, "container")) {
      printf(" %*s[%s]%s\n", depth, "", id ? id : "", label);
      mupnp_example_browse_directory(action, depth + 1, id, budget);
    }
    else {
      const char* url = mupnp_xml_node_getchildnodevalue(node, "res");
      printf(" %*s[%s]%s (%s)\n", depth, "", id ? id : "", label, url ? url : "");
    }
  }
}

static void mupnp_example_browse_directory(mUpnpAction* action, int depth, const char* objectId, mUpnpExampleBrowseBudget* budget)
{
  if (!objectId || depth < 0 || depth >= 64 || budget->requests >= 1024 || mupnp_dictionary_getvalue(budget->visited, objectId))
    return;
  /* The dictionary copies IDs, which otherwise belong to temporary XML trees. */
  mupnp_dictionary_setvalue(budget->visited, objectId, "visited");
  budget->requests++;

  mupnp_action_setargumentvaluebyname(action, "ObjectID", objectId);
  mupnp_action_setargumentvaluebyname(action, "BrowseFlag", "BrowseDirectChildren");
  mupnp_action_setargumentvaluebyname(action, "Filter", "*");
  mupnp_action_setargumentvaluebyname(action, "StartingIndex", "0");
  mupnp_action_setargumentvaluebyname(action, "RequestedCount", "0");
  mupnp_action_setargumentvaluebyname(action, "SortCriteria", "");
  if (!mupnp_action_post(action))
    return;

  const char* result = mupnp_action_getargumentvaluebyname(action, "Result");
  if (mupnp_strlen(result) == 0)
    return;
  mUpnpXmlNodeList* nodes = mupnp_xml_nodelist_new();
  mUpnpXmlParser* parser = mupnp_xml_parser_new();
  if (nodes && parser && mupnp_xml_parse(parser, nodes, result, mupnp_strlen(result))) {
    mUpnpXmlNode* directory = mupnp_xml_nodelist_getbyname(nodes, "DIDL-Lite");
    if (directory)
      mupnp_example_print_children(action, depth, directory, budget);
  }
  if (nodes)
    mupnp_xml_nodelist_delete(nodes);
  if (parser)
    mupnp_xml_parser_delete(parser);
}

static void mupnp_example_print_content_directory(mUpnpAction* action, int depth, const char* objectId)
{
  mUpnpExampleBrowseBudget budget;
  budget.visited = mupnp_dictionary_new();
  budget.requests = 0;
  if (!budget.visited)
    return;
  mupnp_example_browse_directory(action, depth, objectId, &budget);
  mupnp_dictionary_delete(budget.visited);
}

#endif
