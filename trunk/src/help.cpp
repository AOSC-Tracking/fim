#ifdef FIM_WANT_INLINE_HELP

fim_var_help_db[FIM_VID_RANDOM]= "[internal,out] a pseudorandom number" ;
fim_var_help_db[FIM_VID_BINARY_DISPLAY]= "[internal,in] will force loading of the specified files as pixelmaps (no image decoding will be performed); if 1, using one bit per pixel;  if 24, using 24 bits per pixel; otherwise will load and decode the files as usual" ;
fim_var_help_db[FIM_VID_TEXT_DISPLAY]= "[internal,in] will force loading of the specified files as text files (no image decoding will be performed); if 1; otherwise will load and decode the files as usual" ;
fim_var_help_db[FIM_VID_CACHE_STATUS]= "[internal,out] string with current information on cache status" ;
fim_var_help_db[FIM_VID_DISPLAY_CONSOLE]= "[internal,in] if 1, will display the output console" ;
fim_var_help_db[FIM_VID_DEVICE_DRIVER]= "[internal,out] the current display device string" ;
fim_var_help_db[FIM_VID_DISPLAY_STATUS]= "[internal,in] if 1, will display the status bar" ;
fim_var_help_db[FIM_VID_DISPLAY_STATUS_FMT]= "[internal,in,experimental] format of display status string; if 0, full pathname; if 1, basename. " ;
fim_var_help_db[FIM_VID_PUSH_PUSHES_DIRS]= "[internal,in] if 1, the push command will also accept and push directories (using pushdir)" ;
fim_var_help_db[FIM_VID_SANITY_CHECK]= "[internal,in,experimental] if 1, will execute a sanity check on startup" ;
fim_var_help_db[FIM_VID_LAST_SYSTEM_OUTPUT]= "[internal,out,experimental] the standard output of the last call to the system command" ;
fim_var_help_db[FIM_VID_LOAD_DEFAULT_ETC_FIMRC]= "[internal,in] if 1 at startup, will load the system wide initialization file" ;
fim_var_help_db[FIM_VID_DEFAULT_ETC_FIMRC]= "[internal,in] string with the global configuration file name" ;
fim_var_help_db[FIM_VID_FILE_LOADER]= "[in] if not empty, this string will force a file loader (among the ones listed in the -V switch output)" ;
fim_var_help_db[FIM_VID_RETRY_LOADER_PROBE]= "[in] if 1 and user specified a file loader and this fails, will probe for a different loader" ;
fim_var_help_db[FIM_VID_NO_RC_FILE]= "[internal,in] if 1, the ~/.fimrc file will not be loaded at startup" ;
fim_var_help_db[FIM_VID_NO_EXTERNAL_LOADERS]= "[internal,in] if 1, no external loading programs will be tried for piping in an unsupported type image file" ;
fim_var_help_db[FIM_VID_SCRIPTOUT_FILE]= "[internal,in] the name of the file to write to when recording sessions" ;
fim_var_help_db[FIM_VID_PUSHDIR_RE]= "[internal,in] regular expression to match against when pushing files from a directory" ;
fim_var_help_db[FIM_VID_STATUS_LINE]= "[internal,in] if 1, will display the status bar" ;
fim_var_help_db[FIM_VID_WANT_PREFETCH]= "[internal,in] if 1, will prefetch further files just after display of the first file" ;
fim_var_help_db[FIM_VID_WANT_SLEEPS]= "[internal,in] number of seconds of sleep during slideshow mode" ;
fim_var_help_db[FIM_VID_AUTOTOP]= "[internal,in] if 1, will align to the top freshly loaded images" ;
fim_var_help_db[FIM_VID_SCALE_STYLE]= "[internal,in] if non empty, this string will be fed to the scale command" ;
fim_var_help_db[FIM_VID_FILEINDEX]= "[internal,out] the current image numeric index" ;
fim_var_help_db[FIM_VID_FILELISTLEN]= "[internal,out] the length of the current image list" ;
fim_var_help_db[FIM_VID_INFO_FMT_STR]= "[internal,in] custom (status bar) file info format string; may contain ordinary text and special sequences; these are: %p for scale, in percentage; %w for width; %h for height; %i for image index in list; %l for image list length; %L for flip/mirror information; % for page information; %F for file size; %M for screen image memory size; %m for memory used by mipmap; %C for memory used by cache; %T for total memory used (approximation); %c for centering information; %% for an ordinary %. This feature has to be enabled at configure time." ;
fim_var_help_db[FIM_VID_FILENAME]= "[internal,out] the current file name string" ;
fim_var_help_db[FIM_VID_FIM_DEFAULT_CONFIG_FILE_CONTENTS]= "[internal,out] the contents of the default (hardcoded) configuration file (executed after the minimal hardcoded config)" ;
fim_var_help_db[FIM_VID_FIM_DEFAULT_GRAMMAR_FILE_CONTENTS]= "[internal,out] the contents of the default (hardcoded) grammar file" ;
fim_var_help_db[FIM_VID_FRESH]= "[internal,in,out,experimental] 1 if the image was loaded, before all autocommands execution" ;
fim_var_help_db[FIM_VID_PAGE]= "[internal,out,experimental] the current page" ;
fim_var_help_db[FIM_VID_PAGECOUNT]= "[internal,out,experimental] the page count for a given image" ;
fim_var_help_db[FIM_VID_OVERRIDE_DISPLAY]= "[internal] if 1, will inhibit display" ;
fim_var_help_db[FIM_VID_MAX_ITERATED_COMMANDS]= "[internal,experimental] the iteration limit for N in \"N[commandname]\" iterated command invocations" ;
fim_var_help_db[FIM_VID_WANT_CAPTION_STATUS]= "[internal,experimental] if not 0, and if supported by the display device, will show the status (or command) line in the window manager caption" ;
fim_var_help_db[FIM_VID_MAGNIFY_FACTOR]= "[internal,in] the image scale multiplier used when magnifying images size" ;
fim_var_help_db[FIM_VID_PWD]= "[internal,out] the current working directory; will be updated at startup and whenever the working directory changes" ;
fim_var_help_db[FIM_VID_REDUCE_FACTOR]= "[internal,in] the image scale multiplier used when reducing images size" ;
fim_var_help_db[FIM_VID_SCALE_FACTOR_MULTIPLIER]= "[internal,in] value used for scaling up/down the scaling factors" ;
fim_var_help_db[FIM_VID_SCALE_FACTOR_DELTA]= "[internal,in] value used for incrementing/decrementing the scaling factors" ;
fim_var_help_db[FIM_VID_COMMENT]= "[internal,out] the image comment, extracted from the image file (if any)" ;
fim_var_help_db[FIM_VID_STEPS]= "[internal,in] the default steps, in pixels, when panning images" ;
fim_var_help_db[FIM_VID_VERSION]= "[internal,out] fim version number; may be used for keeping compatibility of fim scripts across evolving versions."  ;
fim_var_help_db[FIM_VID_HSTEPS]= "[internal,in] the default steps, in pixels, when panning images horizontally (overrides steps)" ;
fim_var_help_db[FIM_VID_VSTEPS]= "[internal,in] the default steps, in pixels, when panning images vertically (overrides steps)" ;
fim_var_help_db[FIM_VID_CONSOLE_ROWS]= "[internal,in] if >0, will set the number of displayed text lines in the console" ;
fim_var_help_db[FIM_VID_CONSOLE_LINE_WIDTH]= "[internal,in] if>0, will force the output console text width" ;
fim_var_help_db[FIM_VID_CONSOLE_LINE_OFFSET]= "[internal,in,out] position of the text beginning in the output console, expressed in lines" ;
fim_var_help_db[FIM_VID_CONSOLE_BUFFER_LINES]= "[internal,out] the number of buffered output console text lines" ;
fim_var_help_db[FIM_VID_CONSOLE_BUFFER_TOTAL]= "[internal,out] amount of memory allocated for the output console buffer" ;
fim_var_help_db[FIM_VID_CONSOLE_BUFFER_FREE]= "[internal,out] amount of unused memory in the output console buffer" ;
fim_var_help_db[FIM_VID_CONSOLE_BUFFER_USED]= "[internal,out] amount of used memory in the output console buffer" ;
fim_var_help_db[FIM_VID_VERBOSE_KEYS]= "[internal,in] if 1, after each interactive mode key hit, the console will display the hit key raw keycode" ;
fim_var_help_db[FIM_VID_CMD_EXPANSION]= "[internal,in] if 1, will enable autocompletion (on execution) of alias and command strings" ;
fim_var_help_db[FIM_VID_VERBOSE_ERRORS]= "[internal,in] if 1, will display on stdout internal errors, while parsing commands" ;
fim_var_help_db[FIM_VID_VERBOSITY]= "[internal,in,experimental] program verbosity" ;
fim_var_help_db[FIM_VID_CONSOLE_KEY]= "[internal,in] the key binding (an integer variable) for spawning the command line; will have precedence over any other binding" ;
fim_var_help_db[FIM_VID_IGNORECASE]= "[internal,in] if 1, will allow for case insensitive regexp-based searches" ;
fim_var_help_db[FIM_VID_SAVE_FIM_HISTORY]= "[internal,in] if 1 on exit, will save the ~/fim_history file on exit" ;
fim_var_help_db[FIM_VID_LOAD_FIM_HISTORY]= "[internal,in] if 1 on startup, will load the ~/fim_history file on startup" ;
fim_var_help_db[FIM_VID_TERM]= "[internal,out] the environment TERM variable" ;
fim_var_help_db[FIM_VID_NO_DEFAULT_CONFIGURATION]= "[internal,in] if 0, a default, hardcoded configuration will be executed at startup, after the minimal hardcoded one. " ;
fim_var_help_db[FIM_VID_DISPLAY_STATUS_BAR]= "[internal,in] if 1, will display the status bar" ;
fim_var_help_db[FIM_VID_DISPLAY_BUSY]= "[internal,in] if 1, will display a message on the status bar when processing" ;
fim_var_help_db[FIM_VID_WANT_MIPMAPS]= "[internal,in] if >0, will compute mipmaps; if >1, will also use mipmaps to speed up downscaling of images" ;
fim_var_help_db[FIM_VID_SCALE]= "[internal,in] the scale of the current image" ;
fim_var_help_db[FIM_VID_ASCALE]= "[internal,in,out] the asymmetric scaling of the current image" ;
fim_var_help_db[FIM_VID_ANGLE]= "[internal,in,out] a floating point number specifying the rotation angle, in degrees" ;
fim_var_help_db[FIM_VID_ORIENTATION]= "[internal] Rotation is controlled by: 'i:_orientation', 'v:_orientation', 'g:_orientation' and applied with a per-image basis.  In particular, the values of the three variables are summed up and the sum is interpreted as the image orientation.  If the sum is 0, no rotation will apply; if it is 1, a single ( 90') rotation will apply; if it is 2, a double (180') rotation will apply; if it is 3, a triple (270') rotation will apply.  If the sum is not one of 0,1,2,3, the value of the sum modulo 4 is considered.  Therefore, \":i:_orientation=1\" and \":i:_orientation=5\" will do the same thing: rotate the image one time by 90'." ;
fim_var_help_db[FIM_VID_WIDTH]= "[internal,out] the current image original width" ;
fim_var_help_db[FIM_VID_HEIGHT]= "[internal,out] the current image original height" ;
fim_var_help_db[FIM_VID_SWIDTH]= "[internal,out] the current image scaled width" ;
fim_var_help_db[FIM_VID_SHEIGHT]= "[internal,out] the current image scaled height" ;
fim_var_help_db[FIM_VID_AUTOFLIP]= "[internal,in] if 1, will flip images by default" ;
fim_var_help_db[FIM_VID_AUTONEGATE]= "[internal,in] if 1, will negate images by default" ;
fim_var_help_db[FIM_VID_AUTODESATURATE]= "[internal,in] if 1, will desaturate images by default" ;
fim_var_help_db[FIM_VID_LAST_FILE_LOADER]= "[internal,out] string identifying the last file loader which has loaded an image" ;
fim_var_help_db[FIM_VID_FLIPPED]= "[internal,out] 1, if the image is flipped" ;
fim_var_help_db[FIM_VID_NEGATED]= "[internal,out] 1, if the image is negated" ;
fim_var_help_db[FIM_VID_DESATURATED]= "[internal,out] 1, if the image is desaturated" ;
fim_var_help_db[FIM_VID_FIM_BPP]= "[internal,out] the bits per pixel count" ;
fim_var_help_db[FIM_VID_AUTOMIRROR]= "[internal,in] if 1, will mirror images by default" ;
fim_var_help_db[FIM_VID_MIRRORED]= "[internal,out] 1, if the image is mirrored " ;
fim_var_help_db[FIM_VID_WANT_AUTOCENTER]= "[internal,in] if 1, the image will be displayed centered " ;
fim_var_help_db[FIM_VID_MAX_CACHED_IMAGES]= "[internal,in] the maximum number of images allowed in the cache" ;
fim_var_help_db[FIM_VID_MAX_CACHED_MEMORY]= "[internal,in] the maximum amount of memory allowed for the cache (in KiB)" ;
fim_var_help_db[FIM_VID_CACHED_IMAGES]= "[internal,out] the number of images currently cached" ;
fim_var_help_db[FIM_VID_SCREEN_WIDTH]= "[internal,out] the screen width"  ;
fim_var_help_db[FIM_VID_SCREEN_HEIGHT]= "[internal,out] the screen height" ;
fim_var_help_db[FIM_VID_DBG_AUTOCMD_TRACE_STACK]= "[internal,in] dump to stdout autocommands stack trace during their execution (for debugging purposes)" ;
fim_var_help_db[FIM_VID_DBG_COMMANDS]= "[internal,in] print out each command before its execution (for debugging purposes)" ;
fim_var_help_db[FIM_VID_OPEN_OFFSET]= "[internal,in,optional] offset (specified in bytes) used when opening a file " ;
fim_var_help_db[FIM_VID_SEEK_MAGIC]= "[internal,optional] will seek for a magic signature before opening a file (for now, use like this: fim -c '_seek_magic=MAGIC_STRING;push file_to_seek_in.ext' ) " ;
fim_var_help_db[FIM_VID_PREFERRED_RENDERING_WIDTH]= "[internal,in,optional] if >0, bit based rendering will use this value for a default document width (instead of a default value) " ;
fim_var_help_db[FIM_VID_PREFERRED_RENDERING_DPI]= "[internal,in,optional] if >0, pdf, ps, djvu rendering will use this value for a default document dpi (instead of a default value) " ;
#endif /* FIM_WANT_INLINE_HELP */
