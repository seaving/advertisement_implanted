/*************************************************************************************************************************************
 实现固定长度的缓冲区，用于共享内存中。平时就用长度可变的缓冲区
*************************************************************************************************************************************/
#include "includes.h"

#define min(x, y) ((x) < (y) ? (x) : (y))
	
/**************************************************************************************
* 函数功能：初始化FIFO结构体
* 入口参数：指针，缓冲区大小
* 返回数值：FIFO指针
* 调用函数：
***************************************************************************************/
int ft_fifo_fix_init(FT_FIFO_FIX * fifo, unsigned int dwSize)
{
    fifo->size = dwSize;
	fifo->in = fifo->out = 0;

	fifo->iLock = 0;

	return 0;
}

/************************************************************
* 函数功能：初始化FIFO结构体
* 入口参数：指针，缓冲区大小
* 返回数值：FIFO指针
* 调用函数：
************************************************************/
void _ft_fifo_fix_clear(FT_FIFO_FIX * fifo)
{
	fifo->in = fifo->out = 0;
}

/************************************************************
* 函数功能：将源缓冲区的数据拷贝到FIFO,改变写地址
* 入口参数：FIFO指针，源缓冲区指针，数据长度
* 返回数值：实际写入的数据个数
* 调用函数：
************************************************************/
unsigned int _ft_fifo_fix_put(FT_FIFO_FIX *fifo,
unsigned char *buffer, unsigned int len)
{
	unsigned int l,temp;
	unsigned int freesize;

	//检查FIFO是否有剩余空间
	if(fifo->out == ((fifo->in + 1) % fifo->size))
		return 0;
	
	// 计算FIFO的剩余空间
	if(fifo->in >= fifo->out)
		freesize = fifo->size - fifo->in + fifo->out - 1;
	else
		freesize = fifo->out - fifo->in - 1;
	
    // 取剩余空间与写入长度的较小者，防止溢出
	len = min(len, freesize);

	// 拷贝数据
	l = min(len, fifo->size - fifo->in % fifo->size );

    // 拷贝当前位置至末尾的一段
	memcpy(fifo->buffer + (fifo->in % fifo->size), buffer, l);

    // 拷贝剩余的一段
	memcpy(fifo->buffer, buffer + l, len - l);
	
  	// 更新写数据的指针
	temp = fifo->in;
	temp = temp + len;
	temp = temp % fifo->size;
	fifo->in = temp;

	return len;
}

/************************************************************************
* 函数功能：从FIFO中将数据读到用户缓冲区,改变读地址
* 入口参数：FIFO指针，源缓冲区指针，偏移地址(相对于读地址),读取数据长度
* 返回数值：实际读取的长度
* 调用函数：
************************************************************************/
unsigned int _ft_fifo_fix_get(FT_FIFO_FIX *fifo,
unsigned char *buffer, unsigned int offset, unsigned int len)
{
	unsigned int l,temp;
	unsigned int datasize;

	// 检查缓冲区内是否有有效数据
	if( fifo->out  == fifo->in )
		return 0;

	// 计算有效数据的长度
	if(fifo->out >= fifo->in)
		datasize = fifo->size - fifo->out + fifo->in;
	else
		datasize = fifo->in - fifo->out;

    // 计算新的读地址
	if(offset > datasize)
		return -1;
	else
	{
		datasize = datasize - offset;
		temp = fifo->out;
		temp = temp + offset;
		temp = temp % fifo->size;
		fifo->out = temp;
	}

	// 取需要读取的数据与数据长度的较小者
	len = min(len, datasize);

	// 拷贝数据到用户缓存
	l = min(len, fifo->size - (fifo->out % fifo->size));
	memcpy(buffer, fifo->buffer + (fifo->out % fifo->size), l);
	memcpy(buffer + l, fifo->buffer, len - l);

  	// 更新写数据的指针
	temp = fifo->out;
	temp = temp + len;
	temp = temp % fifo->size;
	fifo->out = temp;

	return len;
}

/***********************************************************************
* 函数功能：从FIFO中将数据读到用户缓冲区,<不改变读地址>
* 入口参数：FIFO指针，源缓冲区指针，偏移地址(相对于读地址),读取数据长度
* 返回数值：实际读取的长度
* 调用函数：
***********************************************************************/
unsigned int _ft_fifo_fix_seek(FT_FIFO_FIX *fifo,
unsigned char *buffer, unsigned int offset, unsigned int len)
{
	unsigned int l,temp;
	unsigned int datasize;

	// 检查缓冲区内是否有有效数据
	if( fifo->out  == fifo->in )
		return 0;

	// 计算有效数据的长度
	if(fifo->out > fifo->in)
		datasize = fifo->size - fifo->out + fifo->in;
	else
		datasize = fifo->in - fifo->out;

	// 计算新的读地址
	if(offset > datasize)
		return -1;
	else
	{
		datasize = datasize - offset;
		temp = fifo->out;
		temp = temp + offset;
		temp = temp % fifo->size;
	}
	
	// 取需要读取的数据与数据长度的较小者
	len = min(len, datasize);

	// 拷贝数据到用户缓存
	l = min(len, fifo->size - (temp % fifo->size));
	memcpy(buffer, fifo->buffer + (temp % fifo->size), l);
	memcpy(buffer + l, fifo->buffer, len - l);

	return len;
}
	
/************************************************************
* 函数功能：设置FIFO的读起始位置
* 入口参数：FIFO指针，偏移地址(相对于读地址)
* 返回数值：实际设置的偏移地址
* 调用函数：
************************************************************/
unsigned int _ft_fifo_fix_setoffset(FT_FIFO_FIX *fifo, unsigned int offset)
{
	unsigned int datasize;
	unsigned int l,temp;

    // 检查缓冲区内是否有有效数据
	if( fifo->out  == fifo->in )
		return 0;

	// 计算有效数据的长度
	if(fifo->out > fifo->in)
		datasize = fifo->size - fifo->out + fifo->in;
	else
		datasize = fifo->in - fifo->out;

	// 计算新的读地址
	l = min(offset, datasize);
	
	temp = fifo->out;
	temp = temp + l;
	temp = temp % fifo->size;
	fifo->out = temp;

	return l;
}

unsigned int _ft_fifo_fix_getlenth(FT_FIFO_FIX *fifo)
{
	unsigned int datasize;
	
	// 检查缓冲区内是否有有效数据
	if( fifo->out  == fifo->in )
		return 0;
	
	// 计算有效数据的长度
	if(fifo->out >= fifo->in)
		datasize = fifo->size - fifo->out + fifo->in;
	else
		datasize = fifo->in - fifo->out;

	return datasize;
}

// 锁定，表示在使用
void _ft_fifo_fix_lock(FT_FIFO_FIX * fifo)
{
    int i = 0;

    // TraceLog(O_NULL, "ft_fifo_LOCK\r\n");

    // 其它的进程在使用FIFO，等待
    while (1 == fifo->iLock)
    {
        i++;
        usleep(1);
        if (i > 10000)
        {
            LOG_ERROR_INFO("ft_fifo_LOCK 失败!\r\n");
            return;
        }
    }

    fifo->iLock = 1;
}

// 解除锁定，表示空闲
void _ft_fifo_fix_unlock(FT_FIFO_FIX * fifo)
{
    // TraceLog(O_NULL, "ft_fifo_UNLOCK\r\n");

    fifo->iLock = 0;
}


