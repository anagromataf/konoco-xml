/*
 *  konoco_sax_delegate.h
 *  KonocoXML
 *
 *  Created by Tobias Kräntzer on 09.04.09.
 *  Copyright 2009 Konoco. All rights reserved.
 *
 */

#ifndef _konoco_sax_delegate_h_
#define _konoco_sax_delegate_h_

#include "konoco_sax.h"
#include "konoco_sax_handle.h"

void call_start_document(parser_handle * parser);
void call_end_document(parser_handle * parser);
void call_start_element(parser_handle * parser);
void call_end_element(parser_handle * parser);
void call_attribute(parser_handle * parser);
void call_characters(parser_handle * parser);
void call_error(parser_handle * parser, const char * msg);

#endif //_konoco_sax_delegate_h_
