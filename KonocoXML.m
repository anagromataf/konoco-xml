/*
 *  main.c
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

#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include "konoco_sax.h"

void
delegate_start_document(void * parser, void * data)
{	
	printf("Start Document\n");
	printf("- xmlversion: ");
	switch (konoco_sax_get_version(parser)) {
		case konoco_xml_version_undefined:
			printf("undefined\n");
			break;
		case konoco_xml_version_1_0:
			printf("1.0\n");
			break;
		case konoco_xml_version_1_1:
			printf("1.1\n");
	};
	printf("- charset: %s\n", konoco_sax_get_charset(parser));
}

void
delegate_end_document(void * parser, void * data)
{
	*(int *)data = 0;
	printf("End Document\n");
}

void
delegate_characters(void * parser, void * data, konoco_buffer * text)
{
	printf("Characters: '%.*s'\n", text->length, text->data);
}

void
delegate_start_element(void * parser, void * data, konoco_buffer * name, konoco_buffer * namespace)
{
	printf("Start Element: '%.*s'",
		   name->length,
		   name->data);
	if (namespace) {
		printf(" '%.*s'\n",
			   namespace->length,
			   namespace->data);
	} else {
		printf("\n");
	};
}

void
delegate_end_element(void * parser, void * data, konoco_buffer * name, konoco_buffer * namespace)
{
	printf("End Element: '%.*s'",
		   name->length,
		   name->data);
	if (namespace) {
		printf(" '%.*s'\n",
			   namespace->length,
			   namespace->data);
	} else {
		printf("\n");
	};
}

void
delegate_attribute(void * parser, void * data, konoco_buffer * name, konoco_buffer * namespace, konoco_buffer * value)
{
	printf("Attribute: '%.*s'='%.*s'",
		   name->length, name->data,
		   value->length, value->data);
	if (namespace) {
		printf(" '%.*s'\n",
			   namespace->length,
			   namespace->data);
	} else {
		printf("\n");
	};
}

void
delegate_error(void * parser, void * data, const char * error)
{
	*(int *)data = 0;
	printf("Error: '%s'\n", error);
}

int main (int argc, const char * argv[]) {	
	if (argc == 2) {
		
		int running = 1;
		
		konoco_sax_delegate delegate;
		
		delegate.data = &running;
		delegate.start_document = &delegate_start_document;
		delegate.end_document = &delegate_end_document;
		delegate.start_element = &delegate_start_element;
		delegate.end_element = &delegate_end_element;
		delegate.attribute = &delegate_attribute;
		delegate.characters = &delegate_characters;
		delegate.error = &delegate_error;
		
		void * parser = konoco_sax_create(1024, 0);
		konoco_buffer * buffer = konoco_sax_get_buffer(parser);
		konoco_sax_set_delegate(parser, &delegate, RESOLVE_NAMESPACE);
		
		printf("@ Reading file: %s\n", argv[1]);
		
		int fd = open(argv[1], O_RDONLY);
		if (fd > 0) {
			int bytes_read = 0;
			do {
				bytes_read = read(fd, buffer->data, buffer->length);
				konoco_sax_parse(parser, bytes_read);
			} while (bytes_read > 0 && running);
		} else {
			printf("@ Error while openeing the file '%s'\n.", argv[1]);
		};
		
		(buffer->data)[0] = 0;
		buffer->length = 1;
		konoco_sax_parse(parser, 1);
		
		konoco_sax_destroy(parser);
	} else {
		printf("usage: KonocoSAX <xml file>\n");
	};
	
	return 0;
}

