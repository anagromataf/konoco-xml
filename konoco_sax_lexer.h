/*
 *  konoco_sax_lexer.h
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

#ifndef _konoco_sax_lexer_h_
#define _konoco_sax_lexer_h_

#include "konoco_sax.h"
#include "konoco_sax_handle.h"

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

enum lexer_token
get_next_token(parser_handle * parser);

#endif // _konoco_sax_lexer_h_
