/* BDB code to try out the select of join on multiple fields */

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

int sec_auto_color(DB *dbp, const DBT *key, const DBT *data, DBT *skey)
{
        automotive *sec_data;

        printf("\nInside sec_auto_color...\n");
        sec_data = data->data;

        printf("\nsec_data->auto_color = [%s] \n", sec_data->auto_color);
        memset(skey, 0, sizeof(DBT));
        skey->data = sec_data->auto_color;
        skey->size = strlen(sec_data->auto_color)+1;

        return 0;
}

int sec_auto_make(DB *dbp, const DBT *key, const DBT *data, DBT *skey)
{
        automotive *sec_data;

        printf("\nInside sec_auto_make...\n");
        sec_data = data->data;

        printf("\nsec_data->auto_make = [%s] \n", sec_data->auto_make);
        memset(skey, 0, sizeof(DBT));
        skey->data = sec_data->auto_make;
                skey->size = strlen(sec_data->auto_make)+1;

        return 0;
}

int sec_auto_type(DB *dbp, const DBT *key, const DBT *data, DBT *skey)
{
        automotive *sec_data;

        printf("\nInside sec_auto_typer...\n");
        sec_data = data->data;

        printf("\nsec_data->auto_type = [%s] \n", sec_data->auto_type);
        memset(skey, 0, sizeof(DBT));
        skey->data = sec_data->auto_type;
        skey->size = strlen(sec_data->auto_type)+1;

        return 0;
}

