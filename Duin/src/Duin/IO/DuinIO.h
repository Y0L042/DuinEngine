#pragma once

#include <string>

// Used SDL enums for standard user folders
typedef enum DN_Folder {
    DN_FOLDER_HOME,        /**< The folder which contains all of the current user's data, preferences, and documents. It usually contains most of the other folders. If a requested folder does not exist, the home folder can be considered a safe fallback to store a user's documents. */
    DN_FOLDER_DESKTOP,     /**< The folder of files that are displayed on the desktop. Note that the existence of a desktop folder does not guarantee that the system does show icons on its desktop; certain GNU/Linux distros with a graphical environment may not have desktop icons. */
    DN_FOLDER_DOCUMENTS,   /**< User document files, possibly application-specific. This is a good place to save a user's projects. */
    DN_FOLDER_DOWNLOADS,   /**< Standard folder for user files downloaded from the internet. */
    DN_FOLDER_MUSIC,       /**< Music files that can be played using a standard music player (mp3, ogg...). */
    DN_FOLDER_PICTURES,    /**< Image files that can be displayed using a standard viewer (png, jpg...). */
    DN_FOLDER_PUBLICSHARE, /**< Files that are meant to be shared with other users on the same computer. */
    DN_FOLDER_SAVEDGAMES,  /**< Save files for games. */
    DN_FOLDER_SCREENSHOTS, /**< Application screenshots. */
    DN_FOLDER_TEMPLATES,   /**< Template files to be used when the user requests the desktop environment to create a new file in a certain folder, such as "New Text File.txt".  Any file in the Templates folder can be used as a starting point for a new file. */
    DN_FOLDER_VIDEOS,      /**< Video files that can be played using a standard video player (mp4, webm...). */
    DN_FOLDER_COUNT        /**< Total number of types in this enum, not a folder type by itself. */
} DNL_Folder;

//namespace duin {
//	const std::string GetBasePath(); // Returns the base path of the application
//	const std::string GetUserFolder(DN_Folder folder); // Returns the path to a standard user folder
//	bool EnsureDirectory(const std::string& dirPath); // Creates a directory if it doesn't exist, returns true if successful or if directory exists
//}
