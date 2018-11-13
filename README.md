This repository is having the BDB samples taken from BDB documentation PDFs downloaded from the below link.
https://docs.oracle.com/database/bdb181/books.htm

After BDB installation, the example files uploaded in the BDB repository can be tried to understand features of BDB.

BDB Installation :
==================
https://www.oracle.com/technetwork/database/database-technologies/berkeleydb/downloads/index.html

cd $downloaded_directory

cd db-18.1.25/build_unix

../dist/configure

make

make install


BDB Code Compilation and Linking:
=================================
Compilation of these example files can be done as given below:
Example :
$HOME/bdb/build/db-18.1.25/examples/c/getting_started

gcc -o ex_read gettingstarted_common.c example_database_read.c -I$DB_INSTALL/build_unix $DB_INSTALL/build_unix/libdb.a -pthread -lssl -lcrypto


gcc -o ex_load gettingstarted_common.c example_database_load.c -I$DB_INSTALL/build_unix $DB_INSTALL/build_unix/libdb.a -pthread -lssl -lcrypto







