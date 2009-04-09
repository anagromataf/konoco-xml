/*
 *  konoco_sax_parser.c
 *  KonocoXML
 *
 *  Created by Tobias Kräntzer on 09.04.09.
 *  Copyright 2009 Konoco. All rights reserved.
 *
 */


#include "konoco_sax.h"
#include "konoco_sax_parser.h"
#include "konoco_buffer.h"
#include "konoco_sax_handle.h"
#include "konoco_sax_delegate.h"

#include <stdlib.h>
#include <string.h>

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

