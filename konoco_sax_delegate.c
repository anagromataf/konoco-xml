/*
 *  konoco_sax_delegate.c
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

#include "konoco_sax_delegate.h"


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

void
_flush(parser_handle * parser)
{
	if (parser->delegate && parser->delegate->_flush) {
		(parser->delegate->_flush)(parser, parser->delegate->data);
	};
}

