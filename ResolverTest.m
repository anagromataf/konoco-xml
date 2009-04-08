//
//  ResolverTest.m
//  KonocoXML
//
//  Created by Tobias Kräntzer on 07.04.09.
//  Copyright 2009 Konoco. All rights reserved.
//

#import "ResolverTest.h"
#include "konoco_namespace_resolver.h"
#include "konoco_buffer.h"

@implementation ResolverTest

- (void) testPushPop
{
	void * resolver1;
	void * resolver2;
	void * resolver3;
	void * resolver4;
	
	resolver1 = konoco_namespace_resolver_push(0);
	STAssertTrue(resolver1 != 0, @"Push the NS Resolver Stack. The result is a pointer.");
	
	resolver2 = konoco_namespace_resolver_push(resolver1);
	STAssertTrue(resolver2 != 0 && resolver1 != resolver2, @"Push the NS Resolver Stack. The result is a pointer.");
	
	resolver3 = konoco_namespace_resolver_pop(resolver2);
	STAssertTrue(resolver3 != 0 && resolver3 == resolver1, @"Pop the NS Resolver Stack. We should get the previous resolver.");
	
	resolver4 = konoco_namespace_resolver_pop(resolver3);
	STAssertTrue(resolver4 == 0, @"Pop the NS Resolver. It was the last item on the stack - so we should get NULL");	
}

- (void) testSetGetNamespace
{
	void * resolver;
	resolver = konoco_namespace_resolver_push(0);
	
	konoco_buffer prefix, ns;
	konoco_buffer_init(&prefix);
	konoco_buffer_init(&ns);
	
	konoco_buffer_strcpy(&prefix, "ex");
	konoco_buffer_strcpy(&ns, "http://example.com/ex#");
	
	konoco_namespace_resolver_set(resolver, &prefix, &ns);
	
	konoco_buffer * result;
	result = konoco_namespace_resolver_get(resolver, &prefix);
	
	STAssertTrue(konoco_buffer_eq_str(result, "http://example.com/ex#"), @"Compare the namespace set to the resolver with the one returend.");
	
	resolver = konoco_namespace_resolver_pop(resolver);
}

- (void) testSetGetNamespaceAcquire
{
	void * resolver;
	resolver = konoco_namespace_resolver_push(0);
	
	konoco_buffer prefix, ns1, ns2;
	konoco_buffer * result;
	konoco_buffer_init(&prefix);
	konoco_buffer_init(&ns1);
	konoco_buffer_init(&ns2);
	
	konoco_buffer_strcpy(&prefix, "ex");
	konoco_buffer_strcpy(&ns1, "http://example.com/ex-1#");
	konoco_buffer_strcpy(&ns2, "http://example.com/ex-2#");
	
	konoco_namespace_resolver_set(resolver, &prefix, &ns1);
	result = konoco_namespace_resolver_get(resolver, &prefix);
	STAssertTrue(konoco_buffer_eq_str(result, "http://example.com/ex-1#"), @"Compare the namespace set to the resolver with the one returend.");

	resolver = konoco_namespace_resolver_push(resolver);
	
	konoco_namespace_resolver_set(resolver, &prefix, &ns2);
	result = konoco_namespace_resolver_get(resolver, &prefix);
	STAssertTrue(konoco_buffer_eq_str(result, "http://example.com/ex-2#"), @"Compare the namespace set to the resolver with the one returend.");
	
	resolver = konoco_namespace_resolver_pop(resolver);
	
	result = konoco_namespace_resolver_get(resolver, &prefix);
	STAssertTrue(konoco_buffer_eq_str(result, "http://example.com/ex-1#"), @"Compare the namespace set to the resolver with the one returend.");
	
	resolver = konoco_namespace_resolver_pop(resolver);
}

- (void) testUnknownPrefix
{
	void * resolver;
	resolver = konoco_namespace_resolver_push(0);
	
	konoco_buffer prefix, ns;
	konoco_buffer * result;
	konoco_buffer_init(&prefix);
	konoco_buffer_init(&ns);
	
	konoco_buffer_strcpy(&prefix, "ex");
	konoco_buffer_strcpy(&ns, "http://example.com/ex#");
	
	resolver = konoco_namespace_resolver_push(resolver);
	
	konoco_namespace_resolver_set(resolver, &prefix, &ns);
	result = konoco_namespace_resolver_get(resolver, &prefix);
	STAssertTrue(konoco_buffer_eq_str(result, "http://example.com/ex#"), @"Compare the namespace set to the resolver with the one returend.");
	
	resolver = konoco_namespace_resolver_pop(resolver);
	
	result = konoco_namespace_resolver_get(resolver, &prefix);
	STAssertTrue(result == 0, @"Compare the namespace set to the resolver with the one returend.");
	
	resolver = konoco_namespace_resolver_pop(resolver);
}

@end
