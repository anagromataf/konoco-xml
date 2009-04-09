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

#include <stdlib.h>
#include <string.h>

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

// Tokens, retuned by the lexer
enum lexer_token {
	token_error,
	token_eof,
	
	// This token is used, if the end of the
	// bufer was reached but the end of the
	// token is not reached. 
	token_input_needed,
	
	// Data, not in a tag, pi, ...
	token_characters,
	
	// A name for an element, pi or attribute
	token_name,
	
	// The value of an attribute with "'" or '"'
	token_string,
	
	// "="
	token_eq,
	
	// White space
	token_ws,
	
	// "<?xml"
	token_begin_xml_decl,
	
	// "?>"
	token_end_pi,
	
	// "<", "</", ">", "/>"
	// The names below are not correct,
	// but I have to use something.
	token_begin_stag,
	token_begin_etag,
	token_end_stag,
	token_end_etag
};

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
} parser_handle;

enum lexer_token
get_next_token(parser_handle * parser);

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

#pragma mark Functions to call the Delegate

void
call_start_document(parser_handle * parser)
{
	if (parser->delegate && parser->delegate->start_document) {
		(parser->delegate->start_document)(parser,
										   parser->delegate->data);
	};
}

void
call_end_document(parser_handle * parser)
{
	if (parser->delegate && parser->delegate->end_document) {
		(parser->delegate->end_document)(parser,
										 parser->delegate->data);
	};
}

void
call_start_element(parser_handle * parser)
{
	if (parser->delegate && parser->delegate->start_element) {
		(parser->delegate->start_element)(parser,
										  parser->delegate->data,
										  &parser->element_name, 0);
	};
}

void
call_end_element(parser_handle * parser)
{
	if (parser->delegate && parser->delegate->end_element) {
		(parser->delegate->end_element)(parser,
										parser->delegate->data,
										&parser->element_name, 0);
	};
}

void
call_attribute(parser_handle * parser)
{
	if (parser->delegate && parser->delegate->attribute) {
		(parser->delegate->attribute)(parser,
									  parser->delegate->data,
									  &parser->attr_name, 0,
									  &parser->attr_value);
	};
}

void
call_characters(parser_handle * parser)
{
	if (parser->delegate && parser->delegate->characters) {
		(parser->delegate->characters)(parser,
									   parser->delegate->data,
									   &parser->lexer_value);
	};
}

void
call_error(parser_handle * parser, const char * msg)
{
	if (parser->delegate && parser->delegate->error) {
		(parser->delegate->error)(parser, parser->delegate->data, msg);
	};
}

#pragma mark Parameter & Co

void
set_element_name(parser_handle * parser)
{
	konoco_buffer_cpy(&parser->element_name, &parser->lexer_value);
}

void
set_attr_name(parser_handle * parser)
{
	konoco_buffer_cpy(&parser->attr_name, &parser->lexer_value);
}

void
set_attr_value(parser_handle * parser)
{
	konoco_buffer_cpy(&parser->attr_value, &parser->lexer_value);
}

int
set_parser_parameter(parser_handle * parser)
{
	if (konoco_buffer_eq_str(&parser->attr_name, "version")) {
		// set version
		if (konoco_buffer_eq_str(&parser->attr_value, "1.0")) {
			parser->version = konoco_xml_version_1_0;
			return (0);
		} else if (konoco_buffer_eq_str(&parser->attr_value, "1.1")) {
			parser->version = konoco_xml_version_1_1;
			return (0);
		};
	} else if (konoco_buffer_eq_str(&parser->attr_name, "charset")) {
		// set charset
		parser->charset = malloc(parser->attr_value.length + 1);
		memcpy(parser->charset, parser->attr_value.data, parser->attr_value.length);
		parser->charset[parser->attr_value.length + 1] = 0;
		return (0);
	};
	return (0);
}

#pragma mark Lexer

int
is_name_start_char(unsigned char c)
{
	if (c == ':' || c == '_' ||
		('A' <= c && c <= 'Z') ||
		('a' <= c && c <= 'z') ||
		(0xc0 <= c && c <= 0xd6) ||
		(0xd8 <= c && c <= 0xf6) ||
		(0xf8 <= c)) {
		return (1);
	} else {
		return (0);
	};
}

int
is_name_char(unsigned char c)
{
	if (is_name_start_char(c))
		return (1);
	else if (c == '-' || c == '.' ||
			 ('0' <= c && c <= '9') ||
			 c == 0xb7)
		return (1);
	else
		return (0);
}

int
find_string_end(parser_handle * parser, int position, char delimiter)
{
	while (position < parser->buffer_end) {
		char c = parser->buffer[position];
		if (c == '<' || c == '&' ) {
			// not allowed char
			return (-2);
		} else if (c == delimiter) {
			return (position);
		} else {
			position++;
		};
	};
	// we need more input
	return (-1);
}

