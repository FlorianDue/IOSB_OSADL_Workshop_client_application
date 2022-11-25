//
// Created by flo47663 on 25.11.2022.
//
#include <open62541/plugin/log_stdout.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>

#include <signal.h>
#include <stdlib.h>

static void define_temperature_sensor_type(UA_Server *server, const char* name){

    UA_NodeId sensor_id = UA_NODEID_NUMERIC(1, 1001);
    UA_ObjectAttributes oAttr = UA_ObjectAttributes_default;
    oAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Heater Controller");
    UA_Server_addObjectNode(server, UA_NODEID_NUMERIC(1, 1001),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                            UA_QUALIFIEDNAME(1, "Heater Controller"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
                            oAttr, NULL, NULL);

    UA_VariableAttributes mnAttr = UA_VariableAttributes_default;
    UA_Double curr_temp = (UA_Double) 15;
    UA_Variant_setScalar(&mnAttr.value, &curr_temp, &UA_TYPES[UA_TYPES_DOUBLE]);
    mnAttr.displayName = UA_LOCALIZEDTEXT("en-US", "current temperature");
    mnAttr.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;
    mnAttr.userAccessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    mnAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1,1000), sensor_id,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "current temperature"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), mnAttr, NULL, NULL);


    UA_VariableAttributes mAttr = UA_VariableAttributes_default;
    UA_Double des_temp = (UA_Double) 20;
    UA_Variant_setScalar(&mAttr.value, &des_temp, &UA_TYPES[UA_TYPES_DOUBLE]);
    mAttr.displayName = UA_LOCALIZEDTEXT("en-US", "desired temperature");
    mAttr.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;
    mAttr.userAccessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    mAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    UA_Server_addVariableNode(server, UA_NODEID_NUMERIC(1,1002), sensor_id,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "desired temperature"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), mAttr, NULL, NULL);



}


static UA_StatusCode
set_temperature_callback(UA_Server *server,
                         const UA_NodeId *sessionId, void *sessionHandle,
                         const UA_NodeId *methodId, void *methodContext,
                         const UA_NodeId *objectId, void *objectContext,
                         size_t inputSize, const UA_Variant *input,
                         size_t outputSize, UA_Variant *output) {
    UA_Double *new_temp = (UA_Double*)input->data;
    UA_Variant *set_temp = UA_Variant_new();
    UA_Variant_setScalar(set_temp, new_temp, &UA_TYPES[UA_TYPES_DOUBLE]);
    UA_StatusCode ret_val = UA_Server_writeValue(server, UA_NODEID_NUMERIC(1,1002), *set_temp);
    if(ret_val == UA_STATUSCODE_GOOD){
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "succeded to set temp: \n");
    }
    else{
        UA_LOG_WARNING(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Failed to set temp: \n");
    }


    UA_Server_writeValue(server, UA_NODEID_NUMERIC(1, 1000), *set_temp);

    UA_String out = UA_STRING_NULL;
    UA_print(set_temp, &UA_TYPES[UA_TYPES_VARIANT], &out);
    printf("the value written to the current temp variable: %.*s\n", (int)out.length, out.data);
    UA_String_clear(&out);


    return UA_STATUSCODE_GOOD;
}

static void
add_set_temperature_method(UA_Server *server) {
    UA_Argument inputArgument;
    UA_Argument_init(&inputArgument);
    inputArgument.description = UA_LOCALIZEDTEXT("en-US", "The new desired temperature value");
    inputArgument.name = UA_STRING("new value");
    inputArgument.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;
    inputArgument.valueRank = UA_VALUERANK_SCALAR;


    UA_MethodAttributes helloAttr = UA_MethodAttributes_default;
    helloAttr.displayName = UA_LOCALIZEDTEXT("en-US","Set Temperature");
    helloAttr.executable = true;
    helloAttr.userExecutable = true;
    UA_Server_addMethodNode(server, UA_NODEID_NUMERIC(1,62541),
                            UA_NODEID_NUMERIC(1, 1001),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                            UA_QUALIFIEDNAME(1, "set temperature"),
                            helloAttr, &set_temperature_callback,
                            1, &inputArgument, 0, NULL, NULL, NULL);
}



static volatile UA_Boolean running = true;
static void stopHandler(int sig) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "received ctrl-c");
    running = false;
}

int main(void) {
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    UA_Server *server = UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));

    define_temperature_sensor_type(server, "Heater_Room_1");
    add_set_temperature_method(server);


    UA_StatusCode retval = UA_Server_run(server, &running);

    UA_Server_delete(server);
    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
}


