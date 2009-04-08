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

typedef struct {
	unsigned char * data;
	int length;
	int _size;
} konoco_buffer;

void
konoco_buffer_init(konoco_buffer * buffer);

void
konoco_buffer_free(konoco_buffer * buffer);

void
konoco_buffer_memcpy(konoco_buffer * buffer, const unsigned char * data, int length);

void
konoco_buffer_strcpy(konoco_buffer * buffer, const char * str);

void
konoco_buffer_cpy(konoco_buffer * buffer, const konoco_buffer * orig);

int
konoco_buffer_eq(const konoco_buffer * a, const konoco_buffer * b);

int
konoco_buffer_eq_str(const konoco_buffer * buffer, const char * str);

#endif // _konoco_buffer_h_
