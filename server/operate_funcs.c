#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include "operate_funcs.h"
#include "dbapi.h"
#include "../common/debug.h"

#define USER_NAME_LENGTH 50
#define USER_PASSWD_LENGTH 20

#define DEBUG_MODE

int 
LoginRsp( int sockfd, int paras_num, parameter *paras_req )
{
    char user_name[USER_NAME_LENGTH];
    char user_passwd[USER_PASSWD_LENGTH];
    char send_buf[BUFSIZE];
    int result, user_id, temp, package_size, res;
    parameter *paras_rsp;

    memset(user_name, 0, sizeof(user_name));
    memset(user_passwd, 0, sizeof(user_passwd)); 
    
    memcpy(user_name, paras_req[0].data, paras_req[0].len);
    memcpy(user_passwd, paras_req[1].data, paras_req[1].len);

    paras_rsp = (parameter *)malloc( sizeof(parameter)*2 );    
    if( paras_rsp == NULL )
    {
        fprintf(stderr,"malloc error! %s:%d\n",__FILE__,__LINE__);
        return -1;
    }

    //check user identity
    printf("before verify!\n");
    if( DBVerify( user_name, user_passwd ) == 0 )
        result = 1;
    else 
        result = 0;
    
    user_id = 0;

    temp = htonl(result);
    memcpy( paras_rsp[0].data,
            &temp,
            sizeof(int) );
    paras_rsp[0].len = sizeof(int);
    
    temp = htonl(user_id);
    memcpy( paras_rsp[1].data,
            &temp,
            sizeof(int) );
    paras_rsp[1].len = sizeof(int);
    
    package( LOGIN_RSP,
             2,
             paras_rsp,
             &package_size,
             send_buf );
    
    res = send( sockfd,
                send_buf,
                package_size,
                0 );
                
    if( res < 0 )
    {
        fprintf(stderr,
                "send error! %s:%d\n",
                __FILE__,
                __LINE__);
        return -1;
    }             
       
    return 0;
}

int 
RegisterRsp( int sockfd, int paras_num, parameter *paras_req )
{
    char user_name[USER_NAME_LENGTH];
    char user_passwd[USER_PASSWD_LENGTH];
    char send_buf[BUFSIZE];
    int result, user_id, temp, package_size, res;
    parameter *paras_rsp;
    
    memset(user_name, 0, sizeof(user_name));
    memset(user_passwd, 0, sizeof(user_passwd));   
    
    memcpy(user_name, paras_req[0].data, paras_req[0].len);
    memcpy(user_passwd, paras_req[1].data, paras_req[1].len);
  
    paras_rsp = (parameter *)malloc( sizeof(parameter)*2 );    
    if( paras_rsp == NULL )
    {
        fprintf(stderr,"malloc error! %s:%d\n",__FILE__,__LINE__);
        return -1;
    }
    
    //check user_name avalability
    if( DBCheckUserName(user_name) == 0 )
    {
        DBInsert(user_name, user_passwd);
        result = 1;
    }
    else
        result = 0;

    user_id = 0;//user_id = check(user_name, user_passwd);
    
    temp = htonl(result);
    memcpy( paras_rsp[0].data,
            &temp,
            sizeof(int) );
    paras_rsp[0].len = sizeof(int);
    
    temp = htonl(user_id);
    memcpy( paras_rsp[1].data,
            &temp,
            sizeof(int) );
    paras_rsp[1].len = sizeof(int);
    
    package( REGISTER_RSP,
             2,
             paras_rsp,
             &package_size,
             send_buf );
    
    res = send( sockfd,
                send_buf,
                package_size,
                0 );
                
    if( res < 0 )
    {
        fprintf(stderr,
                "send error! %s:%d\n",
                __FILE__,
                __LINE__);
        return -1;
    }

    return 0;
}

