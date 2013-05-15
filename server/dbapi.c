#include <stdio.h>
#include <mysql/mysql.h>
#include <string.h>
#include "dbapi.h"
#include "../common/debug.h"

#define DB_INIT()                                                                   \
                                                                                    \
        MYSQL *conn;;                                                               \
        conn = mysql_init(NULL);                                                    \
        mysql_real_connect(conn, "localhost", "root", "lee", "here", 0, NULL, 0);
        
int 
DBInsert (char *user_name, char *passwd)
{  
    DB_INIT();
    
    char sql[200]="insert into users (name, passwd) values ('";
    strcat(sql, user_name);
    strcat(sql, "', '");
    strcat(sql, passwd);
    strcat(sql, "');");
    
    mysql_query(conn, sql);
    
    mysql_close(conn);
    return 0;
}


int
DBCheckUserName(char *user_name)
{
    MYSQL_RES *result;
    MYSQL_ROW row;
    int num_fields;
    int i, res;
    
    DB_INIT();
    
    char sql[200]="SELECT * FROM users where name = '";
    strcat(sql, user_name);
    strcat(sql, "';");
    
    mysql_query(conn, sql);
    result = mysql_store_result(conn);

    num_fields = mysql_num_fields(result);

    if( mysql_fetch_row(result) == 0 )
        res = 0;
    else
        res = -1;
        
    mysql_free_result(result);
    mysql_close(conn);
    
    return res;
}

int
DBVerify(char *user_name, char *passwd)
{
    MYSQL_RES *result;
    MYSQL_ROW row;
    int num_fields;
    int i, j, res;
    
    DB_INIT();
    
    char sql[200]="SELECT * FROM users where name = '";
    strcat(sql, user_name);
    strcat(sql, "';");
    
    mysql_query(conn, sql);
    result = mysql_store_result(conn);

    num_fields = mysql_num_fields(result);

    row = mysql_fetch_row(result);
    
    if( row == NULL )
        return -1;
        
    if( strcmp(passwd, row[1]) == 0 )
        res = 0;
    else
        res = -1;
    
    mysql_free_result(result);
    mysql_close(conn);
    
    return res;
}
