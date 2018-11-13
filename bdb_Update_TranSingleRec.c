/* BDB code to show the transaction commit and abort. String "thedata" is modified to "test string" */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"
int
main(void)
{
    DBT key, data;
    DBC *cursorp;
    DB_TXN *txn = NULL;
    int ret, c_ret, ret_c;
    char *replacementString = "test string";
    DB *dbp;
    DB_ENV *envp;
    const char *db_home_dir = "./";
    const char *file_name = "txndb.db";
    char *keystr ="testkey";
    char *datastr = "thedata";
    u_int32_t db_flags, env_flags;

    /* environment and db handle creation omitted */
    dbp = NULL;
    envp = NULL;
    /* Open the environment */
    ret = db_env_create(&envp, 0);
    if (ret != 0) {
        fprintf(stderr, "Error creating environment handle: %s\n",
            db_strerror(ret));
        return (EXIT_FAILURE);
    }

     if ((ret = envp->open(envp, NULL, DB_CREATE | DB_INIT_LOCK |
                DB_INIT_MPOOL | DB_INIT_TXN | DB_THREAD, 0644)) != 0) {
                printf("\ndbenv open error\n");
                return ret;
        } else { printf("\ndb_env_open done successfully...\n"); }
        

/* Initialize the DB handle */
ret = db_create(&dbp, envp, 0);
if (ret != 0) {
    envp->err(envp, ret, "Database creation failed");
goto err; }
else { printf("\ndb create done successfully...\n"); }

db_flags = DB_CREATE | DB_AUTO_COMMIT;

/*
 * Open the database. Note that we are using auto commit for the open,
 * so the database is able to support transactions.
 */
ret = dbp->open(dbp,        /* Pointer to the database */
                NULL,       /* Txn pointer */
                file_name,  /* File name */
                NULL,       /* Logical db name */
                DB_BTREE,   /* Database type (using btree) */
                db_flags,   /* Open flags */
                0);         /* File mode. Using defaults */
if (ret != 0) {
    envp->err(envp, ret, "Database '%s' open failed",
        file_name);
    goto err;
}
else { printf("\ndb open done successfully...\n"); }

/* Prepare the DBTs */
memset(&key, 0, sizeof(DBT));
memset(&data, 0, sizeof(DBT));
key.data = keystr;
key.size = strlen(keystr) + 1;
data.data = datastr;
data.size = strlen(datastr) + 1;
/*
 * Perform the database write. A txn handle is not provided, but the
 * database support auto commit, so auto commit is used for the write.
 */
printf("\nBefore db put values are...key.data = [%s], data.data = [%s]\n", key.data, data.data);
ret = dbp->put(dbp, NULL, &key, &data, 0);
if (ret != 0) {
    envp->err(envp, ret, "Database put failed.");
goto err; }
else { printf("\ndb put done successfully...key.data = [%s], data.data = [%s]\n", key.data, data.data); }


    /* Get the txn handle */
    txn = NULL;
    ret = envp->txn_begin(envp, NULL, &txn, 0);
    if (ret != 0) {
        envp->err(envp, ret, "Transaction begin failed.");
        goto err;
    }
    else { printf("\ntxn begine done successfully...\n"); }

    /* Get the cursor, supply the txn handle at that time */
    ret = dbp->cursor(dbp, txn, &cursorp, 0);
    if (ret != 0) {
        envp->err(envp, ret, "Cursor open failed.");
        txn->abort(txn);
        goto err;
    }
    else { printf("\ndb cursor done successfully...\n"); }
    /*
     * Now use the cursor. Note that we do not supply any txn handles to
     * these methods.
     */
    /* Prepare the DBTs */
    memset(&key, 0, sizeof(DBT));
    memset(&data, 0, sizeof(DBT));
    
    ret = 1;
    while (cursorp->get(cursorp, &key, &data, DB_NEXT) == 0) {
        printf("\nInside while loop..[%d] \n", ret++);
        printf("\nkey.data = [%s], data.data = [%s] \n", key.data, data.data);
        data.data = (void *)replacementString;
        data.size = (strlen(replacementString) + 1) * sizeof(char);
        printf("\nNow trying to replace using put...\n");
        c_ret = cursorp->put(cursorp, &key, &data, DB_CURRENT);
        if (c_ret != 0) {
            printf("\nCursor put failed with ret [%d] and msg [%s]\n", c_ret, db_strerror(c_ret));
            /* abort the transaction and goto error */
            envp->err(envp, ret, "Cursor put failed.");
            cursorp->close(cursorp);
            cursorp = NULL;
            txn->abort(txn);
            goto err;
        } else { printf("\nCursor put is successful..key.data=[%s], data.data=[%s]\n", key.data, data.data); }

    }
    /*
     * Commit the transaction. Note that the transaction handle
     * can no longer be used.
     */
    ret = cursorp->close(cursorp);
    if (ret != 0) {
        envp->err(envp, ret, "Cursor close failed.");
        txn->abort(txn);
        goto err;
    }
    ret = txn->commit(txn, 0);
    if (ret != 0) {
        envp->err(envp, ret, "Transaction commit failed.");
        goto err;
    }
err:
/* Close the cursor (if the handle is not NULL)
     * and perform whatever other cleanup is required */
    /* Close the database */
    /* Close the environment */

  /* Close the cursor
    if (cursorp != NULL) {
        ret_c = cursorp->close(cursorp);
        if (ret_c != 0) {
            envp->err(envp, ret_c, "Cursor close failed.");
        ret = ret_c; }
} */

  /* Close the database */
    if (dbp != NULL) {
        ret_c = dbp->close(dbp, 0);
        if (ret_c != 0) {
            envp->err(envp, ret_c, "Database close failed.");
ret = ret_c; }
}
    /* Close the environment */
    if (envp != NULL) {
        ret_c = envp->close(envp, 0);
        if (ret_c != 0) {
            fprintf(stderr, "environment close failed: %s\n", db_strerror(ret_c));
ret = ret_c; }
}

/*    if (c_ret != 0)
        ret = c_ret;
    return (ret == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
*/

}
