/* BDB code to show that foreign key is present and so primary db insert goes through fine
Pre-requisite : Insert foreign key record using bdb_Insert_Put_ForeignDB.c file
*/

#include <db.h>
#include <string.h>
#define MAX 10
typedef struct _automotive {
        int auto_id;
        char auto_name[MAX];
        char auto_color[MAX];
        char auto_make[MAX];
        char auto_type[MAX];
} automotive;

typedef struct _autoinv {
        int order_id;
        int auto_id;
        char cust_name[MAX];
        int qty;
} autoinv;

/*
* getautoid -- extracts a secondary key (the auto id) from a primary
*     key/data pair
*/

int
getautoid(secondary, pkey, pdata, skey)
       DB *secondary;
       const DBT *pkey, *pdata;
       DBT *skey;
{
        printf("\bInside getautoid...\n");
        autoinv *ainv;

    ainv = pdata->data;
    /* Now set the secondary key's data to be the representative's name */
    memset(skey, 0, sizeof(DBT));
        printf("\nAssigning skey->data with ainv_auto_id..\n");
    skey->data = &ainv->auto_id;
    skey->size = sizeof(ainv->auto_id);
        printf("\nSize of auto_id = [%d] \n", skey->size);

    /* Return 0 to indicate that the record can be created/updated. */
        return 0;

}

int
getcustname(secondary, pkey, pdata, skey)
       DB *secondary;
       const DBT *pkey, *pdata;
       DBT *skey;
{
        printf("\bInside getcustname...\n");
        autoinv *ainv;

    ainv = pdata->data;
    /* Now set the secondary key's data to be the representative's name */
    memset(skey, 0, sizeof(DBT));
        printf("\nAssigning skey->data with ainv_cust_name..\n");
    skey->data = ainv->cust_name;
    skey->size = sizeof(ainv->cust_name);
        printf("\nSize of cust_name = [%d] \n", skey->size);

    /* Return 0 to indicate that the record can be created/updated. */
        return 0;

}

