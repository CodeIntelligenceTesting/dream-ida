////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#include "Variable.h"
#include "../../IntermediateRepresentation/Expression/Expression.h"
#include <boost/lexical_cast.hpp>

Variable::Variable()
	: name(""), subscript(NO_SUBSCRIPT)
{}

Variable::Variable(std::string _name, int _subscript)
	: name(_name), subscript(_subscript)
{
	resolved = true;
}

Variable::Variable(std::string _name, int _subscript, int _phiSourceBlockId)
	: name(_name), subscript(_subscript), phiSourceBlockId(_phiSourceBlockId)
{
	resolved = true;
}

Variable::~Variable(void)
{
}

std::string Variable::toString() const{
	return name + "_" + boost::lexical_cast<std::string>(subscript);
}

bool Variable::isNullVariable(){
	return name.empty() || subscript == NO_SUBSCRIPT;
}

bool Variable::isEqual(const Variable &toThis){
	return name.compare(toThis.name) == 0 && subscript == toThis.subscript;
}

bool Variable::doesRepresentSameVariable(const ExpressionPtr &exp) const{
	return name.compare(exp->getName()) == 0 && subscript == exp->subscript;
}