int
find_name_end(parser_handle * parser, int position)
{
	while (position < parser->buffer_end) {
		char c = parser->buffer[position];
		if (is_name_char(c)) {
			position++;
		} else {
			return (position);
		};
	};
	// we need more input
	return (-1);
}

enum lexer_token
get_next_token(parser_handle * parser)
{
	if (parser->position >= parser->buffer_end)
		return (token_input_needed);
	
	switch (parser->state) {
		case state_characters:
			// Set the begin of the lexem to
			// the postion of the parser.
			parser->lexer_value.length = 0;
			parser->lexer_value.data = parser->buffer + parser->position;

			do {
				// Get the current char.
				unsigned char c = parser->buffer[parser->position];
			
				if (c == '<' || c == '&' || c == '\0') {
					break;
				} else {
					parser->lexer_value.length++;
					parser->position++;
				};
			} while (parser->position < parser->buffer_end);
			
			// If the length of the char data section is 0,
			// we weren't able to consume any chars.
			if (parser->lexer_value.length > 0) {
				return (token_characters);
			};
			break;
	};
	
	// Find tokens in the 'normal' state.
	char c = parser->buffer[parser->position];
	
	switch (c) {
		case '\0':
			return (token_eof);
			
		case '<':
		{
			// We need at lest one more char, to decide
			// which token we have.
			if (parser->position + 1 >= parser->buffer_end)
				return (token_input_needed);
			c = parser->buffer[++parser->position];
			switch (c) {
				case '?':
					// We need at least 3 more chars to
					// decide if we are in the prolog ("<?xml ...").
					if (parser->position + 3 >= parser->buffer_end)
						return (token_input_needed);
					c = parser->buffer[parser->position + 1];
					if (c != 'X' && c != 'x') {
						parser->position = parser->position + 1;
						return (token_error);
					};
					c = parser->buffer[parser->position + 2];
					if (c != 'M' && c != 'm') {
						parser->position = parser->position + 2;
						return (token_error);
					};
					c = parser->buffer[parser->position + 3];
					if (c != 'L' && c != 'l') {
						parser->position = parser->position + 3;
						return (token_error);
					};
					parser->position = parser->position + 4;
					return (token_begin_xml_decl);
				case '/':
					parser->position++;
					return (token_begin_etag);
				default:
					return (token_begin_stag);
			};
		};
			
		case '>':
			parser->position++;
			return (token_end_stag);
			
		case '/':
		{
			// We need at lest one more char, to decide
			// which token we have.
			if (parser->position + 1 >= parser->buffer_end)
				return (token_input_needed);
			c = parser->buffer[++parser->position];
			if (c == '>') {
				parser->position++;
				return (token_end_etag);
			} else {
				parser->position;
				return (token_error);
			};
		};
			
		case '?':
		{
			// We need at lest one more char, to decide
			// which token we have.
			if (parser->position + 1 >= parser->buffer_end)
				return (token_input_needed);
			c = parser->buffer[++parser->position];
			if (c == '>') {
				parser->position++;
				return (token_end_pi);
			} else {
				return (token_error);
			};
		};
			
		case '=':
			parser->position++;
			return (token_eq);
			
		case '\'':
		case '"':
		{
			// String
			int pos = find_string_end(parser, parser->position + 1, c);
			switch (pos) {
				case -1:
					// More input needed
					return (token_input_needed);
				case -2:
					// Not allowd char
					parser->position = pos;
					return (token_error);
				default:
					parser->lexer_value.data = parser->buffer + parser->position + 1;
					parser->lexer_value.length = pos - parser->position - 1;
					parser->position = ++pos;
					return (token_string);
			};
		};
			
		case 0x20:
		case 0x9:
		case 0xD:
		case 0xA:
			// White space
			parser->position++;
			return (token_ws);
			
		default:
			// Test for the rest ...
			if (is_name_start_char(c)) {
				int pos = find_name_end(parser, parser->position + 1);
				switch (pos) {
					case -1:
						// More input needed
						return (token_input_needed);
					case -2:
						// Not allowd char
						parser->position = pos;
						return (token_error);
					default:
						parser->lexer_value.data = parser->buffer + parser->position;
						parser->lexer_value.length = pos - parser->position;
						parser->position = pos;
						return (token_name);
				};
			};
	};
	
	return (token_error);
}

#pragma mark Parser