int main ()
{
DB_ENV *dbenv;
DB *automotiveDB, *dbp, *sdbp, *cdbp;
DBC *ainv_curs, *auto_curs;
DBT key, data, akey, adata;
int ret, flags;
autoinv ainv_s, *ainv_p;
automotive auto_s, *auto_p;

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
                printf("\ndb_create orders.db error\n");
                return ret;
        } else { printf("\ndb_create orders.db done successfully...\n"); }

       if ((ret = dbp->open(dbp, NULL,
           "orders.db", NULL, DB_BTREE, DB_CREATE, 0600)) != 0) {
                printf("\ndb open orders.db error\n");
                return ret;
        } else { printf("\ndb open orders.db done successfully...\n"); }

       if ((ret = db_create(&sdbp, dbenv, 0)) != 0) {
                printf("\ndb_create orders_auto_ids.db  error\n");
                return ret;
        } else { printf("\ndb_create orders_auto_ids.db done successfully...\n"); }
        
       if ((ret = sdbp->set_flags(sdbp, DB_DUP | DB_DUPSORT)) != 0) {
                printf("\nsdbp set flags error - [%d], [%s] \n", ret, db_strerror(ret));
                return ret;
        } else { printf("\nsdbp set flags done successfully...\n"); }

       if ((ret = sdbp->open(sdbp, NULL, "orders_auto_ids.db",
           NULL, DB_BTREE, DB_CREATE, 0600)) != 0) {
                printf("\ndb open orders_auto_ids.db error\n");
                return ret;
        } else { printf("\ndb open orders_auto_ids.db done successfully...\n"); }

       if ((ret = dbp->associate(dbp, NULL, sdbp, getautoid, 0)) != 0) {
                printf("\ndbp associate error - [%d], [%s] \n", ret, db_strerror(ret));
                return ret;
        } else { printf("\ndb associate done successfully...\n"); }


       if ((ret = db_create(&cdbp, dbenv, 0)) != 0) {
                printf("\ndb_create orders_cust_names.db  error\n");
                return ret;
        } else { printf("\ndb_create orders_cust_names.db done successfully...\n"); }

       if ((ret = cdbp->set_flags(cdbp, DB_DUP | DB_DUPSORT)) != 0) {
                printf("\nsdbp set flags error - [%d], [%s] \n", ret, db_strerror(ret));
                return ret;
        } else { printf("\nsdbp set flags done successfully...\n"); }

       if ((ret = cdbp->open(cdbp, NULL, "orders_cust_names.db",
           NULL, DB_BTREE, DB_CREATE, 0600)) != 0) {
                printf("\ndb open orders_cust_names.db error\n");
                return ret;
        } else { printf("\ndb open orders_cust_names.db done successfully...\n"); }

       if ((ret = dbp->associate(dbp, NULL, cdbp, getcustname, 0)) != 0) {
                       printf("\ndbp associate error - [%d], [%s] \n", ret, db_strerror(ret));
                return ret;
        } else { printf("\ndb associate done successfully...\n"); }

        if ((ret = db_create(&automotiveDB, dbenv, 0)) != 0) {
                printf("\ndb_create error\n");
                return ret;
        } else { printf("\ndb_create done successfully...\n"); }

        if ((ret = automotiveDB->open(automotiveDB, NULL, "automotiveDB.db", NULL, DB_BTREE,
                DB_AUTO_COMMIT | DB_CREATE | DB_THREAD, 0644)) != 0) {
                printf("\ndb open error\n");
                return ret;
        } else { printf("\ndb open done successfully...\n"); }

        /* Associate the foreign with the secondary. */
        if ((ret = automotiveDB->associate_foreign(
           automotiveDB, sdbp, NULL, DB_FOREIGN_CASCADE)) != 0) {
                printf("\nAssociate foreign error - [%d] , [%s] \n", ret, db_strerror(ret));
                return ret;
        } else { printf("\nAssociate foreign done successfully...\n"); }

        ainv_s.order_id = 1001;
        ainv_s.auto_id = 101;
        strcpy(ainv_s.cust_name, "Jack");
        ainv_s.qty = 2;

        memset(&key, 0, sizeof(DBT));
        memset(&data, 0, sizeof(DBT));
        key.data = &ainv_s.order_id;
        key.size = sizeof(int);
        data.data = &ainv_s;
        data.size = sizeof(autoinv);
        data.ulen = sizeof(autoinv);
        data.flags = DB_DBT_USERMEM;
                if ((ret = dbp->put(dbp, NULL, &key, &data, DB_AUTO_COMMIT)) != 0) {
                printf("\nPut error [%d] [%s]\n", ret, db_strerror(ret));
                return ret;
        } else { printf("\nPut done successfully...\n"); }

        sdbp->cursor(sdbp, NULL, &ainv_curs, 0);

        memset(&key, 0, sizeof(DBT));
        memset(&data, 0, sizeof(DBT));
        key.data = &ainv_s.auto_id;
        key.size = sizeof(ainv_s.auto_id);
        data.data = ainv_p;
        data.size = sizeof(autoinv);
        data.ulen = sizeof(autoinv);
        data.flags = DB_DBT_USERMEM;

        printf("\nValue of auto id passed for get = [%d]\n", ainv_s.auto_id);
        flags = DB_SET;
        while ((ret = ainv_curs->get(ainv_curs, &key, &data, flags)) == 0) {
                printf("\nReturn of ainv curs = [%d] , [%s] \n", ret, db_strerror(ret));
                printf("\nGet values [%d], [%d], [%s], [%d] \n",
                        ainv_p->order_id, ainv_p->auto_id, ainv_p->cust_name, ainv_p->qty);

                flags = DB_NEXT_DUP;
        }

        automotiveDB->cursor(automotiveDB, NULL, &auto_curs, 0);
        memset(&akey, 0, sizeof(DBT));
        memset(&adata, 0, sizeof(DBT));
        auto_s.auto_id = 101;
        akey.data = &auto_s.auto_id;
        akey.size = sizeof(int);
        adata.data = &auto_s;
        adata.size = sizeof(automotive);
        adata.ulen = sizeof(automotive);
        adata.flags = DB_DBT_USERMEM;

        printf("\nValue of auto id passed for get to auto_curs = [%d]\n", auto_s.auto_id);
        flags = DB_NEXT;
        while ((ret = auto_curs->get(auto_curs, &akey, &adata, flags)) == 0) {
                auto_p = adata.data;
                printf("\nReturn of auto curs = [%d] , [%s] \n", ret, db_strerror(ret));
                printf("\nGet values [%d], [%s], [%s], [%s] [%s] \n", auto_p->auto_id,
                        auto_p->auto_name, auto_p->auto_color, auto_p->auto_make, auto_p->auto_type);
        }

        ainv_curs->close(ainv_curs);
        auto_curs->close(auto_curs);
        dbp->close(dbp, 0);
        sdbp->close(sdbp, 0);
        cdbp->close(cdbp, 0);
        automotiveDB->close(automotiveDB, 0);

        return 0;
}
