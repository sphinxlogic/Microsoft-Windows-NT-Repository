int mem_fill (old,new,bytes)
char *old,*new;
int bytes;
{

    for(;(bytes+1);bytes--) *(new+bytes) = *(old+bytes);

};