int 
NoteRsp( int sockfd )
{
    int i, temp, notes_fsize, notes_num, package_size, paras_num;
    FILE *notes_fp, *notes_num_fp;
    char notes_fname[] = "test_data/notes";
    char notes_num_fname[] = "test_data/notes_num";
    char send_buf[BUFSIZE];
    char notes_buf[BUFSIZE];
    parameter *paras;
    struct stat st;
    
    notes_num = 0;
    
    notes_num_fp = fopen(notes_num_fname,"rb+");
    if( notes_num_fp != NULL )
    {
        fread( &notes_num, 
               1, 
               sizeof(int), 
               notes_num_fp );
               
        if( fclose(notes_num_fp) < 0)
        {
            fprintf(stderr,
                    "fclose Error,%s:%d\n",
                    __FILE__,
                    __LINE__);
                    
            return -1;    
        }
    }
    
    paras = (parameter *)malloc(sizeof(parameter));
    
    temp = htonl(notes_num);    
    memcpy( paras[0].data, &temp, sizeof(int) );
    paras[0].len = sizeof(int);
    
    package( NOTE_RSP,
             1,
             paras,
             &package_size,
             send_buf );
             
    if( notes_num > 0 )
    {
        if( stat(notes_fname, &st) < 0 )
        {
            fprintf(stderr,"stat Error,%s:%d\n", __FILE__,__LINE__);
            return -1;
        }
        notes_fsize = (int)st.st_size;
        
        notes_fp = fopen(notes_fname,"rb+");    
        if( notes_fp == NULL )
        {
            fprintf(stderr,"fopen Error,%s:%d\n", __FILE__,__LINE__);
            return -1;
        }
        
        fread( notes_buf,
               1,
               notes_fsize,
               notes_fp );

        if( fclose(notes_fp) < 0 )
        {
            fprintf(stderr,"fclose Error,%s:%d\n", __FILE__,__LINE__);
            return -1;    
        }        
        
        paras_num = 1 + notes_num*3;
        paras_num = htonl(paras_num);
        memcpy( send_buf+sizeof(int), &paras_num, sizeof(int) );
        memcpy( send_buf+package_size, notes_buf, notes_fsize );
    }
    
    if( send( sockfd, send_buf, package_size+notes_fsize, 0 ) < 0 )
    {
        fprintf(stderr,
                "send Error,%s:%d\n",
                __FILE__,
                __LINE__);
                
        return -1;
    }

    return 0;
}

int 
NoteAdd( int sockfd, int paras_num, parameter *paras )
{
    int i, temp, notes_num, res;
    FILE *notes_fp, *notes_num_fp;
    char notes_fname[] = "test_data/notes";
    char notes_num_fname[] = "test_data/notes_num";
    
    notes_num_fp = fopen(notes_num_fname,"rb+");
    if( notes_num_fp != NULL )
    {
        notes_num = 0;
        fread( &notes_num,
               1, 
               sizeof(int), 
               notes_num_fp );
        
        if( fclose(notes_num_fp) < 0 )
        {
            fprintf(stderr,"fclose Error,%s:%d\n", __FILE__,__LINE__);
            return -1;
        }           
    }
    else notes_num = 0;
    
    dbg("before write notes_num = %d\n",notes_num);
    
    notes_num_fp = fopen(notes_num_fname,"wb+");
    
    notes_num++;
    fwrite( &notes_num,
            1,
            sizeof(int),
            notes_num_fp );    
    dbg("after write notes_num = %d\n",notes_num);
    
    if( fclose(notes_num_fp) < 0 )
    {
        fprintf(stderr,"fclose Error,%s:%d\n", __FILE__,__LINE__);
        return -1;
    }    
    
    notes_fp = fopen(notes_fname,"ab+");
    if( notes_fp == 0 )
    {
        fprintf(stderr,"fopen Error,%s:%d\n", __FILE__,__LINE__);
        return -1;
    }
        
    for( i=0; i<paras_num; i++ )
    {
        temp = htonl(paras[i].len);
        fwrite ( &temp,
                 1,
                 sizeof(int),
                 notes_fp );
                 
        fwrite ( paras[i].data,
                 1,
                 paras[i].len,
                 notes_fp );
    } 
    
    if( fclose(notes_fp) < 0 )
    {
        fprintf(stderr,"fclose Error,%s:%d\n", __FILE__,__LINE__);
        return -1;
    }

    if( NoteRsp( sockfd ) < 0 )
    {
        fprintf(stderr,"NoteRsp Error,%s:%d\n", __FILE__,__LINE__);
        return -1;
    }
    
    return 0;
}
