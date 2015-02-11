#ifdef FIM_WANT_INLINE_HELP

fim_var_help_db[FIM_VID_ARCHIVE_FILES]= "[in] Regular expression matching filenames to be treated as (multipage) archives. If empty, \"" FIM_CNS_ARCHIVE_RE "\" will be used. Within each archive, only filenames matching the regular expression in the " FIM_VID_PUSHDIR_RE " variable will be considered for opening." ;
fim_var_help_db[FIM_VID_RANDOM]= "[out] a pseudorandom number" ;
fim_var_help_db[FIM_VID_BINARY_DISPLAY]= "[in] will force loading of the specified files as pixelmaps (no image decoding will be performed); if 1, using one bit per pixel;  if 24, using 24 bits per pixel; otherwise will load and decode the files as usual" ;
fim_var_help_db[FIM_VID_TEXT_DISPLAY]= "[in] will force loading of the specified files as text files (no image decoding will be performed); if 1; otherwise will load and decode the files as usual" ;
fim_var_help_db[FIM_VID_CACHE_STATUS]= "[out] string with current information on cache status" ;
fim_var_help_db[FIM_VID_DISPLAY_CONSOLE]= "[in] if 1, will display the output console" ;
fim_var_help_db[FIM_VID_DEVICE_DRIVER]= "[out] the current display device string" ;
fim_var_help_db[FIM_VID_DISPLAY_STATUS]= "[in] if 1, will display the status bar" ;
fim_var_help_db[FIM_VID_DISPLAY_STATUS_FMT]= "[in] custom (status bar) info format string, displayed in the lower left corner; if unset: full pathname; otherwise a custom format string specified with the same format of _info_fmt_str. " ;
fim_var_help_db[FIM_VID_PUSH_PUSHES_DIRS]= "[in] if 1, the push command will also accept and push directories (using pushdir)" ;
fim_var_help_db[FIM_VID_SANITY_CHECK]= "[in,experimental] if 1, will execute a sanity check on startup" ;
fim_var_help_db[FIM_VID_LAST_SYSTEM_OUTPUT]= "[out,experimental] the standard output of the last call to the system command" ;
fim_var_help_db[FIM_VID_LOAD_DEFAULT_ETC_FIMRC]= "[in] if 1 at startup, will load the system wide initialization file" ;
fim_var_help_db[FIM_VID_DEFAULT_ETC_FIMRC]= "[in] string with the global configuration file name" ;
fim_var_help_db[FIM_VID_FILE_LOADER]= "[in] if not empty, this string will force a file loader (among the ones listed in the -V switch output); [out] i:" FIM_VID_FILE_LOADER " stores the loader of the current image" ;
fim_var_help_db[FIM_VID_RETRY_LOADER_PROBE]= "[in] if 1 and user specified a file loader and this fails, will probe for a different loader" ;
fim_var_help_db[FIM_VID_NO_RC_FILE]= "[in] if 1, the ~/.fimrc file will not be loaded at startup" ;
fim_var_help_db[FIM_VID_NO_EXTERNAL_LOADERS]= "[in] if 1, no external loading programs will be tried for piping in an unsupported type image file" ;
fim_var_help_db[FIM_VID_SCRIPTOUT_FILE]= "[in] the name of the file to write to when recording sessions" ;
fim_var_help_db[FIM_VID_PUSHDIR_RE]= "[in] regular expression to match against when pushing files from a directory or an archive. By default this is \"" FIM_CNS_PUSHDIR_RE "\"." ;
fim_var_help_db[FIM_VID_STATUS_LINE]= "[in] if 1, will display the status bar" ;
fim_var_help_db[FIM_VID_WANT_PREFETCH]= "[in] if 1, will prefetch further files just after display of the first file" ;
fim_var_help_db[FIM_VID_WANT_SLEEPS]= "[in] number of seconds of sleep during slideshow mode" ;
fim_var_help_db[FIM_VID_WANT_EXIF_ORIENTATION]= "[in] if 1, will reorient images using information from EXIF metadata (and stored in in " FIM_VID_EXIF_ORIENTATION ", " FIM_VID_EXIF_MIRRORED ", " FIM_VID_EXIF_FLIPPED" )." ;
fim_var_help_db[FIM_VID_EXIF_ORIENTATION]= "[out] orientation information in the same format of " FIM_VID_ORIENTATION ", read from the EXIF tags of a given image (i:)." ;
fim_var_help_db[FIM_VID_EXIF_MIRRORED]= "[out] mirroring information, read from the EXIF tags of a given image (i:)." ;
fim_var_help_db[FIM_VID_EXIF_FLIPPED]= "[out] flipping information, read from the EXIF tags of a given image (i:)." ;
fim_var_help_db[FIM_VID_AUTOTOP]= "[in] if 1, will align to the top freshly loaded images" ;
fim_var_help_db[FIM_VID_SCALE_STYLE]= "[in] if non empty, this string will be fed to the scale command" ;
fim_var_help_db[FIM_VID_FILEINDEX]= "[out] the current image numeric index" ;
fim_var_help_db[FIM_VID_LASTFILEINDEX]= "[out] the last visited image numeric index. Useful for jumping back and forth easily between two images with 'goto _lastfileindex'." ;
fim_var_help_db[FIM_VID_FILELISTLEN]= "[out] the length of the current image list" ;
fim_var_help_db[FIM_VID_INFO_FMT_STR]= "[in] custom (status bar) info format string, displayed in the lower right corner; may contain ordinary text and special 'expando' sequences. These are: %p for current scale, in percentage; %w for width; %h for height; %i for image index in list; %l for image list length; %L for flip/mirror/orientation information; %P for page information; %F for file size; %M for screen image memory size; %m for memory used by mipmap; %C for memory used by cache; %T for total memory used (approximation); %n for the current file path name; %N for the current file path name basename; ; %c for centering information; %v for the fim program/version identifier string; %% for an ordinary %. A sequence like %?VAR?EXP? expands to EXP if i:VAR is set; EXP will be copied verbatim except for contained sequences of the form %:VAR:, which will be expanded to the value of variable i:VAR; this is meant to be used like in e.g. 'EXIF_DateTimeOriginal?[%:EXIF_DateTimeOriginal:]?', where an EXIF-set variable (make sure you have libexif for this) will be used only if present." ;
fim_var_help_db[FIM_VID_FILENAME]= "[out] the current file name string" ;
fim_var_help_db[FIM_VID_FIM_DEFAULT_CONFIG_FILE_CONTENTS]= "[out] the contents of the default (hardcoded) configuration file (executed after the minimal hardcoded config)" ;
fim_var_help_db[FIM_VID_FIM_DEFAULT_GRAMMAR_FILE_CONTENTS]= "[out] the contents of the default (hardcoded) grammar file" ;
fim_var_help_db[FIM_VID_FRESH]= "[in,out,experimental] 1 if the image was loaded, before all autocommands execution" ;
fim_var_help_db[FIM_VID_PAGE]= "[out,experimental] the current page" ;
fim_var_help_db[FIM_VID_PAGES]= "[out,experimental] the current number of pages of an image (i:) " ;
fim_var_help_db[FIM_VID_PAGECOUNT]= "[out,experimental] the page count for a given image" ;
fim_var_help_db[FIM_VID_OVERRIDE_DISPLAY]= "[internal] if 1, will inhibit display" ;
fim_var_help_db[FIM_VID_MAX_ITERATED_COMMANDS]= "[experimental] the iteration limit for N in \"N[commandname]\" iterated command invocations" ;
fim_var_help_db[FIM_VID_WANT_CAPTION_STATUS]= "[in] this works only if supported by the display device (currently only SDL). if set to a number that is not 0, will show the status (or command) line in the window manager caption; if set to a non-empty string, will interpret that string file info format string (see _info_fmt_str for the format); if empty, will show the program version." ;
fim_var_help_db[FIM_VID_MAGNIFY_FACTOR]= "[in] the image scale multiplier used when magnifying images size" ;
fim_var_help_db[FIM_VID_PWD]= "[out] the current working directory; will be updated at startup and whenever the working directory changes" ;
fim_var_help_db[FIM_VID_REDUCE_FACTOR]= "[in] the image scale multiplier used when reducing images size" ;
fim_var_help_db[FIM_VID_SCALE_FACTOR_MULTIPLIER]= "[in] value used for scaling up/down the scaling factors" ;
fim_var_help_db[FIM_VID_SCALE_FACTOR_DELTA]= "[in] value used for incrementing/decrementing the scaling factors" ;
fim_var_help_db[FIM_VID_COMMENT]= "[out] the image comment, extracted from the image file (if any)" ;
fim_var_help_db[FIM_VID_COMMENT_OI]= "[experimental,in] if v:_comment_over_image is set not to 0, will display the contents of i:_comment over the image." ;
fim_var_help_db[FIM_VID_EXIFTOOL_COMMENT]= "[out] comment extracted via the exiftool interface; see _use_exiftool." ;
fim_var_help_db[FIM_VID_STEPS]= "[in] the default steps, in pixels, when panning images" ;
fim_var_help_db[FIM_VID_VERSION]= "[out] fim version number; may be used for keeping compatibility of fim scripts across evolving versions."  ;
fim_var_help_db[FIM_VID_FBFONT]= "[out] The current console font file string. If the internal hardcoded font has been used, then its value is \"" FIM_DEFAULT_HARDCODEDFONT_STRING "\"." ;
fim_var_help_db[FIM_VID_HSTEPS]= "[in] the default steps, in pixels, when panning images horizontally (overrides steps)" ;
fim_var_help_db[FIM_VID_VSTEPS]= "[in] the default steps, in pixels, when panning images vertically (overrides steps)" ;
fim_var_help_db[FIM_VID_CONSOLE_ROWS]= "[in] if >0, will set the number of displayed text lines in the console" ;
fim_var_help_db[FIM_VID_CONSOLE_LINE_WIDTH]= "[in] if>0, will force the output console text width" ;
fim_var_help_db[FIM_VID_CONSOLE_LINE_OFFSET]= "[in,out] position of the text beginning in the output console, expressed in lines" ;
fim_var_help_db[FIM_VID_CONSOLE_BUFFER_LINES]= "[out] the number of buffered output console text lines" ;
fim_var_help_db[FIM_VID_CONSOLE_BUFFER_TOTAL]= "[out] amount of memory allocated for the output console buffer" ;
fim_var_help_db[FIM_VID_CONSOLE_BUFFER_FREE]= "[out] amount of unused memory in the output console buffer" ;
fim_var_help_db[FIM_VID_CONSOLE_BUFFER_USED]= "[out] amount of used memory in the output console buffer" ;
fim_var_help_db[FIM_VID_VERBOSE_KEYS]= "[in] if 1, after each interactive mode key hit, the console will display the hit key raw keycode" ;
fim_var_help_db[FIM_VID_CMD_EXPANSION]= "[in] if 1, will enable autocompletion (on execution) of alias and command strings" ;
fim_var_help_db[FIM_VID_VERBOSE_ERRORS]= "[in] if 1, will display on stdout internal errors, while parsing commands" ;
fim_var_help_db[FIM_VID_VERBOSITY]= "[in,experimental] program verbosity" ;
fim_var_help_db[FIM_VID_CONSOLE_KEY]= "[in] the key binding (an integer variable) for spawning the command line; will have precedence over any other binding" ;
fim_var_help_db[FIM_VID_IGNORECASE]= "[in] if 1, will allow for case insensitive regexp-based match in autocmd's; " ;
fim_var_help_db[FIM_VID_RE_SEARCH_OPTS]= "[in] affects regexp-based searches; if an empty string, defaults will apply; if contains 'i' ('I'), case insensitive (sensitive) searches will occur; if contains 'b', will match on basename, if contains 'f' on full pathname. " ;
fim_var_help_db[FIM_VID_SAVE_FIM_HISTORY]= "[in] if 1 on exit, will save the " FIM_CNS_HIST_COMPLETE_FILENAME " file on exit" ;
fim_var_help_db[FIM_VID_LOAD_FIM_HISTORY]= "[in] if 1 on startup, will load the " FIM_CNS_HIST_COMPLETE_FILENAME " file on startup" ;
fim_var_help_db[FIM_VID_TERM]= "[out] the environment TERM variable" ;
fim_var_help_db[FIM_VID_NO_DEFAULT_CONFIGURATION]= "[in] if 0, a default, hardcoded configuration will be executed at startup, after the minimal hardcoded one. " ;
fim_var_help_db[FIM_VID_DISPLAY_STATUS_BAR]= "[in] if 1, will display the status bar" ;
fim_var_help_db[FIM_VID_DISPLAY_BUSY]= "[in] if 1, will display a message on the status bar when processing" ;
fim_var_help_db[FIM_VID_WANT_MIPMAPS]= "[in] if >0, will use mipmaps to speed up downscaling of images (this has a memory overhead equivalent to one image copy); mipmaps will not be cached. " ;
fim_var_help_db[FIM_VID_EXIFTOOL]= "[in] if >0 and supported, exiftool will be used to get additional information. if 1, this will be appened to _comment; if 2, will go to _exiftool_comment" ;
fim_var_help_db[FIM_VID_SCALE]= "[in] the scale of the current image" ;
fim_var_help_db[FIM_VID_ASCALE]= "[in,out] the asymmetric scaling of the current image" ;
fim_var_help_db[FIM_VID_ANGLE]= "[in,out] a floating point number specifying the rotation angle, in degrees" ;
fim_var_help_db[FIM_VID_ORIENTATION]= "[internal] Orthogonal clockwise rotation (orientation) is controlled by: 'i:_orientation', 'v:_orientation', 'g:_orientation' and applied on a per-image basis. In particular, the values of the three variables are summed up and the sum is interpreted as the image orientation.  If the sum is 0, no rotation will apply; if it is 1, a single ( 90') rotation will apply; if it is 2, a double (180') rotation will apply; if it is 3, a triple (270') rotation will apply.  If the sum is not one of 0,1,2,3, the value of the sum modulo 4 is considered.  Therefore, \":i:_orientation=1\" and \":i:_orientation=5\" are equivalent: they rotate the image one time by 90'." ;
fim_var_help_db[FIM_VID_WIDTH]= "[out] the current image original width" ;
fim_var_help_db[FIM_VID_HEIGHT]= "[out] the current image original height" ;
fim_var_help_db[FIM_VID_SWIDTH]= "[out] the current image scaled width" ;
fim_var_help_db[FIM_VID_SHEIGHT]= "[out] the current image scaled height" ;
fim_var_help_db[FIM_VID_AUTOFLIP]= "[in] if 1, will flip images by default" ;
fim_var_help_db[FIM_VID_AUTONEGATE]= "[in] if 1, will negate images by default" ;
fim_var_help_db[FIM_VID_AUTODESATURATE]= "[in] if 1, will desaturate images by default" ;
fim_var_help_db[FIM_VID_LAST_FILE_LOADER]= "[out] after each image load, " FIM_VID_LAST_FILE_LOADER " will be set to the last file loader used" ;
fim_var_help_db[FIM_VID_FLIPPED]= "[out] 1, if the image is flipped" ;
fim_var_help_db[FIM_VID_NEGATED]= "[out] 1, if the image is negated" ;
fim_var_help_db[FIM_VID_DESATURATED]= "[out] 1, if the image is desaturated" ;
fim_var_help_db[FIM_VID_FIM_BPP]= "[out] the bits per pixel count" ;
fim_var_help_db[FIM_VID_AUTOMIRROR]= "[in] if 1, will mirror images by default" ;
fim_var_help_db[FIM_VID_MIRRORED]= "[out] 1, if the image is mirrored " ;
fim_var_help_db[FIM_VID_WANT_AUTOCENTER]= "[in] if 1, the image will be displayed centered " ;
fim_var_help_db[FIM_VID_MAX_CACHED_IMAGES]= "[in,experimental] the maximum number of images after which evictions will be forced. Setting this to 0 (no limits) is ok provided _max_cached_memory is set meaningfully." ;
fim_var_help_db[FIM_VID_MAX_CACHED_MEMORY]= "[in,experimental] the maximum amount of memory (in KiB) at which images will be continued being added to the cache. Setting this to 0 (no limit) will lead to a crash (there is no protection currently)." ;
fim_var_help_db[FIM_VID_CACHED_IMAGES]= "[out] the number of images currently cached." ;
fim_var_help_db[FIM_VID_SCREEN_WIDTH]= "[out] the screen width"  ;
fim_var_help_db[FIM_VID_SCREEN_HEIGHT]= "[out] the screen height" ;
fim_var_help_db[FIM_VID_DBG_AUTOCMD_TRACE_STACK]= "[in] dump to stdout autocommands stack trace during their execution (for debugging purposes)" ;
fim_var_help_db[FIM_VID_DBG_COMMANDS]= "[in] print out each command before its execution (for debugging purposes)" ;
fim_var_help_db[FIM_VID_OPEN_OFFSET]= "[in,optional] offset (specified in bytes) used when opening a file; [out] i:" FIM_VID_OPEN_OFFSET " will be assigned to images opened at a nonzero offset " ;
fim_var_help_db[FIM_VID_OPEN_OFFSET_RETRY]= "[in,optional] number of adjacent bytes to probe in opening the file " ;
fim_var_help_db[FIM_VID_SEEK_MAGIC]= "[optional] will seek for a magic signature before opening a file (for now, use like this: fim -c '_seek_magic=MAGIC_STRING;push file_to_seek_in.ext' ) " ;
fim_var_help_db[FIM_VID_PREFERRED_RENDERING_WIDTH]= "[in,optional] if >0, bit based rendering will use this value for a default document width (instead of a default value) " ;
fim_var_help_db[FIM_VID_PREFERRED_RENDERING_DPI]= "[in,optional] if >0, pdf, ps, djvu rendering will use this value for a default document dpi (instead of a default value) " ;
fim_var_help_db[FIM_VID_PRELOAD_CHECKS]= "[in,experimental] if 1 (default), will check with stat() existence of input files before push'ing them (set this to 0 to speed up loading very long file lists; in these cases a trailing slash (/) will have to be used to tell fim a pathname is a directory). This only works after initialization (thus, after command line files have been push'ed); use --no-stat-push if you wish to set this to 0 at command line files specification " ;
#endif /* FIM_WANT_INLINE_HELP */
