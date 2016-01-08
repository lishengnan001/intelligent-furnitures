
/*
 * FILENAME: 
 *        config_etc_cs.c
 *
 * DESCRIPTION: 
 *        Central Server config file.
 *
 * ORIGINAL DATE:
 *        06/21/2012
 *
 * AUTHOR:
 *        Yuhui
 *
 * MODIFIED DATE:
 *
 * MODIFIED LOG:
 *
 * MODIFIED AUTHOR:
 *
 * New Feature
 * 
 * VERSION:
 *        Comment Version:      0.0.1
 *
 * COPYRIGHT
 *        Copyright (c) 12        Chinasoft International
 */

#ifndef CONFIG_ETC_CS_H
#define CONFIG_ETC_CS_H

//------------------------------------------------------------------------
// Etc Central Server.
//------------------------------------------------------------------------

#define IDSTRING "central_server 1.0"

#define DEBUG

#define ETC_HAS_CENTRAL_SERVER

//------------------------------------------------------------------------
// version
//------------------------------------------------------------------------

// Socket Interface Version
#define socket_server_version             "0.0.1"
#define socket_server_version_num         3.02
#define socket_server_major_version       3
#define socket_server_minor_version       2
#define socket_server_update_version      0
#define socket_server_major_version_str   "3"
#define socket_server_minor_version_str   "2"
#define socket_server_update_version_str  "0"

#endif // CONFIG_ETC_CS_H
