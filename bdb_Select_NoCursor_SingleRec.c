/* BDB code to insert and read from DB without cursor */

#include <db.h>
#include <stdio.h>
#include <string.h>

#define MAXFIELD 50

typedef struct vendor {
        int  sales_rep_id;;
        char sales_rep[MAXFIELD];
        int  sales_rep_phone;
} VENDOR;

int main()
{

        DB *dbp;
        DBT key, data;
        DBC *dbc;
        u_int32_t flags;
        int ret;
        VENDOR vendor_s, *vendor_get, *vendor_pget;
        /* Primary DB handle */
        /* Primary database open flags */
        /* Function return value */
        /* Primary */
        ret = db_create(&dbp, NULL, 0);
        if (ret != 0) {
                  /* Error handling goes here */
        }
        /* Database open flags */
        flags = DB_CREATE;    /* If the database does not exist,
                               * create it.*/
        /* open the primary database */
        ret = dbp->open(dbp,        /* DB structure pointer */
                NULL,       /* Transaction pointer */
                "my_db.db", /* On-disk file that holds the database. */
                NULL,       /* Optional logical database name */
                DB_BTREE,   /* Database access method */
                flags,      /* Open flags */
                0);         /* File mode (using defaults) */
                        if (ret != 0) {
        /* Error handling goes here */
        }

        memset(&key, 0, sizeof(DBT));
        memset(&data, 0, sizeof(DBT));

        vendor_s.sales_rep_id = 123;
        strcpy(vendor_s.sales_rep, "Smith");
        vendor_s.sales_rep_phone = 54321;

        /* Set up our DBTs */
        key.data = &vendor_s.sales_rep_id;
        key.size = sizeof(int);

        data.data = &vendor_s;
        data.size = sizeof(VENDOR);

        printf("\nPut data into db \n");
        dbp->put(dbp, NULL, &key, &data, DB_AUTO_COMMIT);

        vendor_get = data.data;
        printf("\nData after put load... [%d][%s][%d] \n",
                vendor_get->sales_rep_id, vendor_get->sales_rep, vendor_get->sales_rep_phone);



        /* Get the record using primary key sales_rep_id from primary db */
        memset(&key, 0, sizeof(DBT));
        memset(&data, 0, sizeof(DBT));

        vendor_s.sales_rep_id = 123;
        key.data = &vendor_s.sales_rep_id;
        key.size = sizeof(int);

        data.flags = DB_DBT_USERMEM;
        data.size = sizeof(VENDOR);
        data.ulen = sizeof(VENDOR);
        vendor_pget = &vendor_s;
        data.data = vendor_pget;

        ret = dbp->get(dbp, NULL, &key, &data, 0);
        printf("Return after get from primary db = [%d] [%s]\n", ret, db_strerror(ret));

        vendor_pget = data.data;
        printf("\nData after get from primary... [%d][%s][%d] \n",
                vendor_pget->sales_rep_id, vendor_pget->sales_rep, vendor_pget->sales_rep_phone);

        if (dbp != NULL)
                dbp->close(dbp, 0);

}
