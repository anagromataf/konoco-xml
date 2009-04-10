/*
 *  konoco_sax_delegate.c
 *  KonocoXML
 *
 *  Created by Tobias Kräntzer on 09.04.09.
 *  Copyright 2009 Konoco. All rights reserved.
 *
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

