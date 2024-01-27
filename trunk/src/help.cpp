#ifdef FIM_WANT_INLINE_HELP

fim_var_help_db[FIM_VID_ARCHIVE_FILES]= "[in,g:] If non-empty, a regular expression matching the filenames to be treated as archives (multipage files). If empty, \"" FIM_CNS_ARCHIVE_RE "\" is used. Within each archive, only filenames matching the regular expression in the " FIM_VID_PUSHDIR_RE " variable are considered for loading." ;
fim_var_help_db[FIM_VID_RANDOM]= "[out] a pseudorandom number." ;
fim_var_help_db[FIM_VID_BINARY_DISPLAY]= "[in,g:] force loading files as pixelmaps (no image decoding is performed); if 1, using one bit per pixel; if 24, using 24 bits per pixel. If empty, load and decode the files as usual." ;
fim_var_help_db[FIM_VID_TEXT_DISPLAY]= "[in,g:] if 1, force loading specified files as text files (no image decoding is performed); otherwise load and decode the files as usual." ;
fim_var_help_db[FIM_VID_CACHE_CONTROL]= "[in,g:] string for cache control. If it starts with 'm', cache mipmaps; if it starts with 'M' then do not not cache them. Otherwise defaults apply." ;
fim_var_help_db[FIM_VID_CACHE_STATUS]= "[out,g:] string with current information on cache status." ;
fim_var_help_db[FIM_VID_DISPLAY_CONSOLE]= "[in,g:] if 1, display the output console." ;
fim_var_help_db[FIM_VID_DEVICE_DRIVER]= "[out,g:] the current display device string." ;
fim_var_help_db[FIM_VID_DISPLAY_STATUS]= "[in,g:] if 1, display display the status bar." ;
fim_var_help_db[FIM_VID_DISPLAY_STATUS_FMT]= "[in,g:] custom info format string, displayed in the lower left corner of the status bar; if unset: full pathname; otherwise a custom format string specified just as " FIM_VID_INFO_FMT_STR"." ;
fim_var_help_db[FIM_VID_PUSH_PUSHES_DIRS]= "[in,g:] if 1, the push command also accepts and pushes directories (using pushdir). if 2, also push hidden files/directories, that is, ones whose names begin with a dot (.). " ;
fim_var_help_db[FIM_VID_SANITY_CHECK]= "[in,experimental,g:] if 1, execute a sanity check on startup." ;
fim_var_help_db[FIM_VID_LAST_SYSTEM_OUTPUT]= "[out,experimental,g:] the standard output of the last call to the system command." ;
fim_var_help_db[FIM_VID_LAST_CMD_OUTPUT]= "[out,experimental,g:] the last command output." ;
fim_var_help_db[FIM_VID_LOAD_DEFAULT_ETC_FIMRC]= "[in,g:] if 1 at startup, load the system wide initialization file." ;
fim_var_help_db[FIM_VID_LOAD_HIDDEN_DIRS]= "[in,g:] if not 1, when pushing directories/files, those with name beginning with a dot (.) are skipped." ;
fim_var_help_db[FIM_VID_DEFAULT_ETC_FIMRC]= "[in,g:] string with the global configuration file name." ;
fim_var_help_db[FIM_VID_IMAGE_LOAD_TIME]= "[out,i:] time taken to load the file and decode the image, in seconds." ;
fim_var_help_db[FIM_VID_FILE_LOADER]= "[in,i:,g:] specify a file loader to use (among the ones listed in the -V switch output); [out] i:" FIM_VID_FILE_LOADER " stores the loader of the current image." ;
fim_var_help_db[FIM_VID_FILE_BUFFERED_FROM]= "[out,i:] if an image has been temporarily converted and decoded from a temporary file, its name is here." ;
fim_var_help_db[FIM_VID_EXTERNAL_CONVERTER]= "[out,i:] if an image has been decoded via an external program, its name is here." ;
fim_var_help_db[FIM_VID_RETRY_LOADER_PROBE]= "[in,g:] if set to 1 and a user-specified file loader fails, probe for a different loader." ;
fim_var_help_db[FIM_VID_NO_RC_FILE]= "[in,g:] if 1, do not load the ~/.fimrc configuration file at startup." ;
fim_var_help_db[FIM_VID_NO_EXTERNAL_LOADERS]= "[in,g:] if 1, do not attempt using external programs to decode a file of an unknown format." ;
fim_var_help_db[FIM_VID_SCRIPTOUT_FILE]= "[in,g:] the name of the file to write to when recording sessions." ;
fim_var_help_db[FIM_VID_PUSHDIR_RE]= "[in] regular expression to match against when pushing files from a directory or an archive. By default this is \"" FIM_CNS_PUSHDIR_RE "\"." ;
fim_var_help_db[FIM_VID_STATUS_LINE]= "[in,g:] if 1, display the status bar." ;
fim_var_help_db[FIM_VID_WANT_PREFETCH]= "[in,g:] if 1, prefetch further files just after displaying the first file; if 2 (and configured with --enable-cxx11) load in the background." ;
fim_var_help_db[FIM_VID_LOADING_IN_BACKGROUND]= "[out,g:] 1 if program has been invoked with --" FIM_OSW_BGREC " and still loading in background. " ;
fim_var_help_db[FIM_VID_RESIZE_HUGE_ON_LOAD]= "[in,g:] if 1, downscale at load time huge images (that is, ones exceeding by " FIM_CNS_HUGE_IMG_TO_VIEWPORT_PROPORTION_S " times the screen size)." ;
fim_var_help_db[FIM_VID_WANT_SLEEPS]= "[in,g:] number of seconds of sleep during slideshow mode." ;
fim_var_help_db[FIM_VID_WANT_EXIF_ORIENTATION]= "[in,g:] if 1, reorient images using information from EXIF metadata (and stored in in " FIM_VID_EXIF_ORIENTATION ", " FIM_VID_EXIF_MIRRORED ", " FIM_VID_EXIF_FLIPPED" )." ;
fim_var_help_db[FIM_VID_EXIF_ORIENTATION]= "[out,i:] orientation information in the same format of " FIM_VID_ORIENTATION ", read from the orientation EXIF tags (i:EXIF_Orientation)." ;
fim_var_help_db[FIM_VID_EXIF_MIRRORED]= "[out,i:] mirroring information, read from the EXIF tags of a given image." ;
fim_var_help_db[FIM_VID_EXIF_FLIPPED]= "[out,i:] flipping information, read from the EXIF tags of a given image." ;
fim_var_help_db[FIM_VID_AUTOTOP]= "[in,g:] if 1, align to the top freshly loaded images." ;
fim_var_help_db[FIM_VID_SCALE_STYLE]= "[in,g:] if non empty, pass it to the " FIM_VID_SCALE " command; see its documentation for possible values." ;
fim_var_help_db[FIM_VID_FILEINDEX]= "[out,g:] the current image numeric index." ;
fim_var_help_db[FIM_VID_LOOP_ONCE]= "[internal,g:] if 1 and doing a --slideshow, do it once." ;
fim_var_help_db[FIM_VID_LASTFILEINDEX]= "[out,g:] the last visited image numeric index. Useful for jumping back and forth easily between two images with 'goto _lastfileindex'." ;
fim_var_help_db[FIM_VID_LASTGOTODIRECTION]= "[out,g:] the last file goto direction (either string '+1' or string '-1')." ;
fim_var_help_db[FIM_VID_FILELISTLEN]= "[out,g:] current image list length (number of visible images)." ;
fim_var_help_db[FIM_VID_INFO_FMT_STR]= "[in,g:] custom info format string, displayed in the lower right corner of the status bar; may contain ordinary text and special 'expando' sequences. These are: %p for current scale, in percentage; %w for width; %h for height; %i for image index in list; %k for the value of i:_comment (comment description) variable in square brackets; %l for current image list length; %L for flip/mirror/orientation information; %P for page information; %F for file size; %M for screen image memory size; %m for memory used by mipmap; %C for memory used by cache; %T for total memory used (approximation); %R for total max memory used (as detected by getrusage()); %n for the current file path name; %N for the current file path name basename; ; %c for centering information; %v for the fim program/version identifier string; %% for an ordinary %. A sequence like %?VAR?EXP? expands to EXP if i:VAR is set; EXP is copied verbatim except for contained sequences of the form %:VAR:, which expand to the value of variable i:VAR; this is meant to be used like in e.g. '%?EXIF_DateTimeOriginal?[%:EXIF_DateTimeOriginal:]?', where the EXIF-set variable EXIF_DateTimeOriginal (make sure you have libexif for this) are used only if present." ;
fim_var_help_db[FIM_VID_FILENAME]= "[out,i:] the current file name string." ;
fim_var_help_db[FIM_VID_FIM_DEFAULT_CONFIG_FILE_CONTENTS]= "[out,g:] the contents of the default (hardcoded) configuration file (executed after the minimal hardcoded config)." ;
fim_var_help_db[FIM_VID_FIM_DEFAULT_GRAMMAR_FILE_CONTENTS]= "[out,g:] the contents of the default (hardcoded) grammar file." ;
fim_var_help_db[FIM_VID_FRESH]= "[in,out,i:,experimental] 1 if the image was loaded, before all autocommands (autocmd) execution." ;
fim_var_help_db[FIM_VID_PAGE]= "[out,experimental,g:] the current page." ;
fim_var_help_db[FIM_VID_PAGES]= "[out,experimental,i:] the current number of pages of an image." ;
fim_var_help_db[FIM_VID_OVERRIDE_DISPLAY]= "[internal,g:] if 1, inhibit display." ;
fim_var_help_db[FIM_VID_MAX_ITERATED_COMMANDS]= "[g:] the iteration limit for N in \"N[commandname]\" iterated command invocations." ;
fim_var_help_db[FIM_VID_WANT_CAPTION_STATUS]= "[in,g:] this works only if supported by the display device (currently only SDL). If set to a number that is not 0, show the status (or command) line in the window manager caption; if set to a non-empty string, interpret it just as a file info format string (see " FIM_VID_INFO_FMT_STR "); if empty, show the program version." ;
fim_var_help_db[FIM_VID_WANT_MOUSE_CTRL]= "[in,g:] if at least 9 chars long, enable mouse click/movement behaviour when in SDL mode; the 9 chars correspond to a 3x3 screen clickable grid and the equivalent command keys; clicking middle or right button toggle on-screen usage info." ;
fim_var_help_db[FIM_VID_MAGNIFY_FACTOR]= "[in,g:] the image scale multiplier used when magnifying images size." ;
fim_var_help_db[FIM_VID_PWD]= "[out,g:] the current working directory; variable updated at startup and whenever the working directory changes." ;
fim_var_help_db[FIM_VID_REDUCE_FACTOR]= "[in,g:] the image scale multiplier used when reducing images size." ;
fim_var_help_db[FIM_VID_SCALE_FACTOR_MULTIPLIER]= "[in,g:] value used for scaling up/down the scaling factors." ;
fim_var_help_db[FIM_VID_SCALE_FACTOR_DELTA]= "[in,g:] value used for incrementing/decrementing the scaling factors." ;
fim_var_help_db[FIM_VID_COMMENT]= "[i:,out] the image comment, extracted from the image file (if any)." ;
fim_var_help_db[FIM_VID_COMMENT_OI]= "[in,g:] if set to a value different than 0, display a custom comment string specified according to the value of " FIM_VID_COMMENT_OI_FMT "; if larger than 1, with black background; if 3, draw above the image. Occupies at most half of the screen." ;
fim_var_help_db[FIM_VID_COMMENT_OI_FMT]= "[in,g:] custom info format string, displayed in a caption over the image; if unset: i:_comment; otherwise a custom format string specified just as " FIM_VID_INFO_FMT_STR"." ;
fim_var_help_db[FIM_VID_EXIFTOOL_COMMENT]= "[out,g:] comment extracted via the exiftool interface; see " FIM_VID_EXIFTOOL "." ;
fim_var_help_db[FIM_VID_STEPS]= "[in,g:] the default steps, in pixels, when panning images." ;
fim_var_help_db[FIM_VID_PREAD]= "[in,g:] a user-specified shell command emitting an image on stdout, in a format readable by the convert utility. If the current filename matches \"" FIM_CNS_PIPEABLE_PATH_RE "\", substitute it to any occurrence of '{}'." ;
fim_var_help_db[FIM_VID_VERSION]= "[out,g:] fim version number; may be used for keeping compatibility of fim scripts across evolving versions."  ;
fim_var_help_db[FIM_VID_FBFONT]= "[out,g:] The current console font file string. If the internal hardcoded font has been used, then its value is \"" FIM_DEFAULT_HARDCODEDFONT_STRING "\"." ;
fim_var_help_db[FIM_VID_FBFMF]= "[in,g:] For the rendered text use a font magnified by this (integer) factor. Maximal value is \"" FIM_XSTRINGIFY(FIM_FONT_MAGNIFY_FACTOR_MAX) "\". (Only enabled if configured with --with-font-magnifying-factor=FACTOR, with FACTOR<1)." ;
fim_var_help_db[FIM_VID_FBAFS]= "[in,g:] Scale the rendered text to at least this (integer) fraction of the screen. Disable font autoscaling with -1. (Only enabled if configured with --with-font-magnifying-factor=FACTOR, with FACTOR<1)." ;
fim_var_help_db[FIM_VID_FB_VERBOSITY]= "[in,g:] if > 0, verbose font loading" ;
fim_var_help_db[FIM_VID_HSTEPS]= "[in,g:] the default steps, in pixels, when panning images horizontally (overrides steps)." ;
fim_var_help_db[FIM_VID_VSTEPS]= "[in,g:] the default steps, in pixels, when panning images vertically (overrides steps)." ;
fim_var_help_db[FIM_VID_CONSOLE_ROWS]= "[in,g:] if >0, set the number of text lines in the console to be displayed ." ;
fim_var_help_db[FIM_VID_CONSOLE_LINE_WIDTH]= "[in,g:] if>0, force the output console text width." ;
fim_var_help_db[FIM_VID_CONSOLE_LINE_OFFSET]= "[in,out,g:] position of the text beginning in the output console, expressed in lines." ;
fim_var_help_db[FIM_VID_CONSOLE_BUFFER_LINES]= "[out,g:] the number of buffered output console text lines." ;
fim_var_help_db[FIM_VID_CONSOLE_BUFFER_TOTAL]= "[out,g:] amount of memory allocated for the output console buffer." ;
fim_var_help_db[FIM_VID_CONSOLE_BUFFER_FREE]= "[out,g:] amount of unused memory in the output console buffer." ;
fim_var_help_db[FIM_VID_CONSOLE_BUFFER_USED]= "[out,g:] amount of used memory in the output console buffer." ;
fim_var_help_db[FIM_VID_VERBOSE_KEYS]= "[in,g:] if 1, display on the console the raw keycode of each key hit interactively." ;
fim_var_help_db[FIM_VID_CMD_EXPANSION]= "[in,g:] if 1, enable autocompletion (on execution) of alias and command strings." ;
fim_var_help_db[FIM_VID_VERBOSE_ERRORS]= "[in,g:] if 1, display on stdout internal errors, while parsing commands." ;
fim_var_help_db[FIM_VID_VERBOSITY]= "[in,experimental,g:] program verbosity." ;
fim_var_help_db[FIM_VID_CONSOLE_KEY]= "[in,g:] the key bound (an integer variable) to spawn the command line; overrides any other binding." ;
fim_var_help_db[FIM_VID_IGNORECASE]= "[in,g:] if 1, allow for case-insensitive regexp-based match in autocommands (autocmd)." ;
fim_var_help_db[FIM_VID_RE_SEARCH_OPTS]= "[in,g:] affects regexp-based searches; if an empty string, defaults apply; if it contains 'i' ('I'), case insensitive (sensitive) searches occur; if it contains 'b', match on basename, if contains 'f' on full pathname; if it contains 'D', match on description." ;
fim_var_help_db[FIM_VID_SAVE_FIM_HISTORY]= "[in,g:] if 1 on exit, save the " FIM_CNS_HIST_COMPLETE_FILENAME " file on exit." ;
fim_var_help_db[FIM_VID_LOAD_FIM_HISTORY]= "[in,g:] if 1 on startup, load the " FIM_CNS_HIST_COMPLETE_FILENAME " file on startup." ;
fim_var_help_db[FIM_VID_TERM]= "[out,g:] the environment TERM variable." ;
fim_var_help_db[FIM_VID_NO_DEFAULT_CONFIGURATION]= "[in,g:] if 0, a default, hardcoded configuration is loaded at startup, after the minimal hardcoded one." ;
fim_var_help_db[FIM_VID_DISPLAY_STATUS_BAR]= "[in,g:] if 1, display the status bar." ;
fim_var_help_db[FIM_VID_DISPLAY_BUSY]= "[in,g:] if 1, display a message on the status bar when processing." ;
fim_var_help_db[FIM_VID_WANT_MIPMAPS]= "[in,g:] if >0, use mipmaps to speed up downscaling of images (this has a memory overhead equivalent to one image copy); mipmaps are not cached. If 2, use every fourth source pixel instead of averaging (good for photos, not for graphs)." ;
fim_var_help_db[FIM_VID_EXIFTOOL]= "[in,g:] if >0 and supported, use exiftool to get additional information. If 1, append this information to _comment; if 2, to " FIM_VID_EXIFTOOL_COMMENT "." ;
fim_var_help_db[FIM_VID_SCALE]= "[in,i:] the scale of the current image." ;
fim_var_help_db[FIM_VID_ASCALE]= "[in,out,i:] the asymmetric scaling of the current image." ;
fim_var_help_db[FIM_VID_ANGLE]= "[in,out,i:] a floating point number specifying the rotation angle, in degrees." ;
fim_var_help_db[FIM_VID_ORIENTATION]= "[internal,i:] Orthogonal clockwise rotation (orientation) is controlled by: 'i:" FIM_VID_ORIENTATION "', 'g:" FIM_VID_ORIENTATION "' and applied on a per-image basis. In particular, the values of the three variables are summed up and the sum is interpreted as the image orientation.  If the sum is 0, no rotation applies; if it is 1, a single ( 90') rotation applies; if it is 2, a double (180') rotation applies; if it is 3, a triple (270') rotation applies.  If the sum is not one of 0,1,2,3, the value of the sum modulo 4 is considered.  Therefore, \":i:" FIM_VID_ORIENTATION "=1\" and \":i:" FIM_VID_ORIENTATION "=5\" are equivalent: they rotate the image one time by 90'." ;
fim_var_help_db[FIM_VID_WIDTH]= "[out,i:] the current image original width." ;
fim_var_help_db[FIM_VID_HEIGHT]= "[out,i:] the current image original height." ;
fim_var_help_db[FIM_VID_SWIDTH]= "[out,i:] the current image scaled width." ;
fim_var_help_db[FIM_VID_SHEIGHT]= "[out,i:] the current image scaled height." ;
fim_var_help_db[FIM_VID_AUTOFLIP]= "[in,g:] if 1, flip all images." ;
fim_var_help_db[FIM_VID_AUTONEGATE]= "[in,g:] if 1, negate all images." ;
fim_var_help_db[FIM_VID_AUTODESATURATE]= "[in,g:] if 1, desaturate all images." ;
fim_var_help_db[FIM_VID_LAST_FILE_LOADER]= "[out,g:] set to the name of the last file loader used." ;
fim_var_help_db[FIM_VID_ALL_FILE_LOADERS]= "[out,g:] space-separated list of hardcoded file loaders usable with " FIM_VID_FILE_LOADER "." ;
fim_var_help_db[FIM_VID_FLIPPED]= "[out,i:] 1, if the image is flipped." ;
fim_var_help_db[FIM_VID_NEGATED]= "[out,i:] 1, if the image is negated." ;
fim_var_help_db[FIM_VID_DESATURATED]= "[out,i:] 1, if the image is desaturated." ;
fim_var_help_db[FIM_VID_FIM_BPP]= "[out,g:] the bits per pixel count." ;
fim_var_help_db[FIM_VID_AUTOMIRROR]= "[in,g:] if 1, mirror all images." ;
fim_var_help_db[FIM_VID_MIRRORED]= "[out,i:] 1, if the image is mirrored." ;
fim_var_help_db[FIM_VID_WANT_AUTOCENTER]= "[in,g:] if 1, center the image when displayed." ;
fim_var_help_db[FIM_VID_MAX_CACHED_IMAGES]= "[in,g:] the maximum number of images after which forcing evictions. Setting this to 0 (no limits) is ok provided " FIM_VID_MAX_CACHED_MEMORY " is set meaningfully." ;
fim_var_help_db[FIM_VID_MIN_CACHED_IMAGES]= "[in,g:] the minimum number of images to keep from eviction; if less than four can lead to inefficiencies: e.g. when jumping between two images, each time an erase and a prefetch of neighboring images would trigger. default value is 4." ;
fim_var_help_db[FIM_VID_MAX_CACHED_MEMORY]= "[in,g:] the maximum amount of memory (in KiB) at which images continue being added to the cache. Setting this to 0 (no limit) leads to a crash (there is no protection currently)." ;
fim_var_help_db[FIM_VID_CACHED_IMAGES]= "[out,g:] the number of images currently cached." ;
fim_var_help_db[FIM_VID_SCREEN_WIDTH]= "[out,g:] the screen width." ;
fim_var_help_db[FIM_VID_SCREEN_HEIGHT]= "[out] the screen height." ;
fim_var_help_db[FIM_VID_SKIP_SCROLL]= "[int,g:] if >1, fraction of page to skip when scrolling (e.g. 'scrollforward'); if 1, auto chosen; if <1, disabled." ;
fim_var_help_db[FIM_VID_DBG_AUTOCMD_TRACE_STACK]= "[in,g:] dump to stdout autocommands (autocmd) stack trace during their execution (for debugging purposes)." ;
fim_var_help_db[FIM_VID_DBG_COMMANDS]= "[in,g:] debugging option string for printing out . if containing 'a', print out autocmd info; if containing 'c', print out each command; if containing 'k', print out each pressed key; if containing 'j', print interpreter internal steps; if containing 'B', clear screen and print background loading files; if containing 'C', print cache activity; if containing 'm', gtk mode menus tooltips will contain also specification strings; if containing 'mm', gtk mode menu building will also be very verbose." ;
fim_var_help_db[FIM_VID_OPEN_OFFSET_L]= "[in,optional,g:,i:] offset (specified in bytes) used when opening a file; [out] i:" FIM_VID_OPEN_OFFSET_L " is assigned to images opened at a nonzero offset." ;
fim_var_help_db[FIM_VID_OPEN_OFFSET_U]= "[in,optional,g:] number of adjacent bytes to probe in opening the file." ;
fim_var_help_db[FIM_VID_SEEK_MAGIC]= "[optional,g:] seek a 'magic' signature in the file after opening it, and try decoding it starting within the range of that signature (use like this: fim -C '" FIM_VID_SEEK_MAGIC "=MAGIC_STRING;push filename')." ;
fim_var_help_db[FIM_VID_PREFERRED_RENDERING_WIDTH]= "[in,optional,g:] if >0, bit-based (see " FIM_VID_BINARY_DISPLAY ") rendering uses this value for a default document width (instead of a default value)." ;
fim_var_help_db[FIM_VID_PREFERRED_RENDERING_DPI]= "[in,optional,g:] if >0, rendering of pdf, ps, djvu use this value for a default document dpi; if unset, use an internal default value." ;
fim_var_help_db[FIM_VID_PRELOAD_CHECKS]= "[in,experimental,g:] if 1 (default), check with stat() the existence of input files before push'ing them (set this to 0 to speed up loading very long file lists; in these cases a trailing slash (/) must be used to tell fim a pathname is a directory). This only works after initialization (thus, after command line files have been push'ed); use --" FIM_OSW_NO_STAT_PUSH " if you wish to set this to 0 at command line files specification." ;
#endif /* FIM_WANT_INLINE_HELP */
