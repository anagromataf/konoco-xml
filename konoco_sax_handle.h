/*
 *  konoco_sax_handle.h
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

#ifndef _konoco_sax_handle_h_
#define _konoco_sax_handle_h_

#include "konoco_sax.h"
#include "konoco_xml.h"
#include "konoco_buffer.h"

// The state of the parser
enum parser_state {
	state_not_started,
	state_stopped,
	state_error,
	state_undefined,
	state_characters,
	state_xml_decl,
	state_begin_stag,
	state_stag,
	state_begin_etag,
	state_etag,
	state_begin_attr,
	state_attr
};

typedef struct _attribute_list {
	konoco_buffer prefix;
	konoco_buffer name;
	konoco_buffer value;
	struct _attribute_list * next;
} attribute_list;

typedef struct _ns_delegate_data {
	konoco_sax_delegate * orig_delegate;
	
	konoco_buffer element_prefix;
	konoco_buffer element_name;
	attribute_list * attributes;
} ns_delegate_data;

// The Parser Handle
typedef struct _parser_handle {
	
	// A pointer to the struct with the funstions,
	// which are called if on events in the
	// document.
	konoco_sax_delegate * delegate;
	
	// The buffer with the data to parse.
	unsigned char * buffer;
	
	// The size of the buffer (allocated memory).
	int buffer_size;
	
	// The end (offset) of the region in the buffer,
	// which holds usefull data (after last byte).
	int buffer_end;
	
	// Position (offset) in the buffer where the parser
	// should continue parsing.
	int position;
	
	char * charset;
	enum konoco_xml_version version;
	
	enum parser_state state;
	enum parser_state sub_state;
	
	int min_input_size;
	konoco_buffer input;
	konoco_buffer element_name;
	konoco_buffer attr_name;
	konoco_buffer attr_value;
	konoco_buffer lexer_value;
	
	// ns resolver
	void * ns_resolver;
	konoco_sax_delegate ns_delegate;
	ns_delegate_data ns_delegate_data;
	
} parser_handle;

#endif // _konoco_sax_handle_h_
