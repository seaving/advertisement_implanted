

#ifndef __FIFO_FIX_H__
#define __FIFO_FIX_H__


#define FIFO_FIX_LEN  204800  // 1024 * 200

typedef struct ft_fifo_FIX
{
	unsigned char   buffer[FIFO_FIX_LEN];   /* the buffer holding the data              */
	unsigned int    size;                   /* the size of the allocated buffer         */
	unsigned int    wrsize;                 /* the size of the start to write to file   */
	unsigned int    in;                     /* data is added at offset (in % size)      */
	unsigned int    out;                    /* data is extracted from off. (out % size) */
	unsigned int    iLock;                   /* protects concurrent modifications        */
}FT_FIFO_FIX;


int ft_fifo_fix_init(FT_FIFO_FIX * fifo, unsigned int dwSize);



extern void _ft_fifo_fix_clear(FT_FIFO_FIX *fifo);
extern unsigned int _ft_fifo_fix_put(FT_FIFO_FIX *fifo,
				  unsigned char *buffer, unsigned int len);
extern unsigned int _ft_fifo_fix_get(FT_FIFO_FIX *fifo,
			   unsigned char *buffer, unsigned int offset, unsigned int len);
extern unsigned int _ft_fifo_fix_seek(FT_FIFO_FIX *fifo,
			   unsigned char *buffer, unsigned int offset, unsigned int len);
extern unsigned int _ft_fifo_fix_setoffset(FT_FIFO_FIX *fifo,unsigned int offset);
extern unsigned int _ft_fifo_fix_getlenth(FT_FIFO_FIX *fifo);

void _ft_fifo_fix_lock(FT_FIFO_FIX * fifo);
void _ft_fifo_fix_unlock(FT_FIFO_FIX *fifo);


static inline void ft_fifo_fix_clear(FT_FIFO_FIX *fifo)
{
	// pthread_mutex_lock(&fifo->lock);
	_ft_fifo_fix_lock(fifo);

    _ft_fifo_fix_clear(fifo);

    // pthread_mutex_unlock(&fifo->lock);
    _ft_fifo_fix_unlock(fifo);
}

static inline unsigned int ft_fifo_fix_put(FT_FIFO_FIX *fifo,
				       unsigned char *buffer, unsigned int len)
{
	unsigned int ret;

	_ft_fifo_fix_lock(fifo);
	// pthread_mutex_lock(&fifo->lock);
	
	ret = _ft_fifo_fix_put(fifo, buffer, len);

    _ft_fifo_fix_unlock(fifo);
	// pthread_mutex_unlock(&fifo->lock);

	return ret;
}

static inline unsigned int ft_fifo_fix_get(FT_FIFO_FIX *fifo,
			   unsigned char *buffer, unsigned int offset, unsigned int len)
{
	unsigned int ret;

	_ft_fifo_fix_lock(fifo);
	// pthread_mutex_lock(&fifo->lock);

	ret = _ft_fifo_fix_get(fifo, buffer, offset,len);
	if (fifo->in == fifo->out)
		fifo->in = fifo->out = 0;

    _ft_fifo_fix_unlock(fifo);
	// pthread_mutex_unlock(&fifo->lock);

	return ret;
}

static inline unsigned int ft_fifo_fix_getlenth(FT_FIFO_FIX *fifo)
{
	unsigned int ret;

	_ft_fifo_fix_lock(fifo);
	// pthread_mutex_lock(&fifo->lock);

	ret = _ft_fifo_fix_getlenth(fifo);

    _ft_fifo_fix_unlock(fifo);
	// pthread_mutex_unlock(&fifo->lock);

	return ret;
}

static inline unsigned int ft_fifo_fix_seek(FT_FIFO_FIX * fifo,
			   unsigned char *buffer, unsigned int offset, unsigned int len)

{
	unsigned int ret;

	_ft_fifo_fix_lock(fifo);
	//pthread_mutex_lock(&fifo->lock);

	ret = _ft_fifo_fix_seek(fifo, buffer, offset, len);

    _ft_fifo_fix_unlock(fifo);
	// pthread_mutex_unlock(&fifo->lock);

	return ret;
}

static inline unsigned int ft_fifo_fix_setoffset(FT_FIFO_FIX * fifo,unsigned int offset)

{
	unsigned int ret;

	_ft_fifo_fix_lock(fifo);
	// pthread_mutex_lock(&fifo->lock);

	ret = _ft_fifo_fix_setoffset(fifo, offset);

    _ft_fifo_fix_unlock(fifo);
	// pthread_mutex_unlock(&fifo->lock);

	return ret;
}

#endif

