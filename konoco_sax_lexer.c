/*
 *  konoco_sax_lexer.c
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

#include "konoco_sax_lexer.h"


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