enum lexer_token
do_parsing(parser_handle * parser)
{
	enum lexer_token token;
	do {
		token = get_next_token(parser);
		
		if (token == token_input_needed) {
			return (token);
		};
		
		switch (parser->state) {
			case state_not_started:
			{
				// If we start the parser, we want to begin with
				// the xml delc.
				switch (token) {
					case token_begin_xml_decl:
						parser->state = state_xml_decl;
						continue;
					default:
						parser->state = state_error;
						call_error(parser, "We have to start with '<?xml'.");
						return (token);
				};
			};
			
			case state_xml_decl:
			{
				// We are in the xml decl. No we have to look
				// for the version and the charset.
				
				switch (token) {
					case token_name:
						if (parser->sub_state != state_undefined) {
							// If we start with an attribute delc. We have to
							// to be in the sub_state undefined.
							parser->state = state_error;
							call_error(parser, "Syntax error ...");
							return (token);							
						};
						set_attr_name(parser);
						parser->sub_state = state_begin_attr;
						continue;
					case token_eq:
						if (parser->sub_state != state_begin_attr) {
							parser->state = state_error;
							call_error(parser, "Syntax error ...");
							return (token);
						};
						parser->sub_state = state_attr;
						continue;
					case token_string:
						if (parser->sub_state != state_attr) {
							parser->state = state_error;
							call_error(parser, "Syntax error ...");
							return (token);							
						};
						set_attr_value(parser);
						parser->sub_state = state_undefined;
						if (set_parser_parameter(parser) == 0) {
							continue;
						} else {
							return (token);
						};
					case token_ws:
						continue;
					case token_end_pi:
						call_start_document(parser);
						parser->state = state_characters;
						continue;
					default:
						parser->state = state_error;
						call_error(parser, "Syntax error ...");
						return (token);
				};
			};
			
			case state_characters:
			{
				switch (token) {
					case token_characters:
						call_characters(parser);
						continue;
					case token_begin_stag:
						parser->state = state_begin_stag;
						continue;
					case token_begin_etag:
						parser->state = state_begin_etag;
						continue;
					case token_eof:
						parser->state = state_stopped;
						call_end_document(parser);
						return (token);
					default:
						parser->state = state_error;
						call_error(parser, "Syntax error ...");
						return (token);
				};
			};
			
			case state_begin_stag:
			{
				switch (token) {
					case token_name:
						set_element_name(parser);
						parser->state = state_stag;
						parser->sub_state = state_undefined;
						call_start_element(parser);
						continue;
					default:
						parser->state = state_error;
						call_error(parser, "Syntax error: We have to start a tag with a name.");
						return (token);
				};
			};
			
			case state_begin_etag:
			{
				switch (token) {
					case token_name:
						set_element_name(parser);
						parser->state = state_etag;
						parser->sub_state = state_undefined;
						call_end_element(parser);
						continue;
					default:
						parser->state = state_error;
						call_error(parser, "Syntax error: We have to start a tag with a name.");
						return (token);
				};
			};
			
			case state_stag:
			{
				switch (token) {
					case token_end_stag:
						if (parser->sub_state != state_undefined) {
							parser->state = state_error;
							call_error(parser, "Syntax error ...");
							return (token);							
						};
						parser->state = state_characters;
						continue;
					case token_end_etag:
						if (parser->sub_state != state_undefined) {
							parser->state = state_error;
							call_error(parser, "Syntax error ...");
							return (token);							
						};
						call_end_element(parser);
						parser->state = state_characters;
						continue;
					case token_name:
						if (parser->sub_state != state_undefined) {
							// If we start with an attribute delc. We have to
							// to be in the sub_state undefined.
							parser->state = state_error;
							call_error(parser, "Syntax error ...");
							return (token);							
						};
						set_attr_name(parser);
						parser->sub_state = state_begin_attr;
						continue;
					case token_eq:
						if (parser->sub_state != state_begin_attr) {
							parser->state = state_error;
							call_error(parser, "Syntax error ...");
							return (token);
						};
						parser->sub_state = state_attr;
						continue;
					case token_string:
						if (parser->sub_state != state_attr) {
							parser->state = state_error;
							call_error(parser, "Syntax error ...");
							return (token);							
						};
						set_attr_value(parser);
						parser->sub_state = state_undefined;
						call_attribute(parser);
						continue;
					case token_ws:
						continue;
					default:
						parser->state = state_error;
						call_error(parser, "Syntax error ...");
						return (token);
				};
				
			};
			
			case state_etag:
			{
				switch (token) {
					case token_end_stag:
						if (parser->sub_state != state_undefined) {
							parser->state = state_error;
							call_error(parser, "Syntax error ...");
							return (token);							
						};
						parser->state = state_characters;
						continue;
					case token_ws:
						continue;
					default:
						parser->state = state_error;
						call_error(parser, "Syntax error ...");
						return (token);
				};
			};
		};
	} while (token != token_error ||
			 token != token_input_needed ||
			 token != token_eof);
	
	return (token);
}

