/*
 *  konoco_sax_namespace_handler.c
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

#include "konoco_sax_namespace_handler.h"
#include "konoco_sax_handle.h"
#include "konoco_sax.h"
#include "konoco_namespace_resolver.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#pragma mark Split Prefix 

typedef struct _qname {
	konoco_buffer prefix;
	konoco_buffer name;
} qname;

int
split_prefix(qname * result, konoco_buffer * input)
{
	void * split_position = memchr(input->data, ':', input->length);
	if (split_position == 0) {
		result->prefix.length = 0;
		result->name.data = input->data;
		result->name.length = input->length;
		return (0);
	} else {
		result->prefix.data = input->data;
		result->prefix.length = (unsigned char *)split_position - input->data;
		
		result->name.data = (unsigned char *)split_position + 1;
		result->name.length = input->length - 1 - ((unsigned char *)split_position - input->data);
		return (1);
	};
}

void
append_attribute(parser_handle * parser, konoco_buffer * prefix, konoco_buffer * name, konoco_buffer * value)
{
	attribute_list * attr = malloc(sizeof(attribute_list));
	assert(attr);
	
	konoco_buffer_init_cpy(&attr->prefix, prefix);
	konoco_buffer_init_cpy(&attr->name, name);
	konoco_buffer_init_cpy(&attr->value, value);
	attr->next = parser->ns_delegate_data.attributes;
	parser->ns_delegate_data.attributes = attr;
}

void
free_attribute_list(attribute_list * item)
{
	if (item->next)
		free_attribute_list(item->next);
	konoco_buffer_free(&item->prefix);
	konoco_buffer_free(&item->name);
	konoco_buffer_free(&item->value);
	free(item);
}

void
free_attributes(parser_handle * parser)
{
	if (parser->ns_delegate_data.attributes == 0)
		return;
	free_attribute_list(parser->ns_delegate_data.attributes);
	parser->ns_delegate_data.attributes = 0;
}

void
call_cb_for_attr_item(parser_handle * parser, attribute_list * item, konoco_sax_delegate * delegate)
{
	konoco_buffer * ns = konoco_namespace_resolver_get(parser->ns_resolver, &item->prefix);
	(delegate->attribute)(parser,
						  delegate->data,
						  &item->name,
						  ns,
						  &item->value);
	if (item->next)
		call_cb_for_attr_item(parser, item->next, delegate);
}

#pragma mark Callbacks for the NS Delegate

void
ns_delegate_start_document(void * parser, void * data)
{
	ns_delegate_data * ns_data = (ns_delegate_data *)data;
	
	if (ns_data->orig_delegate->start_document) {
		(ns_data->orig_delegate->start_document)(parser,
												 ns_data->orig_delegate->data);
	};
}

void
ns_delegate_end_document(void * parser, void * data)
{
	ns_delegate_data * ns_data = (ns_delegate_data *)data;
	
	if (ns_data->orig_delegate->end_document) {
		(ns_data->orig_delegate->end_document)(parser,
											   ns_data->orig_delegate->data);
	};
}

void
ns_delegate_characters(void * parser, void * data, konoco_buffer * text)
{
	ns_delegate_data * ns_data = (ns_delegate_data *)data;
	
	if (ns_data->orig_delegate->characters) {
		(ns_data->orig_delegate->characters)(parser,
											 ns_data->orig_delegate->data,
											 text);
	};
}

void
ns_delegate_error(void * parser, void * data, const char * msg)
{
	ns_delegate_data * ns_data = (ns_delegate_data *)data;
	
	if (ns_data->orig_delegate->error) {
		(ns_data->orig_delegate->error)(parser,
										ns_data->orig_delegate->data,
										msg);
	};
}

#pragma mark Special Callback Function for NS Handling

void
ns_delegate_start_element(void * p, void * data, konoco_buffer * name, konoco_buffer * namespace)
{
	parser_handle * parser = (parser_handle *)p;
	ns_delegate_data * ns_data = (ns_delegate_data *)data;
	
	parser->ns_resolver = konoco_namespace_resolver_push(parser->ns_resolver);
	
	qname qname;
	split_prefix(&qname, name);
	
	konoco_buffer_cpy(&ns_data->element_prefix, &qname.prefix);
	konoco_buffer_cpy(&ns_data->element_name, &qname.name);
	// split qname 
	// set prefix and name to ns_delegate_data
}

void
ns_delegate_end_element(void * p, void * data, konoco_buffer * name, konoco_buffer * namespace)
{
	parser_handle * parser = (parser_handle *)p;
	ns_delegate_data * ns_data = (ns_delegate_data *)data;
	
	
	// call delegate
	if (ns_data->orig_delegate->end_element) {
		// split qname
		qname qname;
		split_prefix(&qname, name);
		
		// lookup ns
		konoco_buffer * ns = konoco_namespace_resolver_get(parser->ns_resolver, &qname.prefix);
		
		(ns_data->orig_delegate->end_element)(parser,
											  ns_data->orig_delegate->data,
											  &qname.name,
											  ns);
	};
	
	// pop ns resolver
	parser->ns_resolver = konoco_namespace_resolver_pop(parser->ns_resolver);
}

void
ns_delegate_attribute(void * p, void * data, konoco_buffer * name, konoco_buffer * namespace, konoco_buffer * value)
{
	parser_handle * parser = (parser_handle *)p;
	ns_delegate_data * ns_data = (ns_delegate_data *)data;
	
	qname qname;
	
	if (split_prefix(&qname, name)){
		if (konoco_buffer_eq_str(&qname.prefix, "xmlns")) {
			assert(parser->ns_resolver);
			konoco_namespace_resolver_set(parser->ns_resolver, &qname.name, value);
			if (ns_data->orig_delegate->xmlns) {
				(ns_data->orig_delegate->xmlns)(parser,
												ns_data->orig_delegate->data,
												&qname.name,
												value);
			};
		} else {
			append_attribute(parser, &qname.prefix, &qname.name, value);
		}
	} else {
		if (konoco_buffer_eq_str(&qname.name, "xmlns")) {
			assert(parser->ns_resolver);
			konoco_namespace_resolver_set(parser->ns_resolver, &qname.prefix, value);
			if (ns_data->orig_delegate->xmlns) {
				(ns_data->orig_delegate->xmlns)(parser,
												ns_data->orig_delegate->data,
												&qname.prefix,
												value);
			};
		} else {
			append_attribute(parser, &qname.prefix, &qname.name, value);
		};
	};
	
	// split qname
	// if prefix is not 'xmlns', append attribute at the list
	// else set prefix/namespace pair
}

void
ns_delegate_flush(void * p, void * data)
{
	parser_handle * parser = (parser_handle *)p;
	ns_delegate_data * ns_data = (ns_delegate_data *)data;
	
	if (ns_data->orig_delegate->start_element) {
		konoco_buffer * ns = konoco_namespace_resolver_get(parser->ns_resolver, &ns_data->element_prefix);
		(ns_data->orig_delegate->start_element)(parser,
												ns_data->orig_delegate->data,
												&ns_data->element_name,
												ns);
	};
	
	if (ns_data->orig_delegate->attribute && ns_data->attributes) {
		call_cb_for_attr_item(parser, ns_data->attributes, ns_data->orig_delegate);
	};
	
	free_attributes(parser);
	
	// lookup ns for element and call element callback
	// lookup ns for attributes and call callback
	// cleanup
}

#pragma mark Inject the NS Delegate

void
inject_namespace_delegate(parser_handle * parser)
{
	parser->ns_delegate_data.orig_delegate = parser->delegate;
	
	parser->ns_delegate.data = &parser->ns_delegate_data;
	parser->ns_delegate.start_document = &ns_delegate_start_document;
	parser->ns_delegate.end_document = &ns_delegate_end_document;
	parser->ns_delegate.start_element = &ns_delegate_start_element;
	parser->ns_delegate.end_element = &ns_delegate_end_element;
	parser->ns_delegate.attribute = &ns_delegate_attribute;
	parser->ns_delegate.characters = &ns_delegate_characters;
	parser->ns_delegate.error = &ns_delegate_error;
	parser->ns_delegate._flush = &ns_delegate_flush;
	
	parser->delegate = &parser->ns_delegate;
}

