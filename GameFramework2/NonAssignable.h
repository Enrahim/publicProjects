#pragma once

/** Clever trick from http://stackoverflow.com/questions/6077143/disable-copy-constructor 
*	Just inherit from this class and you disable default copy operations
*	Should come at no extra performance cost with optimizing compiler */
class NonAssignable {
	/** Unimplemented. Should never be called - compiler error */
	NonAssignable(NonAssignable const& noCopy);
	/** Unimplemented. Should never be called - compiler error */
	NonAssignable operator=(NonAssignable const& noCopy);
public:
	NonAssignable() { ; }
};
