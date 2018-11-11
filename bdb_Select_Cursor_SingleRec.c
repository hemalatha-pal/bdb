/* BDB code to select the values from db using cursor */

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<db.h>
        
typedef struct _emp_data {
        int emp_id;
        char emp_name[10];
        float emp_sal;
} emp_data;
        
typedef int emp_key; 

int main()
{       
        DB_ENV *dbenv;
        int ret;
        DB *dbp;
        DBT key_dbt, data_dbt;
        DBC *dbc;
        emp_data *edata, edata_s;       
        edata = &edata_s;
        emp_key ekey;
                
        if ((ret = db_env_create(&dbenv, 0)) != 0) {
                printf("\ndb_env_create error\n");
                return ret;
        } else { printf("\ndb_env_create done successfully...\n"); }

        if ((ret = dbenv->open(dbenv, NULL, DB_CREATE | DB_INIT_LOCK | 
                DB_INIT_MPOOL | DB_INIT_TXN | DB_THREAD, 0644)) != 0) {
                printf("\ndbenv open error\n");
                return ret;
        } else { printf("\ndb_env_open done successfully...\n"); }

        if ((ret = db_create(&dbp, dbenv, 0)) != 0) {
                printf("\ndb_create error\n");
                return ret;
        } else { printf("\ndb_create done successfully...\n"); }
        
        if ((ret = dbp->open(dbp, NULL, "employee.db", NULL, DB_BTREE,
                DB_AUTO_COMMIT | DB_CREATE | DB_THREAD, 0644)) != 0) {
                printf("\ndb open error\n");
                return ret;
        } else { printf("\ndb open done successfully...\n"); }

        dbp->cursor(dbp, NULL, &dbc, 0);
        memset(&key_dbt, 0, sizeof(key_dbt));
        memset(&data_dbt, 0, sizeof(data_dbt));
        //data_dbt.flags = DB_DBT_USERMEM;

        for (ret = dbc->get(dbc, &key_dbt, &data_dbt, DB_FIRST);
                ret == 0;
                ret = dbc->get(dbc, &key_dbt, &data_dbt, DB_NEXT )) {

                edata = data_dbt.data;
                printf("\nedata->emp_id = %d, edata->emp_name = %s, edata->emp_sal = %f\n",
                        edata->emp_id, edata->emp_name, edata->emp_sal);
        }

        printf("\nExiting after cursor get loop...\n");

        return 0;
}
