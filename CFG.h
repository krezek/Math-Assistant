#ifndef _CFG_H_
#define _CFG_H_

//#define _PKG

#define APP_NAME "Mathematic Assistant"
#define APP_VERSION "Mathematic Assistant\nVersion 15.1\n\n\nBy REZEK\nhttps://krezek.github.io"

#ifdef _PKG
#define DB_URL "\\ProgramData\\rezek\\SAC\\books.db"
#define SHADER_PATH L"\\ProgramData\\rezek\\SAC\\"
#else
#define DB_URL "..\\data\\books.db"
#define SHADER_PATH L"..\\resources\\output\\"
#endif


#endif /* _CFG_H_ */
