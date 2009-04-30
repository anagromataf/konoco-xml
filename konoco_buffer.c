/*
 *  konoco_buffer.c
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

// TODO: Handle the case if no memory could be allocated.

#include "konoco_buffer.h"

#include <stdlib.h>
#include <string.h>

void
konoco_buffer_init(konoco_buffer * buffer)
{
	buffer->data = 0;
	buffer->length = 0;
	buffer->_size = 0;
}

void
konoco_buffer_free(konoco_buffer * buffer)
{
	if (buffer->_size > 0) {
		free(buffer->data);
		buffer->data = 0;
		buffer->length = 0;
		buffer->_size = 0;
	};
}

void
konoco_buffer_memcpy(konoco_buffer * buffer, const unsigned char * data, int length)
{
	// Reallocate the buffer if we need more space.
	if (length > buffer->_size) {
		buffer->data = realloc(buffer->data, length);
		buffer->_size = length;
	}
	buffer->length = length;
	memcpy(buffer->data, data, length);
}

void
konoco_buffer_strcpy(konoco_buffer * buffer, const char * str)
{
	int length = strlen(str);
	return (konoco_buffer_memcpy(buffer, (const unsigned char *)str, length));
}

void
konoco_buffer_cpy(konoco_buffer * buffer, const konoco_buffer * orig)
{
	// Reallocate the buffer if we need more space.
	if (orig->length > buffer->_size) {
		buffer->data = realloc(buffer->data, orig->length);
		buffer->_size = orig->length;
	}
	buffer->length = orig->length;
	memcpy(buffer->data, orig->data, orig->length);
}

void
konoco_buffer_init_cpy(konoco_buffer * buffer, const konoco_buffer * orig)
{
	buffer->data = malloc(orig->length);
	buffer->_size = orig->length;
	buffer->length = orig->length;
	memcpy(buffer->data, orig->data, orig->length);
}

int
konoco_buffer_eq(const konoco_buffer * a, const konoco_buffer * b)
{
	if ((a->length == b->length) && (memcmp(a->data, b->data, a->length) == 0))
		return (1);
	else
		return (0);
}

int
konoco_buffer_eq_str(const konoco_buffer * buffer, const char * str)
{
	int length = strlen(str);
	if ((length == buffer->length) && (memcmp(str, buffer->data, length) == 0))
		return (1);
	else
		return (0);
}
