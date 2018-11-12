/* BDB code to insert record into Foreign DB so that no Foreign constraint error comes in Primary DB
Related Code :
bdb_Insert_Put_ForeignConstrainedDB.c - Returns Success since foreign key is present in foreign DB.
bdb_Insert_Put_ForeignConstrainedDB_NoFK.c - Foreign key constraint error since No Foreign key is present in Foreign DB */
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

int main ()
{
DB_ENV *dbenv;
DB *automotiveDB;
DBC *auto_curs;
DBT key, data;
int ret;
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
        
        if ((ret = db_create(&automotiveDB, dbenv, 0)) != 0) {
                printf("\ndb_create error\n");
                return ret;
        } else { printf("\ndb_create done successfully...\n"); }

        if ((ret = automotiveDB->open(automotiveDB, NULL, "automotiveDB.db", NULL, DB_BTREE,
                DB_AUTO_COMMIT | DB_CREATE | DB_THREAD, 0644)) != 0) {
                printf("\ndb open error\n");
                return ret;
        } else { printf("\ndb open done successfully...\n"); }

        auto_s.auto_id = 101;
        strcpy(auto_s.auto_name, "Auto1");
        strcpy(auto_s.auto_color, "Black");
        strcpy(auto_s.auto_make, "BMW");
        strcpy(auto_s.auto_type, "Sedan");

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

        while ((ret = auto_curs->get(auto_curs, &key, &data, DB_NEXT)) == 0)
                printf("\nGet values [%d], [%s], [%s], [%s], [%s] \n", auto_p->auto_id, auto_p->auto_name,
                        auto_p->auto_color, auto_p->auto_make, auto_p->auto_type);

        auto_curs->close(auto_curs);
        automotiveDB->close(automotiveDB, 0);

        return 0;
}
