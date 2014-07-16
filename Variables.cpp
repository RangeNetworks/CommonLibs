/*
* Copyright 2014 Range Networks, Inc.
* All Rights Reserved.
*
* This software is distributed under multiple licenses;
* see the COPYING file in the main directory for licensing
* information for this specific distribution.
*
* This use of this software may be subject to additional restrictions.
* See the LEGAL file in the main directory for details.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/


#include "UnixSignal.h"
#include "SelfDetect.h"

// These variables all have constructors, and their may be some interaction
// between them, so the order of construction is important.
UnixSignal gSigVec;
SelfDetect gSelf;
