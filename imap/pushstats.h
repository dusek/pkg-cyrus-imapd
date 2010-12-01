
/* pushstats.h -- statistics push interface

 * generated automatically from pushstats.snmp by snmpgen

 *

 * Copyright 2000 Carnegie Mellon University

 *

 * No warranty, yadda yadda

 */                                       

                                          

#ifndef pushstats_H    

#define pushstats_H



#define SNMPDEFINE_cmutree "1.3.6.1.4.1.3.2.2.3"
#define SNMPDEFINE_cmuimap "1.3.6.1.4.1.3.2.2.3.1"



#ifndef USING_SNMPGEN



#define snmp_connect()

#define snmp_close()

#define snmp_increment(a, b)

#define snmp_increment_args(a, b, c, d, e)

#define snmp_set(a, b)

#define snmp_set_str(a, b)

#define snmp_set_oid(a, b)

#define snmp_set_time(a, b)

#define snmp_getdescription(a)

#define snmp_getoid(a, b, c, d)

#define snmp_setvariable(a, b)



typedef void pushstats_t;



#else



typedef enum {

    MYRIGHTS_COUNT,
    BBOARD_COUNT,
    SCAN_COUNT,
    LSUB_COUNT,
    AUTHENTICATION_YES,
    GETQUOTAROOT_COUNT,
    STATUS_COUNT,
    FETCH_COUNT,
    STARTTLS_COUNT,
    LOGIN_COUNT,
    UNSUBSCRIBE_COUNT,
    GETANNOTATION_COUNT,
    SERVER_NAME_VERSION,
    LISTRIGHTS_COUNT,
    AUTHENTICATION_NO,
    SORT_COUNT,
    EXAMINE_COUNT,
    EXPUNGE_COUNT,
    SERVER_UPTIME,
    CHECK_COUNT,
    CAPABILITY_COUNT,
    RENAME_COUNT,
    LIST_COUNT,
    ID_COUNT,
    GETQUOTA_COUNT,
    IDLE_COUNT,
    COMPRESS_COUNT,
    DELETE_COUNT,
    SETANNOTATION_COUNT,
    GETACL_COUNT,
    SELECT_COUNT,
    UNSELECT_COUNT,
    SETACL_COUNT,
    CLOSE_COUNT,
    NOOP_COUNT,
    GETUIDS_COUNT,
    SEARCH_COUNT,
    NAMESPACE_COUNT,
    THREAD_COUNT,
    SETQUOTA_COUNT,
    LOGOUT_COUNT,
    CREATE_COUNT,
    SUBSCRIBE_COUNT,
    TOTAL_CONNECTIONS,
    ACTIVE_CONNECTIONS,
    DELETEACL_COUNT,
    STORE_COUNT,
    FIND_COUNT,
    AUTHENTICATE_COUNT,
    PARTIAL_COUNT,
    APPEND_COUNT,
    COPY_COUNT
} pushstats_t;



typedef enum {

    VARIABLE_LISTEND,
    VARIABLE_AUTH


} pushstats_variable_t;



int snmp_connect(void);



int snmp_close(void);          

                                    

/* only valid on counters */

int snmp_increment(pushstats_t cmd, int);

int snmp_increment_args(pushstats_t cmd, int incr, ...);



/* only valid on values */

int snmp_set(pushstats_t cmd, int);



int snmp_set_str(pushstats_t cmd, char *value);



int snmp_set_oid(pushstats_t cmd, char *str);



int snmp_set_time(pushstats_t cmd, time_t t);

                                    

const char *snmp_getdescription(pushstats_t cmd); 

 

const char *snmp_getoid(const char *name, pushstats_t cmd, char* buf, int buflen); 



void snmp_setvariable(pushstats_variable_t, int);



#endif /* USING_SNMPGEN */

 

#endif /* pushstats_H */ 



