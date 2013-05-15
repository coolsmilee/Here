#include <stdio.h>
#include <sys/stat.h>
#include <sys/socket.h>

void main(void)
{
    int i, file_size;
    char file_name[] = "test_data/note_file";
    char buf[1024];
    FILE *note_file;    
    struct stat st;
    
    note_file = fopen(file_name,"ab+");
    
    for( i=0; i<5; i++ )
    {
        fwrite ( &i,
                 1,
                 sizeof(int),
                 note_file );
    }

    if(stat( file_name, &st ) == 0)
    {
        printf("sizeof file is %d\n",(int)st.st_size);
    }
    
    file_size = (int)st.st_size;
    
    fseek( note_file,
           0,
           SEEK_SET );
           
    printf( "fread res is %d\n", 
            fread(buf,1,file_size,note_file) );
    
    fclose(note_file);
    
    return;
}
