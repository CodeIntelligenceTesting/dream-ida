////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011-2017 Khaled Yakdan.
// All rights reserved.
////////////////////////////////////////////////////////////////////////////////
//#include "houseKeepingUtilities.h"
//
//template <typename T>
//void deleteVectorElements(std::vector<T*>* vecPtr){
//	while(!vecPtr->empty()){
//		delete vecPtr->back();
//		vecPtr->pop_back();
//	}
//}
//
//template <typename T>
//void deleteVectorPointer(std::vector<T*>* vecPtr){
//	deleteVectorElements(vecPtr);
//	delete vecPtr;
//}
//
//template <typename T>
//void deleteSetPointer(std::set<T*>* setPtr){
//	std::set<T*>::iterator it = setPtr->begin();
//	while(it != setPtr->end()){
//		delete *it;
//		it = setPtr->erase(it);
//	}
//	delete setPtr;
//}
//
//template <typename T, typename U>
//void deleteMapToPointers(std::map<T, U*>* mapPtr){
//	std::map<T, U*>::iterator it = mapPtr->begin();
//	while(it != mapPtr->end()){
//		delete it->second;
//		it = mapPtr->erase(it);
//	}
//	delete mapPtr;
//}
