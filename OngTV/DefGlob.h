////////////////////////////////////////////////////////////////////////////////
/// @addtogroup GLOBAL
/// @{
///
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// @file 
/// Definitions globales a la librairie de maintenance
///
///  \n \n <b>Copyright ADENEO 2005</b>
///
/// @author $Author: blavier $
/// @version $Revision: 82 $
/// @date $Date: 2014-05-09 17:12:45 +0200 (ven., 09 mai 2014) $
///
/// @if svn
///    URL: $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/DefGlob.h $
/// @endif
////////////////////////////////////////////////////////////////////////////////

// Pour se proteger de la multi-inclusion : NOM_MODULE_H doit être unique
#ifndef DEF_GLOB_H
#define DEF_GLOB_H

////////////////////////////////////////////////////////////////////////////////
//                                                                      Includes
////////////////////////////////////////////////////////////////////////////////
// Uniquement des includes projet : pas d'inclusion de fichier standard

////////////////////////////////////////////////////////////////////////////////
//                                                      Defines & Types exportes
////////////////////////////////////////////////////////////////////////////////

// Alias qui si il est utilise dans un path fait reference au path du fichier cnx
#define ALIAS_CNXPATH          std::string("$(CnxDir)")

// Alias qui si il est utilise dans un path fait reference au path de l'executable
#define ALIAS_EXEPATH          std::string("$(ExeDir)")

////////////////////////////////////////////////////////////////////////////////
// sections et clefs du fichier INI
////////////////////////////////////////////////////////////////////////////////
// section generale
#define GENERAL_SECTION          "GENERAL"
#define KEY_PROJECT              "PROJECT"
#define KEY_LOCAL_ADDR           "LOCAL_ADDR"
#define KEY_CACHE_ENABLE         "CACHE_ENABLE"
#define KEY_CACHE_PATH           "CACHE_PATH"
#define KEY_SRV_TIMEOUT          "SRV_TIMEOUT"
#define KEY_DISCONNECT_ON_BREAK  "DISCONNECT_ON_BREAK"
#define KEY_HOST_FILES_DIRECTORY "HOST_FILES_DIRECTORY"
#define KEY_SAVE_DIR_ON_EXIT     "SAVE_DIR_ON_EXIT"
#define KEY_DONT_USE_PUBLIC_MODULE  "DONT_USE_PUBLIC_MODULE"

// clefs pour les autorun
#define LUA_AUTORUN_SECTION         "LUA_AUTORUN"
#define KEY_LUA_AUTORUN_ON_OPEN     "LUA_SCRIPT_ON_OPEN"
#define KEY_LUA_AUTORUN_ON_CONNECT  "LUA_SCRIPT_ON_CONNECT"

// clefs pour le chargement des DLLs d'extension LUA
#define PREFIXE_LUA_DLL_SECTION     "LUA_DLL"
#define KEY_LUA_DLL_NAME            "NAME"
#define KEY_LUA_DLL_PATH            "DLL_PATH"

// clefs l'editeur LUA
#define LUA_EDITOR_SECTION          "LUA_EDITOR"
#define KEY_LUA_EDITOR_NB_CHARS_TO_TRIGGER_AUTOC "NB_CHARS_TO_TRIGGER_AUTOC"
#define KEY_LUA_EDITOR_MAX_LINES_IN_AUTOC_LIST   "MAX_LINES_IN_AUTOC_LIST"
#define KEY_LUA_EDITOR_MSK_HIGHLIGHTING "MSK_HIGHLIGHTING"

// section dans laquelle sont stockees les repertoires utilises pour les
// differents type de fichiers
#define DIR_SECTION                 "DIR"

// masques definissant les bits pour les mises en evidence
#define MSK_HIGHLIGTH_BRACES             0x01
#define MSK_HIGHLIGTH_SELECTED_WORD      0x02
#define MSK_HIGHLIGTH_STATEMENT          0x04

// par defaut 5 caracteres pour commencer l'autocompletion
#define DEFAULT_LUA_EDITOR_NB_CHARS_TO_TRIGGER_AUTOC      5
// par defaut 10 lignes max dans la liste pour l'autocompletion 
#define DEFAULT_LUA_EDITOR_MAX_LINES_IN_AUTOC_LIST       10
// par defaut, les mises en evidence sont actives
#define DEFAULT_LUA_EDITOR_MSK_HIGHLIGHTING (MSK_HIGHLIGTH_BRACES|MSK_HIGHLIGTH_SELECTED_WORD|MSK_HIGHLIGTH_STATEMENT)

////////////////////////////////////////////////////////////////////////////////
// rafraichissement de la console
////////////////////////////////////////////////////////////////////////////////
#define CONSOLE_REFRESH   1000     // 1000 ms
#define CONSOLE_TIMER_ID  1

#endif // DEF_GLOB_H

////////////////////////////////////////////////////////////////////////////////
// End of $URL: http://centaure/svn/ong-bootloaders/TRUNK/ONG/ValidationTool/OngTV/DefGlob.h $
////////////////////////////////////////////////////////////////////////////////

///
/// @}
///