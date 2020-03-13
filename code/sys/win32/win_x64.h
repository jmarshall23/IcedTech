// win_64.h
//

// Various 64bit wrapper functions.
#undef  GetWindowLong
#undef  SetWindowLong
#undef SetClassLong
#undef LONG 

#define GWL_WNDPROC GWLP_WNDPROC
#define SetClassLong	SetClassLongPtr
#define GetWindowLong	GetWindowLongPtr
#define SetWindowLong	SetWindowLongPtr
#define GWL_USERDATA GWLP_USERDATA
#define GCL_HICON GCLP_HICON
#define DWL_MSGRESULT DWLP_MSGRESULT
#define DWL_DLGPROC DWLP_DLGPROC