int main ()
{
DB_ENV *dbenv;
DB *automotiveDB;
DB *automotiveColorDB;
DB *automotiveMakeDB;
DB *automotiveTypeDB;
DBC *auto_curs, *color_curs, *make_curs, *type_curs, *join_curs;
DBC *carray[4];
DBT key, data;
int ret;
char *the_color = "Blue";
char *the_make = "Benz";
char *the_type = "SUV";
automotive auto_s, *auto_p;;

        if ((ret = db_env_create(&dbenv, 0)) != 0) {
                printf("\ndb_env_create error\n");
                return ret;
        } else { printf("\ndb_env_create done successfully...\n"); }

        if ((ret = dbenv->open(dbenv, NULL, DB_CREATE | DB_INIT_LOCK |
                DB_INIT_MPOOL | DB_INIT_TXN | DB_THREAD, 0644)) != 0) {
                printf("\ndbenv open error\n");
                return ret;
        } else { printf("\ndb_env_open done successfully...\n"); }

/*
 * Assume a primary database handle:
 *   automotiveDB
 * Assume 3 secondary database handles:
 *   automotiveColorDB  -- secondary database based on automobile color
 *   automotiveMakeDB  -- secondary database based on the manufacturer
 *   automotiveTypeDB  -- secondary database based on automobile type
 */

        if ((ret = db_create(&automotiveDB, dbenv, 0)) != 0) {
                printf("\ndb_create error\n");
                return ret;
        } else { printf("\ndb_create done successfully...\n"); }

        if ((ret = automotiveDB->open(automotiveDB, NULL, "automotiveDB.db", NULL, DB_BTREE,
                DB_AUTO_COMMIT | DB_CREATE | DB_THREAD, 0644)) != 0) {
                printf("\ndb open error\n");
                return ret;
        } else { printf("\ndb open done successfully...\n"); }


        if ((ret = db_create(&automotiveColorDB, dbenv, 0)) != 0) {
                printf("\ndb_create error\n");
                return ret;
        } else { printf("\ndb_create done successfully...\n"); }
        
        if ((ret = automotiveColorDB->open(automotiveColorDB, NULL, "automotiveColorDB.db", NULL, DB_BTREE,
                DB_AUTO_COMMIT | DB_CREATE | DB_THREAD, 0644)) != 0) {
                printf("\ndb open error\n");
                return ret;
        } else { printf("\ndb open done successfully...\n"); }

        if ((ret = db_create(&automotiveMakeDB, dbenv, 0)) != 0) {
                printf("\ndb_create error\n");
                return ret;
        } else { printf("\ndb_create done successfully...\n"); }

        if ((ret = automotiveMakeDB->open(automotiveMakeDB, NULL, "automotiveMakeDB.db", NULL, DB_BTREE,
                DB_AUTO_COMMIT | DB_CREATE | DB_THREAD, 0644)) != 0) {
                printf("\ndb open error\n");
                return ret;
        } else { printf("\ndb open done successfully...\n"); }

        if ((ret = db_create(&automotiveTypeDB, dbenv, 0)) != 0) {
                printf("\ndb_create error\n");
                return ret;
        } else { printf("\ndb_create done successfully...\n"); }

        if ((ret = automotiveTypeDB->open(automotiveTypeDB, NULL, "automotiveTypeDB.db", NULL, DB_BTREE,
                DB_AUTO_COMMIT | DB_CREATE | DB_THREAD, 0644)) != 0) {
                printf("\ndb open error\n");
                return ret;
        } else { printf("\ndb open done successfully...\n"); }


        if ((ret = automotiveDB->associate(automotiveDB, NULL, automotiveColorDB, sec_auto_color, 0)) != 0) {
                printf("\nAuto associate secondary Color db error [%d], [%s]\n", ret, db_strerror(ret));
                return ret;
        } else { printf("\nAuto associate secondary Color db successful...\n"); }

        if ((ret = automotiveDB->associate(automotiveDB, NULL, automotiveMakeDB, sec_auto_make, 0)) != 0) {
                        printf("\nAuto associate secondary Make db error [%d], [%s]\n", ret, db_strerror(ret));
                return ret;
        } else { printf("\nAuto associate secondary Make db successful...\n"); }

        if ((ret = automotiveDB->associate(automotiveDB, NULL, automotiveTypeDB, sec_auto_type, 0)) != 0) {
                printf("\nAuto associate secondary type db error [%d], [%s]\n", ret, db_strerror(ret));
                return ret;
        } else { printf("\nAuto associate secondary type db successful...\n"); }

        auto_s.auto_id = 101;
        strcpy(auto_s.auto_name, "Auto1");
        strcpy(auto_s.auto_color, "Blue");
        strcpy(auto_s.auto_make, "Benz");
        strcpy(auto_s.auto_type, "SUV");

        memset(&key, 0, sizeof(DBT));
        memset(&data, 0, sizeof(DBT));
        key.data = &auto_s.auto_id;
        key.size = sizeof(int);
        data.data = &auto_s;
        data.size = sizeof(automotive);
        data.flags = DB_DBT_USERMEM;
        if ((ret = automotiveDB->put(automotiveDB, NULL, &key, &data, DB_AUTO_COMMIT)) != 0) {
                printf("\nPut error [%d] [%s]\n", ret, db_strerror(ret));
                return ret;
        } else { printf("\nPut done successfully...\n"); }


        automotiveDB->cursor(automotiveDB, NULL, &auto_curs, 0);

        memset(&key, 0, sizeof(DBT));
        memset(&data, 0, sizeof(DBT));
        key.data = &auto_s.auto_id;
        key.size = sizeof(int);
        data.data = auto_p;
        data.size = sizeof(automotive);
                data.ulen = sizeof(automotive);
        data.flags = DB_DBT_USERMEM;

        while ((ret = auto_curs->get(auto_curs, &key, &data, DB_NEXT)) == 0) {
                printf("\nGet values [%d], [%s], [%s], [%s], [%s] \n", auto_p->auto_id, auto_p->auto_name,
                        auto_p->auto_color, auto_p->auto_make, auto_p->auto_type);
        }

/* initialize pointers and structures */
color_curs = NULL;
make_curs = NULL;
type_curs = NULL;
join_curs = NULL;
memset(&key, 0, sizeof(DBT));
memset(&data, 0, sizeof(DBT));
/* open the cursors */
if (( ret = automotiveColorDB->cursor(automotiveColorDB, NULL, &color_curs, 0)) != 0) {
               printf("\nColorDB cursor error\n");
                return ret;
        } else { printf("\nColorDB cursor done successfully...\n"); }

if (( ret = automotiveMakeDB->cursor(automotiveMakeDB, NULL, &make_curs, 0)) != 0) {
               printf("\nMakeDB cursor error\n");
                return ret;
        } else { printf("\nMakeDB cursor done successfully...\n"); }

if (( ret = automotiveTypeDB->cursor(automotiveTypeDB, NULL, &type_curs, 0)) != 0) {
               printf("\nTypeDB cursor error\n");
                return ret;
        } else { printf("\nTypeDB cursor done successfully...\n"); }

printf("\nValues are Color = [%s], Make = [%s], Type = [%s] \n",
                the_color, the_make, the_type);

/* Position the cursors */
key.data = the_color;
key.size = strlen(the_color) + 1;

printf("\nValue of key.data Color = [%s] \n", the_color);
if ((ret = color_curs->get(color_curs, &key, &data, DB_SET)) != 0) {
               printf("\nColor cursor get error - [%d], [%s]\n", ret, db_strerror(ret));
                return ret;
        } else { printf("\nColor cursor get done successfully...\n"); }

key.data = the_make;
key.size = strlen(the_make) + 1;
if ((ret = make_curs->get(make_curs, &key, &data, DB_SET)) != 0) {
               printf("\nMake cursor get error - [%d], [%s]\n", ret, db_strerror(ret));
                return ret;
        } else { printf("\nMake cursor get done successfully...\n"); }

key.data = the_type;
key.size = strlen(the_type) + 1;
if ((ret = type_curs->get(type_curs, &key, &data, DB_SET)) != 0) {
               printf("\nType cursor get error - [%d], [%s]\n", ret, db_strerror(ret));
                return ret;
        } else { printf("\nType cursor get done successfully...\n"); }

/* Set up the cursor array */
carray[0] = color_curs;
carray[1] = make_curs;
carray[2] = type_curs;
carray[3] = NULL;

/* Create the join */
if ((ret = automotiveDB->join(automotiveDB, carray, &join_curs, 0)) != 0) {
               printf("\nJoin error - - [%d], [%s]\n", ret, db_strerror(ret));
                return ret;
        } else { printf("\nJoin done successfully...\n"); }

        data.data = auto_p;
        data.size = sizeof(automotive);
                data.ulen = sizeof(automotive);
        data.flags = DB_DBT_USERMEM;


/* Iterate using the join cursor */
while ((ret = join_curs->get(join_curs, &key, &data, 0)) == 0) {
                printf("%.*s : %.*s\n",
                    (int)key.size, (char *)key.data,
                    (int)data.size, (char *)data.data);
                printf("\nGet values [%d], [%s], [%s], [%s], [%s] \n", auto_p->auto_id, auto_p->auto_name,
                        auto_p->auto_color, auto_p->auto_make, auto_p->auto_type);

}

/*
 * If we exited the loop because we ran out of records,
 * then it has completed successfully.
 */
if (ret == DB_NOTFOUND) {
    /*
     * Close all our cursors and databases as is appropriate,  and
     * then exit with a normal exit status (0).
     */
}


        join_curs->close(join_curs);
        type_curs->close(type_curs);
        make_curs->close(make_curs);
        color_curs->close(color_curs);
        auto_curs->close(auto_curs);
        automotiveTypeDB->close(automotiveTypeDB, 0);
        automotiveMakeDB->close(automotiveMakeDB, 0);
        automotiveColorDB->close(automotiveColorDB, 0);
        automotiveDB->close(automotiveDB, 0);
        
        return 0;
}
