#ifndef _ZIP_MEM_H_
#define _ZIP_MEM_H_


/*
函数功能: 压缩内存,压缩后的数据存放在mem_outptr所指向的空间
参数说明:
mem_inptr: 待压缩的内存指针，由用户提供缓存空间
mem_insize: 待压缩的内存长度
mem_outptr: 压缩后的内存指针, 由用户提供缓存空间
返回值:
压缩后的内存长度
*/
int zipmem( char * mem_inptr, int mem_insize, char * mem_outptr );

/*
函数功能: 解压缩, 解压缩后的数据存放在mem_outptr所指向的空间
参数说明:
mem_inptr: 压缩后的内存指针，由用户提供缓存空间
mem_insize: 压缩后的内存长度
mem_outptr: 解压缩后的内存指针, 由用户提供缓存空间
返回值:
解压缩的内存长度
*/
int unzipmem( char * mem_inptr, int mem_insize, char * mem_outptr );

#endif 
