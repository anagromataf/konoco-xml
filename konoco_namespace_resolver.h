/*
 *  konoco_namespace_resolver.h
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

#ifndef _konoco_namespace_resolver_h_
#define _konoco_namespace_resolver_h_

#include "konoco_xml.h"
#include "konoco_buffer.h"

/** This function creates a new namesapce resolver and
 *	sets the given resolver as its parent. If the given
 *	resolver is NULL, the new resolver is the root.
 */
void *
konoco_namespace_resolver_push(void * resolver);

/** This function destroies the given and retuns
 *	the parent resover. If the given resolver is
 *	the root resover, NULL is returned.
 *
 *  It is save to call this function with NULL.
 *	In this case NULL is returned.
 */
void *
konoco_namespace_resolver_pop(void * resolver);

/** This function sets mapping from prefix to namespace in the given 
 *	resolver. An exsisting mapping with the same prefix is overwritten.
 *
 *  It is save to call this function with a NULL as resolver.
 */
void
konoco_namespace_resolver_set(void * resolver, konoco_buffer * prefix, konoco_buffer * ns);

/** This function searches the resolver and its parents for the
 *	given prefix. If a mapping is found, a pointer to the buffer
 *	with the namespace is returned.
 *
 *	The returned buffer is owned by the namespace resolver and
 *	can be modified or destroyed by an other function.
 */
konoco_buffer *
konoco_namespace_resolver_get(void * resolver, konoco_buffer * prefix);

#endif // _konoco_namespace_resolver_h_
