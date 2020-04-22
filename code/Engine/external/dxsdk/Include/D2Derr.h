/*=========================================================================*\

    Copyright (c) Microsoft Corporation.  All rights reserved.

\*=========================================================================*/

#pragma once

/*=========================================================================*\
    D2D Status Codes
\*=========================================================================*/

#define FACILITY_D2D 0x899

#define MAKE_D2DHR( sev, code )\
    MAKE_HRESULT( sev, FACILITY_D2D, (code) )

#define MAKE_D2DHR_ERR( code )\
    MAKE_D2DHR( 1, code )

