/*
 *  konoco_namespace_resolver.c
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

#include "konoco_namespace_resolver.h"

#include <stdlib.h>
#include <string.h>

typedef struct _ns_prefix_list {
	konoco_buffer prefix;
	konoco_buffer ns;
	struct _ns_prefix_list * next;
} ns_prefix_list;

typedef struct _resolver_handle {
	struct _resolver_handle * parent_resolver;
	ns_prefix_list * p_list;
} resolver_handle;

void free_prefix_list(ns_prefix_list * prefix_list);
void set_prefix_to_list(ns_prefix_list * prefix_list,
						konoco_buffer * prefix,
						konoco_buffer * ns);
konoco_buffer *
get_namespace_from_list(ns_prefix_list * prefix_list,
						konoco_buffer * prefix);

#pragma mark External Funktions

void *
konoco_namespace_resolver_push(void * r)
{
	resolver_handle * new_resolver = malloc(sizeof(resolver_handle));
	if (!new_resolver) {
		// could not allocate memory
		return (0);
	};

	new_resolver->p_list = 0;
	new_resolver->parent_resolver = r;

	return (new_resolver);
}

void *
konoco_namespace_resolver_pop(void * r)
{
	if (r == 0)
		return (0);
	
	resolver_handle * resolver = (resolver_handle *)r;
	resolver_handle * parent = resolver->parent_resolver;
	
	// free memory
	if (resolver->p_list != 0) {
		free_prefix_list(resolver->p_list);
	}
	free(resolver);
	
	return (parent);
}

void
konoco_namespace_resolver_set(void * r, konoco_buffer * prefix, konoco_buffer * ns)
{
	if (r == 0)
		return;
	
	resolver_handle * resolver = (resolver_handle *)r;
	
	if (resolver->p_list == 0) {
		// allocate a new struct
		resolver->p_list = malloc(sizeof(ns_prefix_list));
		resolver->p_list->next = 0;
		
		// allocate and copy prefix
		konoco_buffer_init(&resolver->p_list->prefix);
		konoco_buffer_cpy(&resolver->p_list->prefix, prefix);
		
		// allocate and copy namespace
		konoco_buffer_init(&resolver->p_list->ns);
		konoco_buffer_cpy(&resolver->p_list->ns, ns);
	} else {
		set_prefix_to_list(resolver->p_list, prefix, ns);
	};
}

konoco_buffer *
konoco_namespace_resolver_get(void * r, konoco_buffer * prefix)
{
	if (r == 0)
		return (0);
	
	resolver_handle * resolver = (resolver_handle *)r;
	
	konoco_buffer * result = 0;
	if (resolver->p_list != 0) {
		result = get_namespace_from_list(resolver->p_list, prefix);
	};
	
	if (result == 0 && resolver->parent_resolver != 0) {
		return (konoco_namespace_resolver_get(resolver->parent_resolver, prefix));
	};
	
	return (result);
}

#pragma mark Prefix List Handling

void free_prefix_list(ns_prefix_list * prefix_list)
{
	if (prefix_list->next != 0) {
		free_prefix_list(prefix_list->next);
	}
	konoco_buffer_free(&prefix_list->prefix);
	konoco_buffer_free(&prefix_list->ns);
	free(prefix_list);
}

void set_prefix_to_list(ns_prefix_list * prefix_list, konoco_buffer * prefix, konoco_buffer * ns)
{
	if (konoco_buffer_eq(&prefix_list->prefix, prefix)) {
		// same prefix
		konoco_buffer_cpy(&prefix_list->ns, ns);
	} else if (prefix_list->next == 0) {
		
		// allocate a new struct
		prefix_list->next = malloc(sizeof(ns_prefix_list));
		prefix_list->next->next = 0;
		
		// allocate and copy prefix
		konoco_buffer_init(&prefix_list->next->prefix);
		konoco_buffer_cpy(&prefix_list->next->prefix, prefix);
		
		// allocate and copy namespace
		konoco_buffer_init(&prefix_list->next->ns);
		konoco_buffer_cpy(&prefix_list->next->ns, ns);
	} else {
		set_prefix_to_list(prefix_list->next, prefix, ns);
	};
}

konoco_buffer *
get_namespace_from_list(ns_prefix_list * prefix_list, konoco_buffer * prefix)
{
	if (konoco_buffer_eq(&prefix_list->prefix, prefix)) {
		// same prefix
		return (&prefix_list->ns);
	} else if (prefix_list->next == 0) {
		return (0);
	} else {
		return (get_namespace_from_list(prefix_list->next, prefix));
	};
}


