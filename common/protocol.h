/*protocol.h*/

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdio.h>
#include <string.h>

#define BUFSIZE 1024*512
#define MAX_PARA_NUM 128
#define MAX_PARA_LENGTH 1024

typedef struct{
    char data[MAX_PARA_LENGTH];
    int len;
}parameter;

int 
package( int op_code,
         int para_num,
         parameter *paras,
         int *package_size,
         char *buf );
         
int 
parse( int *op_code,
       int *paras_num,
       parameter *paras,
       char *buf );
       
#endif 
