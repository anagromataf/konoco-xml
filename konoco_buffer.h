/*
 *  konoco_buffer.h
 *  KonocoXML
 *
 *  Created by Tobias Kräntzer <info@tobias-kraentzer.de> on 29.03.09.
 *  Copyright 2009 Konoco. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY Tobias Kräntzer ''AS IS'' AND ANY
 *  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL Tobias Kräntzer BE LIABLE FOR ANY
 *  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _konoco_buffer_h_
#define _konoco_buffer_h_


/**	A konoco_buffer is used to describe a region in memory, which
 *	begins at 'data' and is 'length' bytes long.
 *
 *	The filed '_size' is for internal use. It is set to the amount
 *	of bytes allocated for this buffer. Don't alter it manually.
 */
typedef struct {
	unsigned char * data;
	int length;
	int _size;
} konoco_buffer;


/** This function is used to initialize a konoco_buffer. It only sets all
 *	the fields 'data', 'length' and '_size' to NULL.
 */
void
konoco_buffer_init(konoco_buffer * buffer);

/** This function frees the memory allocated by copy functions. 
 *	(If the field '_size' is > 0)
 */
void
konoco_buffer_free(konoco_buffer * buffer);

/** This function copies length bytes from data in the buffer.
 *	If the buffer is not big enough, it will be reallocated.
 */
void
konoco_buffer_memcpy(konoco_buffer * buffer, const unsigned char * data, int length);

/**	This function copies the NULL terminated string in the buffer without
 *	the terminating NULL. (It calls konoco_buffer_memcpy)
 */
void
konoco_buffer_strcpy(konoco_buffer * buffer, const char * str);

/** This function copies the content of orig in the buffer.
 *	If the buffer is not big enough, it will be reallocated.
 */
void
konoco_buffer_cpy(konoco_buffer * buffer, const konoco_buffer * orig);

/** This function copies the content of orig in the un initialized
 *  buffer. Same as calling konoco_buffer_init and konoco_buffer_cpy.
 */
void
konoco_buffer_init_cpy(konoco_buffer * buffer, const konoco_buffer * orig);

/**	This function comperes buffer a with buffer b. If both buffer are
 *	equal (length and content), it returns 1 else 0.
 */
int
konoco_buffer_eq(const konoco_buffer * a, const konoco_buffer * b);

/** This function compares the buffer with the NULL terminated
 *	string (without the NULL). If both are equal (length and content),
 *	it returns 1 else 0.
 */
int
konoco_buffer_eq_str(const konoco_buffer * buffer, const char * str);

#endif // _konoco_buffer_h_
