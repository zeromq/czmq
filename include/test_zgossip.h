/*  =========================================================================
    test_zgossip - 

    Copyright (c) 1991-2012 iMatix Corporation -- http://www.imatix.com     
    Copyright other contributors as noted in the AUTHORS file.              
                                                                            
    This file is part of Zyre, an open-source framework for proximity-based 
    peer-to-peer applications -- See http://zyre.org.                       
                                                                            
    This is free software; you can redistribute it and/or modify it under   
    the terms of the GNU Lesser General Public License as published by the  
    Free Software Foundation; either version 3 of the License, or (at your  
    option) any later version.                                              
                                                                            
    This software is distributed in the hope that it will be useful, but    
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTA-   
    BILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General  
    Public License for more details.                                        
                                                                            
    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see http://www.gnu.org/licenses/.      
    =========================================================================
*/

#ifndef __TEST_ZGOSSIP_H_INCLUDED__
#define __TEST_ZGOSSIP_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif


//  @interface
//  Create a new test_zgossip
CZMQ_EXPORT test_zgossip_t *
    test_zgossip_new (void);

//  Destroy the test_zgossip
CZMQ_EXPORT void
    test_zgossip_destroy (test_zgossip_t **self_p);

//  Print properties of object
CZMQ_EXPORT void
    test_zgossip_print (test_zgossip_t *self);

//  Self test of this class
CZMQ_EXPORT void
    test_zgossip_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
