#ifndef DBAPI_H
#define DBAPI_H

int 
DBInsert( char *user_name, char *passwd );

int
DBCheckUserName(char *user_name);

int
DBVerify(char *user_name, char *passwd);

#endif
