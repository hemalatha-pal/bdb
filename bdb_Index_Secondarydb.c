/* BDB code to get the records from secondary db using secondary key */

#include <db.h>
#include <stdio.h>
#include <string.h>

#define MAXFIELD 50

typedef struct vendor {
        int  sales_rep_id;;
        char sales_rep[MAXFIELD];
        int  sales_rep_phone;
} VENDOR;

int
get_sales_rep(DB *sdbp,          /* secondary db handle */
              const DBT *pkey,   /* primary db record's key */
              const DBT *pdata,  /* primary db record's data */
              DBT *skey)         /* secondary db record's key */
{
    VENDOR *vendor;

    printf("\nInside get_sales_rep callback function..\n");
    /* First, extract the structure contained in the primary's data */
    vendor = pdata->data;
    /* Now set the secondary key's data to be the representative's name */
    memset(skey, 0, sizeof(DBT));
    skey->data = vendor->sales_rep;
    skey->size = strlen(vendor->sales_rep) + 1;
    /* Return 0 to indicate that the record can be created/updated. */
    return (0);
}

int main()
{

        DB *dbp, *sdbp;
        DBT key, data;
        DBC *dbc;
        u_int32_t flags;
        int ret;
        VENDOR vendor_s, *vendor_get, *vendor_pget, *vendor_sget;
        /* Primary and secondary DB handles */
        /* Primary database open flags */
        /* Function return value */
        /* Primary */
        ret = db_create(&dbp, NULL, 0);
        if (ret != 0) {
                  /* Error handling goes here */
        }
        /* Secondary */
        ret = db_create(&sdbp, NULL, 0);
        if (ret != 0) {
                /* Error handling goes here */
        }
        /* Usually we want to support duplicates for secondary databases */
        ret = sdbp->set_flags(sdbp, DB_DUPSORT);
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
        /* open the secondary database */
        ret = sdbp->open(sdbp,          /* DB structure pointer */
                 NULL,          /* Transaction pointer */
                 "my_secdb.db", /* On-disk file that holds the
                                                    database. */
                 NULL,          /* Optional logical database name */
                 DB_BTREE,      /* Database access method */
                 flags,         /* Open flags */
                 0);            /* File mode (using defaults) */
        if (ret != 0) {
        /* Error handling goes here */
        }
        /* Now associate the secondary to the primary */
        dbp->associate(dbp,            /* Primary database */
                NULL, /*TXNid*/
                sdbp, /* Secondary database */
                get_sales_rep, /* Callback used for key creation. Not
                                * defined in this example. See the next
                                * section. */
               0);              /* Flags */


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

        /* Get the record using secondary key sales_rep from secondary db */
        memset(&key, 0, sizeof(DBT));
        memset(&data, 0, sizeof(DBT));

        ret = sdbp->cursor(sdbp, NULL, &dbc, 0);
        printf("\nReturn after cursor set from secondary db = [%d] msg [%s]\n", ret, db_strerror(ret));

        strcpy(vendor_s.sales_rep, "Smith");
        key.data = vendor_s.sales_rep;
        key.size = (u_int32_t)strlen(vendor_s.sales_rep) + 1;

        ret = dbc->get(dbc, &key, &data, DB_SET);
        printf("\nReturn after cursor get from secondary db = [%d] msg [%s]\n", ret, db_strerror(ret));
        
        if ((ret == DB_NOTFOUND) || (data.data == NULL)) {
                printf("\nEither record not found or data is null\n");
                return ret;
        }

        vendor_sget = data.data;
        printf("\nData after get from secondary... [%d][%s][%d] \n", vendor_sget->sales_rep_id,
                        vendor_sget->sales_rep, vendor_sget->sales_rep_phone);

        dbc->close(dbc);
        if (sdbp != NULL)
                sdbp->close(sdbp, 0);

        if (dbp != NULL)
                dbp->close(dbp, 0);

}
