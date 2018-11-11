/* BDB code to insert single record into employee.db */

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

int main()
{
        DB_ENV *dbenv;
        int ret;
        DB *dbp;
        DBT key_dbt, data_dbt;
        emp_data edata;

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

        memset(&key_dbt, 0, sizeof(key_dbt));
        memset(&data_dbt, 0, sizeof(data_dbt));

        edata.emp_id = 101;
        strcpy(edata.emp_name, "Rajiv");
        edata.emp_sal = 5000.00;

        key_dbt.data = &edata.emp_id;
        key_dbt.size = sizeof(edata.emp_id);

        data_dbt.data = &edata;
        data_dbt.size = sizeof(edata);

        printf("\nData before put...\n");
        printf("\nEmp id [%d], name [%s], sal [%f]\n",
                        edata.emp_id, edata.emp_name, edata.emp_sal);

        if((ret = dbp->put(dbp, NULL, &key_dbt, &data_dbt,
                        DB_AUTO_COMMIT)) != 0) {
                printf("\ndb put error\n");
                return ret;
        }

        return 0;
}
