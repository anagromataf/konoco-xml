/*
 *  konoco_sax.h
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

#ifndef _konoco_sax_h_
#define _konoco_sax_h_

#include "konoco_xml.h"
#include "konoco_buffer.h"

enum konoco_sax_flags {
	RESOLVE_NAMESPACE = 0x1
};

typedef struct _konoco_sax_delegate {
	void * data;
	void (*start_document)(void * parser, void * data);
	void (*end_document)(void * parser, void * data);
	void (*start_element)(void * parser, void * data, konoco_buffer * name, konoco_buffer * namespace);
	void (*end_element)(void * parser, void * data, konoco_buffer * name, konoco_buffer * namespace);
	void (*attribute)(void * parser, void * data, konoco_buffer * name, konoco_buffer * namespace, konoco_buffer * value);
	void (*characters)(void * parser, void * data, konoco_buffer * text);
	void (*xmlns)(void * parser, void * data, konoco_buffer * prefix, konoco_buffer * namespace);
	void (*error)(void * parser, void * data, const char * msg);
	
	// internal fields & functions
	struct _konoco_sax_delegate * _parent;
	void (*_flush)(void * parser, void * data);
} konoco_sax_delegate;

void *
konoco_sax_create(int buffer_size, int flags);

void
konoco_sax_destroy(void * parser);

konoco_buffer *
konoco_sax_get_buffer(void * parser);

void
konoco_sax_set_delegate(void * parser, konoco_sax_delegate * delegate, int flags);

int
konoco_sax_parse(void * parser, int length);

const char *
konoco_sax_get_charset(void * parser);

enum konoco_xml_version
konoco_sax_get_version(void * parser);

#endif // _konoco_sax_h_
