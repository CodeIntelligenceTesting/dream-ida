/*******************************************************************************
 * Copyright (C) 2011-2017 Khaled Yakdan.
 * All rights reserved.
 ******************************************************************************/
#pragma once

#include <memory>

class TypeBase;
typedef std::shared_ptr<TypeBase> TypePtr;
	class TopType;
	typedef std::shared_ptr<TopType> TopTypePtr;
	class NoType;
	typedef std::shared_ptr<NoType> NoTypePtr;
	class UnresolvedNamedType;
	typedef std::shared_ptr<UnresolvedNamedType> UnresolvedNamedTypePtr;
	class SimpleType;
	typedef std::shared_ptr<SimpleType> SimpleTypePtr;
		class PointerType;
		typedef std::shared_ptr<PointerType> PointerTypePtr;
		class IntegralType;
		typedef std::shared_ptr<IntegralType> IntegralTypePtr;
	class ComposedType;
	typedef std::shared_ptr<ComposedType> ComposedTypePtr;
		class ArrayType;
		typedef std::shared_ptr<ArrayType> ArrayTypePtr;
		class ClassType;
		typedef std::shared_ptr<ClassType> ClassTypePtr;
		class UnionType;
		typedef std::shared_ptr<UnionType> UnionTypePtr;
//
class TypeMap;
typedef std::shared_ptr<TypeMap> TypeMapPtr;
