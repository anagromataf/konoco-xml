/*
 *  konoco_sax.c
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

#include "konoco_sax.h"
#include "konoco_sax_handle.h"
#include "konoco_sax_lexer.h"
#include "konoco_sax_namespace_handler.h"

#include <stdlib.h>
#include <string.h>

enum lexer_token
do_parsing(parser_handle * parser);

#pragma mark External Functions (a.k.a. API)

void *
konoco_sax_create(int buffer_size, int flags)
{
	parser_handle * parser = malloc(sizeof(parser_handle));
	if (!parser) {
		// Memory could not be allocated.
		return (0);
	} else {
		// Set the default values.
		parser->min_input_size = buffer_size;
		parser->charset = 0;
		parser->version = konoco_xml_version_undefined;
		parser->delegate = 0;
		parser->buffer = realloc(0, parser->min_input_size);
		parser->buffer_size = parser->min_input_size;
		parser->buffer_end = 0;
		parser->position = 0;
		parser->state = state_not_started;
		parser->sub_state = state_undefined;

		// init the buffer for the names ...
		// calling realloc with size 0 to get a "minimum sized object"
		parser->input.length = parser->buffer_size;
		parser->input.data = parser->buffer;
		konoco_buffer_init(&parser->element_name);
		konoco_buffer_init(&parser->attr_name);
		konoco_buffer_init(&parser->attr_value);
		
		// set the point to the ns resolver to NULL
		parser->ns_resolver = 0;

		konoco_buffer_init(&parser->ns_delegate_data.element_prefix);
		konoco_buffer_init(&parser->ns_delegate_data.element_name);
		
		return (parser);
	};
}

void
konoco_sax_destroy(void * handle)
{
	parser_handle * parser = (parser_handle *)handle;
	
	// Free all buffers (if allocated).
	if (parser->buffer != 0) {
		free(parser->buffer);
	};
	
	konoco_buffer_free(&parser->element_name);
	konoco_buffer_free(&parser->attr_name);
	konoco_buffer_free(&parser->attr_value);
	
	konoco_buffer_free(&parser->ns_delegate_data.element_prefix);
	konoco_buffer_free(&parser->ns_delegate_data.element_name);
	
	// Free the parser.
	free (parser);
}

konoco_buffer *
konoco_sax_get_buffer(void * p)
{
	parser_handle * parser = (parser_handle *)p;
	return (&parser->input);
}

void
konoco_sax_set_delegate(void * handle, konoco_sax_delegate * delegate, int flags)
{
	parser_handle * parser = (parser_handle *)handle;
	parser->delegate = delegate;
	// make sure that the pointer to the internal
	// function '_flush' is NULL
	parser->delegate->_flush = 0;
	if ((flags & RESOLVE_NAMESPACE) == RESOLVE_NAMESPACE) {
		// inject the delegate to hadle namespaces
		inject_namespace_delegate(parser);
	};
}

int
konoco_sax_parse(void * handle, int length)
{
	parser_handle * parser = (parser_handle *)handle;

	parser->buffer_end += length;
	
	// do the magic
	do_parsing(parser);
	
	// clean up
	memmove(parser->buffer, parser->buffer + parser->position, parser->buffer_end - parser->position);
	parser->buffer_end -= parser->position;
	parser->position = 0;
	
	if (parser->buffer_size - parser->buffer_end < parser->min_input_size) {
		parser->buffer = realloc(parser->buffer, parser->buffer_end + parser->min_input_size);
		parser->buffer_size = parser->buffer_end + parser->min_input_size;
	};
	
	parser->input.data = parser->buffer + parser->buffer_end;
	parser->input.length = parser->buffer_size - parser->buffer_end;
	
	return (0);
}

const char *
konoco_sax_get_charset(void * handle)
{
	parser_handle * parser = (parser_handle *)handle;
	return (parser->charset);
}

enum konoco_xml_version
konoco_sax_get_version(void * handle)
{
	parser_handle * parser = (parser_handle *)handle;
	return (parser->version);
}
