/**
 *  This file is a test driver, which attempts to replicate the dns and coo tests
 *  from main.cpp.
 *
 *  @author Geoffrey Meier
 */

#include "libpolylib.a"

//#include <libpolylib/iegenlib.h>
//#include <libpolylib/omega/OmegaLib.hpp>
#include <iostream>

using namespace iegenlib;
using namespace omglib;

/**
 * Drives the test (basically, just your standard main funciton)
 */
int main(int argc, char **argv) {

    // Example from File
    Set *set = new Set("{[a]:0<a}");
    
    // coo example
    Set *coo = new Set("{[n,i,j]:i-row(n)=0&&j-col(n)=0&&n>=0&&NNZ-1>=0&&-n+NNZ-1>=0}");
    // dns example
    Set *dns = new Set("{[i,j]:i>=0&&j>=0&&M-1>=0&&N-1>=0&&-i+N-1>=0&&-j+M-1>=0}");


    // Print sets to stdout
    std::cout << "set: " << set-> prettyPrintString() << std::endl;
    std::cout << "coo: " << coo-> prettyPrintString() << std::endl;
    std::cout << "dns: " << dns-> prettyPrintString() << std::endl;

    // dns example with relation
    iegenlib::Relation *rdns = new iegenlib::Relation("{[i,j]->[j,i]}");
    std::cout << "rdns " << rdns-> prettyPrintString() << std::endl;
    dns = rdns->Apply(dns);
    std::cout << "dns: "<< dns-> prettyPrintString() << std::endl;

//  Attempt to get Omega codegen working....
//    std::cout << OmegaLib::codegen(dns->prettyPrintString()) << std::endl;
    
    return 0;
    
}
