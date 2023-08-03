//useful text
const char const TEXT_BOOL[2][6] = {"false","true "};
const char const TEXT_PROGRAM_LANG[1][10] = {"C-lang   "};
const char const TEXT_ROLLE_OUT[] = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

//setup text
const char const TEXT_SETUP_FAILED_USER_GET[]  = "Failed to get login user.\n";

//chooseList text
const char const TEXT_CHOOSE_LIST_TYPE_NUMBER[]  = "Please press your choise number.\n";
const char const TEXT_CHOOSE_LIST_RETYPE_NUMBER[]  = "Please retype number.\n";
const char const TEXT_CHOOSE_LIST_INFO_NUMBER[]  = "You choose %u\n\n";


//actionList text
const char const TEXT_ACTION_NAME_CREATE_PROJECT[]  = "Create project";
const char const TEXT_ACTION_NAME_OPEN_PROJECT[]    = "Open project";
const char const TEXT_ACTION_NAME_REMOVE_CONFIG[]   = "Remove config";
const char const TEXT_ACTION_BRIEF_CREATE_PROJECT[] = "Create new project for your application";
const char const TEXT_ACTION_BRIEF_OPEN_PROJECT[]   = "Open project for develop your application";
const char const TEXT_ACTION_BRIEF_REMOVE_CONFIG[]  = "Remove config.";

//removeConfig text
const char const TEXT_REMOVE_CONFIG_CONFIRMATION[]  = "Are you remove config?(yes/no)\n";
const char const TEXT_REMOVE_CONFIG_PLEASE_RETYPE[] = "Sorry please retype.\n";
const char const TEXT_REMOVE_CONFIG_FILED_REMOVE[]  = "Filed remove config.\nmay be not authority or any program open %s now.\n";

//createProject text
const char const TEXT_CREATE_PROFECT_INPUT_STATUS[]   = "--------------Project--------------\n\n   ProjectName     ... %s\n\n   ProjectBrief    ... %s"
                                                        "\n\n   ProjectLanguage ... %s\n\n   ProjectPath     ... %s \n\n   UseGithub       ... %s\n\n"
                                                        "-----------------------------------\n\n";

const char const TEXT_CREATE_PROFECT_INPUT_NAME[]     = "Please type project name.\n";
const char const TEXT_CREATE_PROFECT_INPUT_BRIEF[]    = "Please type project brief.\n";
const char const TEXT_CREATE_PROFECT_INPUT_PATH[]     = "Please type project path.\n";
const char const TEXT_CREATE_PROFECT_INPUT_GITHUB[]   = "Are you use github?(yes/no).\n";
const char const TEXT_CREATE_PROFECT_RETYPE_NAME[]    = "May be this name include invalid char.\nPlease retype project name.\n";
const char const TEXT_CREATE_PROFECT_RETYPE_PATH[]    = "May be this path invalid.\nPlease retype project path.\n";

const char const TEXT_CREATE_DIRCTORY_NEED_SUDDO[] = "need sudo for create %s\nexecute as sudo?(yes/no)\n";
const char const TEXT_DELETE_DIRCTORY_NEED_SUDDO[] = "need sudo for delete %s\nexecute as sudo?(yes/no)\n";

//remove project
extern const char const TEXT_REMOVE_PROJECT_SUCCESS[] = "Success remove project\n";
extern const char const TEXT_REMOVE_PROJECT_FAILED[]  = "Failed remove project\n";

extern const char const TEXT_NAME_OF_SRC_DIR[] = "/_src";
extern const char const TEXT_NAME_OF_INCLUDE_DIR[] = "/_include";

extern const char const TEXT_C_LANG_MAKE_FILE[] = "#コンパイラの指定\nCC     = gcc\n\n#コンパイルオプション\nCFLAGS =\n\n"
"#実行ファイル名\nTARGET = a\n\n#コンパイルするファイル\nSRCDIRS  = ./_src\n\nSRCS    = $(SRCDIRS:%%=%%/*.c)\n\n"
"#オブジェクトファイル\n_OBJS   = $(SRCS:%%.c=%%.o)\nOBJS   = $(notdir $(_OBJS))\n\n\n#ヘッダファイルの場所\n\n"
"INCDIR = -I _include\n\n#ライブラリのディレクトリ\nLIBDIR  = \n\n#追加するライブラリファイル\nLIBS    =\n\n "
"#ターゲットファイル生成\n$(TARGET): $(OBJS)\n	$(CC) -o $@ $^ $(LIBDIR) $(LIBS)\n\n#オブジェクトファイル生成\n"
"$(OBJS): $(SRCS)\n	$(CC) $(CFLAGS) $(INCDIR) -c $(SRCS)\n\n# (11)\"make all\"で make cleanとmakeを同時に実施。\n"
"all: clean $(OBJS) $(TARGET)\n# (12).oファイル、実行ファイル、.dファイルを削除\nclean:\n	-rm -f $(OBJS) $(TARGET) $(OBJDIR)/*.d";