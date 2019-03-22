/* Copyright (C) 2007 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <sys/syscall.h>
#include <fcntl.h>

/* Reserve storage for the data of the file associated with FD.  */
int
posix_fallocate (int fd, __off_t offset, __off_t len)
{
#ifdef __NR_fallocate
	INTERNAL_SYSCALL_DECL (err);
	int res = INTERNAL_SYSCALL (fallocate, err, 6, fd, 0,
				__LONG_LONG_PAIR (offset >> 31, offset),
				__LONG_LONG_PAIR (len >> 31, len));

	if (! INTERNAL_SYSCALL_ERROR_P (res, err))
		return 0;

	if (INTERNAL_SYSCALL_ERRNO (res, err) != EOPNOTSUPP)
		return INTERNAL_SYSCALL_ERRNO (res, err);
#else
	return ENOSYS;
#endif
}
