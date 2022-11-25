//
// Created by flo47663 on 22.11.2022.
//
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/client_subscriptions.h>
#include <stdlib.h>



UA_Boolean running = true;
static void stopHandler(int sig) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "received ctrl-c");
    running = false;
}
int main(void) {
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    UA_Client *client = UA_Client_new();
	
	
    /*****************************
    //Set url and config parameter
    ******************************/
    const char *url =          


    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    UA_StatusCode retval = UA_Client_connect(client, url);
    if(retval != UA_STATUSCODE_GOOD) {
        UA_Client_delete(client);
        return (int)retval;
    }

    //UA_NodeId current_temp_nodeId = UA_NODEID_NUMERIC(1, 1000);
    //UA_NodeId desired_temp_nodeId = UA_NODEID_NUMERIC(1, 1002);




    /***************
     * read value
     *************/
    

    /***************
     * write value
     *************/
    

    /******************
     * execute method
     ***************/

    //UA_NodeId heater_object_nodeId = UA_NODEID_NUMERIC(1, 1001);
    //UA_NodeId set_temp_method_nodeId = UA_NODEID_NUMERIC(1, 62541);
    


    /****************
     * create subscription
     ******************/




    while(running)
        UA_Client_run_iterate(client, 100);


}