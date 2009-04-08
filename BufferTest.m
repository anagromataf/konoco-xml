//
//  BufferTest.m
//  KonocoXML
//
//  Created by Tobias Kräntzer on 07.04.09.
//  Copyright 2009 Konoco. All rights reserved.
//

#import "BufferTest.h"

#include "konoco_buffer.h"

@implementation BufferTest

- (void) testBuffer
{
	konoco_buffer a, b;
	konoco_buffer_init(&a);
	konoco_buffer_init(&b);
	
	konoco_buffer_strcpy(&a, "abc");
	konoco_buffer_strcpy(&b, "abc");
	
	STAssertTrue(konoco_buffer_eq_str(&a, "abc"), @"Compare two buffers.");
	
	STAssertTrue(konoco_buffer_eq(&a, &b), @"Compare two buffers.");
	
	konoco_buffer_strcpy(&a, "vuwx");
		
	STAssertTrue(konoco_buffer_eq(&a, &b) == 0, @"Compare two buffers.");
	
	konoco_buffer_strcpy(&b, "vuwxy");
	
	STAssertTrue(konoco_buffer_eq(&a, &b) == 0, @"Compare two buffers.");
	
	konoco_buffer_memcpy(&b, (void *)"vuwxy", 4);
	
	STAssertTrue(konoco_buffer_eq(&a, &b), @"Compare two buffers.");
	
	konoco_buffer_strcpy(&b, "vuwxy");
	konoco_buffer_cpy(&b, &a);
	
	STAssertTrue(konoco_buffer_eq(&a, &b), @"Compare two buffers.");
	
	konoco_buffer_free(&a);
	konoco_buffer_free(&b);
}

@end
