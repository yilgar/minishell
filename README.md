# Minishell Projesi - Türkçe Özet ve Talimatlar

## Proje Genel Bakış

Bu proje, 42 School standartlarına uygun şekilde yazılmış bir POSIX shell implementasyonudur. **Bash benzeri bir shell** oluşturarak temel shell işlevlerini destekler.

## Temel Özellikler

### Shell İşlevleri
- **İnteraktif komut satırı** (readline kütüphanesi ile)
- **Komut geçmişi** (history) 
- **Prompt görüntüleme** (`minishell> `)
- **Çoklu komut desteği** (pipeline `|` operatörü ile)
- **I/O yönlendirme** (`<`, `>`, `>>`, `<<`)
- **Heredoc** desteği (`<<` operatörü)
- **Çevre değişkeni genişletme** (`$VAR`, `$?`)
- **Signal yönetimi** (Ctrl+C, Ctrl+D, Ctrl+\)

### Desteklenen Built-in Komutlar
- `echo` (n seçeneği ile)
- `cd` (relative ve absolute path)
- `pwd` (current directory)
- `export` (çevre değişkeni ayarlama)
- `unset` (çevre değişkeni silme)
- `env` (çevre değişkenlerini listeleme)
- `exit` (shell'den çıkış)

## Mimari Yapı

Proje **modüler bir pipeline mimarisi** kullanır:

```
Kullanıcı Girişi → Lexer → Parser → Expander → Executor → Çıkış
```

### Pipeline Akış Şeması

```
┌─────────────────┐
│ Kullanıcı Input │ "echo $HOME | grep test > output.txt"
│   (readline)    │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│     LEXER       │ Token'lara Ayırma
│                 │ ┌──────────────────────────────────────────────┐
│ tokenize()      │ │ INPUT: "echo $HOME | grep test > output.txt" │
│                 │ └──────────────────────────────────────────────┘
│ Çıktı:          │ ┌──────────────────────────────────────────────┐
│ t_token list    │ │ OUTPUT: [WORD:"echo"] → [ENV_VAR:"$HOME"]    │
│                 │ │         → [PIPE:"|"] → [WORD:"grep"]         │
│                 │ │         → [WORD:"test"] → [REDIRECT_OUT:">"] │
│                 │ │         → [WORD:"output.txt"] → [EOF]        │
│                 │ └──────────────────────────────────────────────┘
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│    EXPANDER     │ Değişken Genişletme
│                 │ ┌─────────────────────────────────────┐
│ expand_vars()   │ │ INPUT: [ENV_VAR:"$HOME"]            │
│                 │ └─────────────────────────────────────┘
│ İşlemler:       │ ┌─────────────────────────────────────┐
│ • $HOME →       │ │ PROCESS: $HOME → "/home/user"       │
│   "/home/user"  │ │ • Quote context check               │
│ • Token concat  │ │ • Word splitting check              │
│ • Word split    │ │ • Adjacent token concatenation      │
│                 │ └─────────────────────────────────────┘
│ Çıktı:          │ ┌─────────────────────────────────────────────┐
│ Expanded tokens │ │ OUTPUT: [WORD:"echo"] → [WORD:"/home/user"] │
│                 │ │         → [PIPE:"|"] → [WORD:"grep"]        │
│                 │ │         → [WORD:"test"] → [REDIRECT_OUT:">"]│
│                 │ │         → [WORD:"output.txt"] → [EOF]       │
│                 │ └─────────────────────────────────────────────┘
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│     PARSER      │ Yapısal Analiz
│                 │ ┌─────────────────────────────────────┐
│ parse_pipeline()│ │ INPUT: Expanded token list          │
│                 │ └─────────────────────────────────────┘
│ İşlemler:       │ ┌─────────────────────────────────────┐
│ • Syntax check  │ │ PROCESS:                            │
│ • Command parse │ │ ┌─────────────────────────────────┐ │
│ • Redirection   │ │ │ Command 1:                      │ │
│ • Argument      │ │ │ • args: ["echo", "/home/user"]  │ │
│   parsing       │ │ │ • input_file: NULL              │ │
│                 │ │ │ • output_file: NULL             │ │
│ Çıktı:          │ │ └─────────────────────────────────┘ │
│ t_pipeline      │ │ ┌─────────────────────────────────┐ │
│                 │ │ │ Command 2:                      │ │
│                 │ │ │ • args: ["grep", "test"]        │ │
│                 │ │ │ • input_file: NULL (from pipe)  │ │
│                 │ │ │ • output_file: "output.txt"     │ │
│                 │ │ └─────────────────────────────────┘ │
│                 │ └─────────────────────────────────────┘
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│    EXECUTOR     │ Komut Çalıştırma
│                 │ ┌─────────────────────────────────────┐
│ execute_        │ │ INPUT: t_pipeline structure         │
│ pipeline()      │ └─────────────────────────────────────┘
│                 │ ┌──────────────────────────────────────────┐
│ İşlemler:       │ │ PROCESS:                                 │
│ • Process yön.  │ │                                          │
│ • Pipe setup    │ │ ┌─────────────┐  pipe()  ┌─────────────┐ │
│ • Fork & exec   │ │ │   Child 1   │ -------→ │   Child 2   │ │
│ • I/O redirect  │ │ │             │          │             │ │
│ • Wait & status │ │ │ execve(     │          │ execve(     │ │
│                 │ │ │ "echo",     │  STDOUT  │ "grep",     │ │
│                 │ │ │ ["/home/    │   ════►  │ ["test"])   │ │
│                 │ │ │ user"])     │  STDIN   │ STDOUT ════►│ │
│                 │ │ │             │   ◄════  │ "output.txt"│ │
│                 │ │ └─────────────┘          └─────────────┘ │
│                 │ │                                          │
│                 │ │ Parent Process:                          │
│                 │ │ • waitpid() for both children            │
│                 │ │ • Collect exit statuses                  │
│                 │ │ • Handle signals                         │
│                 │ └──────────────────────────────────────────┘
│ Çıktı:          │ ┌─────────────────────────────────────┐
│ Exit status     │ │ OUTPUT: Command execution complete  │
│                 │ │ • Exit status propagated            │
│                 │ │ • File "output.txt" created         │
│                 │ │ • Ready for next command            │
│                 │ └─────────────────────────────────────┘
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│    CLEANUP      │ Bellek Temizliği
│                 │ 
│ • gc_free_all() │ • Tüm allocated memory'yi free et
│ • Pipeline freed│ • Token list'i temizle
│ • Ready for next│ • Next command için hazırlan
│   command       │
└─────────────────┘
```

### Özel Durumlar için Akış

#### Heredoc İşleme Akışı
```
┌─────────────────┐
│ Heredoc Token   │ "cat << EOF"
│ Detection       │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│ Child Process   │ Fork for signal isolation
│ Fork            │ 
└─────────┬───────┘
          │
          ▼
┌─────────────────┐          ┌─────────────────┐
│ Child: Input    │  pipe()  │ Parent: Wait    │
│ Collection      │ ◄══════► │ & Read          │
│                 │          │                 │
│ • readline()    │          │ • waitpid()     │
│ • Expansion     │          │ • Handle SIGINT │
│ • Write to pipe │          │ • Read from pipe│
└─────────────────┘          └─────────────────┘
```

#### Built-in vs External Command
```
┌─────────────────┐
│ Command Type    │
│ Detection       │
└─────────┬───────┘
          │
          ▼
    ┌─────────────┐
    │ is_builtin()│
    │ check       │
    └─────┬───────┘
          │
    ┌─────▼─────┐
    │   Built-in?   │
    └─────┬─────┘
          │
     ┌────▼────┐      ┌────▼─────┐
  YES│ Direct  │   NO │ Fork &   │
     │ Function│      │ Exec     │
     │ Call    │      │ Process. │
     │         │      │          │
     │• echo() │      │• fork()  │
     │• cd()   │      │• execve()│
     │• pwd()  │      │• wait()  │
     │• etc.   │      │          │
     └─────────┘      └──────────┘
```

#### Error Handling ve Exit Status Flow
```
┌─────────────────┐
│ Command Execution│
│ Result           │
└─────────┬───────┘
          │
          ▼
    ┌─────────────┐
    │ Exit Status │
    │ Check       │
    └─────┬───────┘
          │
    ┌─────▼─────┐
    │ Success?  │
    └─────┬─────┘
          │
     ┌────▼────┐      ┌────▼─────┐
  YES│ Status  │   NO │ Error    │
     │ = 0     │      │ Status   │
     │         │      │ = 1-255  │
     │ Continue│      │ or       │
     │ Normal  │      │ 128+sig  │
     │ Flow    │      │          │
     └─────────┘      └──────────┘
          │                 │
          ▼                 ▼
    ┌─────────────┐   ┌───────────────┐
    │ g_exit_     │   │ Handle        │
    │ status = 0  │   │ Error Cases   │
    │             │   │ • Command     │
    │ Ready for   │   │   not found   │
    │ next cmd    │   │ • Permission  │
    │             │   │   denied      │
    └─────────────┘   │ • Signal      │
                      │   termination │
                      └───────────────┘
```

#### Signal Handling Matrix
```
┌─────────────────────────────────────────────────────────────┐
│                    SIGNAL HANDLING CONTEXTS                 │
├─────────────────┬─────────────────┬─────────────────────────┤
│    CONTEXT      │     SIGINT      │       SIGQUIT           │
├─────────────────┼─────────────────┼─────────────────────────┤
│ Interactive     │ • Display new   │ • Ignore signal         │
│ Mode            │   prompt        │ • No action             │
│                 │ • Clear line    │                         │
│                 │ • rl_on_new_    │                         │
│                 │   line()        │                         │
├─────────────────┼─────────────────┼─────────────────────────┤
│ Command         │ • Default       │ • Default handling      │
│ Execution       │   handling      │ • Child processes       │
│ (Child)         │ • Exit with     │   handle normally       │
│                 │   130           │                         │
├─────────────────┼─────────────────┼─────────────────────────┤
│ Parent Waiting  │ • Wait for      │ • Continue waiting      │
│ for Child       │   child         │ • No interruption       │
│                 │ • Handle exit   │                         │
│                 │   status        │                         │
├─────────────────┼─────────────────┼─────────────────────────┤
│ Heredoc Child   │ • Write newline │ • Ignore                │
│                 │ • exit(130)     │                         │
│                 │ • Signal parent │                         │
└─────────────────┴─────────────────┴─────────────────────────┘
```

#### Memory Management Flow
```
┌─────────────────┐
│ Memory          │
│ Allocation      │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│ gc_track()      │ ← Wrap all malloc() calls
│ Registration    │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐      ┌─────────────────┐
│ Normal          │      │ Error           │
│ Execution       │      │ Condition       │
│                 │      │                 │
│ • Commands run  │      │ • Syntax error  │
│ • Memory used   │      │ • Exec failure  │
│ • Structures    │      │ • Signal        │
│   built         │      │   received      │
└─────────┬───────┘      └─────────┬───────┘
          │                        │
          ▼                        ▼
┌─────────────────┐              ┌─────────────────┐
│ End of Command  │              │ Emergency       │
│ Cycle           │              │ Cleanup         │
│                 │              │                 │
│ gc_free_all()   │              │ gc_free_all()   │
│ • Free all      │              │ env_cleanup()   │
│   tracked       │              │ exit()          │
│   pointers      │              │                 │
│ • Reset gc      │              │                 │
│ • Ready for     │              │                 │
│   next cycle    │              │                 │
└─────────────────┘              └─────────────────┘
```

#### Redirection Handling Pipeline
```
┌─────────────────┐
│ Redirection     │ INPUT: "cmd > file < input >> append << EOF"
│ Tokens          │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│ Parse           │ ┌─────────────────────────────────────┐
│ Redirection     │ │ • INPUT_REDIRECT: "input"           │
│ Sequence        │ │ • OUTPUT_REDIRECT: "file"           │
│                 │ │ • APPEND_REDIRECT: "append"         │
│                 │ │ • HEREDOC_REDIRECT: "EOF"           │
│                 │ └─────────────────────────────────────┘
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│ Process         │ ┌─────────────────────────────────────┐
│ Left-to-Right   │ │ 1. stdin = open("input", O_RDONLY)  │
│                 │ │ 2. stdout = open("file", O_WRONLY)  │
│                 │ │ 3. stdout = open("append", O_APPEND)│
│                 │ │ 4. stdin = handle_heredoc("EOF")    │
│                 │ └─────────────────────────────────────┘
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│ Final FD State  │ ┌─────────────────────────────────────┐
│                 │ │ stdin: heredoc pipe read end       │
│                 │ │ stdout: "append" file (append mode) │
│                 │ │ stderr: unchanged                   │
│                 │ └─────────────────────────────────────┘
└─────────────────┘
```

#### Quote Processing State Machine
```
    ┌─────────────────┐
    │ INITIAL STATE   │
    │ (No Quote)      │
    └─────────┬───────┘
              │
     ┌────────▼────────┐
     │   Read Char     │
     └─────────────────┘
              │
    ┌─────────▼─────────┐
    │ Character Type?   │
    └───────┬───────────┘
            │
    ┌───────▼───────┐    ┌───────▼───────┐    ┌───────▼───────┐
    │ Single Quote  │    │ Double Quote  │    │ Regular Char  │
    │     '         │    │      "        │    │   a-z, $, etc │
    └───────┬───────┘    └───────┬───────┘    └───────┬───────┘
            │                    │                    │
            ▼                    ▼                    ▼
┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐
│ SINGLE_QUOTE    │  │ DOUBLE_QUOTE    │  │ CONTINUE        │
│ STATE           │  │ STATE           │  │ PARSING         │
│                 │  │                 │  │                 │
│ • No expansion  │  │ • $VAR expands  │  │ • Check for     │
│ • Literal text  │  │ • $? expands    │  │   special chars │
│ • Wait for '    │  │ • Literal $     │  │ • Build word    │
│ • No escaping   │  │   for invalid   │  │ • Check spaces  │
│                 │  │ • Wait for "    │  │                 │
└─────────┬───────┘  └─────────┬───────┘  └─────────┬───────┘
          │                    │                    │
          ▼                    ▼                    ▼
┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐
│ Found closing ' │  │ Found closing " │  │ Space or        │
│ Return to       │  │ Return to       │  │ Special char    │
│ INITIAL         │  │ INITIAL         │  │ End token       │
└─────────────────┘  └─────────────────┘  └─────────────────┘
```

### Ana Bileşenler (Detaylı)

#### 1. **Lexer (`lexer/`)** - Tokenization Engine
```
┌─────────────────────────────────────────────────────────────┐
│                        LEXER COMPONENT                      │
├─────────────────┬───────────────────────────────────────────┤
│ Input           │ "echo 'hello world' | grep $USER > file" │
├─────────────────┼───────────────────────────────────────────┤
│ Character       │ ┌─────────────────────────────────────┐   │
│ Analysis        │ │ • Whitespace detection              │   │
│                 │ │ • Quote boundary tracking           │   │
│                 │ │ • Special character identification  │   │
│                 │ │ • Variable prefix detection ($)     │   │
│                 │ └─────────────────────────────────────┘   │
├─────────────────┼───────────────────────────────────────────┤
│ Token Types     │ • TOKEN_WORD: "echo", "hello world"      │
│ Generated       │ • TOKEN_PIPE: "|"                        │
│                 │ • TOKEN_REDIRECT_OUT: ">"                │
│                 │ • TOKEN_ENV_VAR: "$USER"                 │
│                 │ • TOKEN_SINGLE_QUOTE: 'hello world'      │
├─────────────────┼───────────────────────────────────────────┤
│ Output          │ t_token linked list with metadata:       │
│ Structure       │ • type, value, has_space_before          │
│                 │ • from_env_expansion, is_quoted_delimiter │
└─────────────────┴───────────────────────────────────────────┘
```

#### 2. **Parser (`parser/`)** - Structural Analysis Engine
```
┌─────────────────────────────────────────────────────────────┐
│                       PARSER COMPONENT                      │
├─────────────────┬───────────────────────────────────────────┤
│ Input           │ t_token linked list from lexer           │
├─────────────────┼───────────────────────────────────────────┤
│ Syntax          │ ┌─────────────────────────────────────┐   │
│ Validation      │ │ • Pipe syntax check                 │   │
│                 │ │ • Redirection syntax check          │   │
│                 │ │ • Quote matching verification       │   │
│                 │ │ • Invalid token sequence detection  │   │
│                 │ └─────────────────────────────────────┘   │
├─────────────────┼───────────────────────────────────────────┤
│ Command         │ ┌─────────────────────────────────────┐   │
│ Parsing         │ │ Command 1: echo 'hello world'       │   │
│                 │ │ • args: ["echo", "hello world"]     │   │
│                 │ │ • redirections: none                │   │
│                 │ │                                     │   │
│                 │ │ Command 2: grep $USER               │   │
│                 │ │ • args: ["grep", "$USER"]           │   │
│                 │ │ • input: pipe from cmd1             │   │
│                 │ │ • output: "file"                    │   │
│                 │ └─────────────────────────────────────┘   │
├─────────────────┼───────────────────────────────────────────┤
│ Output          │ t_pipeline containing:                    │
│ Structure       │ • Chain of t_cmd structures               │
│                 │ • Redirection information                 │
│                 │ • Pipeline connection data                │
└─────────────────┴───────────────────────────────────────────┘
```

#### 3. **Expander (`expander/`)** - Variable & Quote Processing Engine
```
┌─────────────────────────────────────────────────────────────┐
│                     EXPANDER COMPONENT                      │
├─────────────────┬───────────────────────────────────────────┤
│ Input           │ Parsed commands with unexpanded variables │
├─────────────────┼───────────────────────────────────────────┤
│ Variable        │ ┌─────────────────────────────────────┐   │
│ Expansion       │ │ • $USER → "codespace"               │   │
│                 │ │ • $HOME → "/home/codespace"         │   │
│                 │ │ • $? → "0" (last exit status)       │   │
│                 │ │ • $INVALID → "" (empty string)      │   │
│                 │ └─────────────────────────────────────┘   │
├─────────────────┼───────────────────────────────────────────┤
│ Quote           │ ┌─────────────────────────────────────┐   │
│ Processing      │ │ Single quotes ('): No expansion     │   │
│                 │ │ • 'hello $USER' → "hello $USER"     │   │
│                 │ │                                     │   │
│                 │ │ Double quotes ("): Expand vars      │   │
│                 │ │ • "hello $USER" → "hello codespace" │   │
│                 │ │                                     │   │
│                 │ │ Unquoted: Expand + word split       │   │
│                 │ │ • hello$USER → "hellocodespace"     │   │
│                 │ └─────────────────────────────────────┘   │
├─────────────────┼───────────────────────────────────────────┤
│ Word Splitting  │ ┌─────────────────────────────────────┐   │
│                 │ │ Unquoted expansions split on IFS:   │   │
│                 │ │ • "one two three" → ["one","two",   │   │
│                 │ │   "three"]                          │   │
│                 │ │ • Preserve quoted strings intact    │   │
│                 │ └─────────────────────────────────────┘   │
├─────────────────┼───────────────────────────────────────────┤
│ Output          │ Fully expanded and split command args     │
│                 │ Ready for execution                       │
└─────────────────┴───────────────────────────────────────────┘
```

#### 4. **Executor (`execution/`)**
- Process yönetimi
- I/O yönlendirme
- Pipeline execution
- Built-in komut çalıştırma

## Detaylı Çalışma Sırası (main'den itibaren)

### 1. **Program Başlatma (`main` fonksiyonu)**
```c
int main(int argc, char **argv, char **envp)
```
- `t_gc gc` - Garbage Collector initialize
- `t_env *env` - Environment copy oluştur (`env_init_copy`)
- `interactive_shell(&gc, env)` - Ana shell loop'una gir

### 2. **Interactive Shell Loop (`interactive_shell`)**
```c
void interactive_shell(t_gc *gc, t_env *env)
```
- `setup_readline()` - Readline kütüphanesini hazırla
- `setup_signals_interactive()` - Interactive signal handler'ları set et
- **Ana Loop:**
  1. `read_command_line()` - Kullanıcıdan input al (readline)
  2. `process_command_line()` - Komutu işle
  3. Signal ve continuation handling
  4. Memory cleanup

### 3. **Komut İşleme Pipeline (`process_command_line`)**
```c
int process_command_line(t_gc *gc, t_env *env, char *line, int *exit_status)
```

#### 3.1. **Input Validation ve Tokenization**
- `validate_input_and_tokenize()` - Boş input kontrol
- `tokenize(gc, line)` - Input'u token'lara ayır

#### 3.2. **Token Processing (`process_tokens`)**
- `mark_heredoc_delimiters()` - Heredoc delimiter'ları işaretle
- `process_token_expansion()` - Token expansion işlemi
- `split_expanded_tokens()` - Expansion sonrası word splitting

#### 3.3. **Pipeline Oluşturma**
- `create_pipeline_from_tokens()` - Token'lardan pipeline oluştur
- `parse_pipeline()` - Parser çağrısı
- Incomplete pipe kontrolü

#### 3.4. **Execution**
- `execute_and_cleanup()` - Pipeline'ı çalıştır ve temizle

### 4. **Lexer Stage (`tokenize`)**
```c
t_token *tokenize(t_gc *gc, char *input)
```
- Character-by-character input parsing
- Token type detection:
  - Words, operators (`|`, `<`, `>`, `>>`, `<<`)
  - Quotes (`'`, `"`)
  - Environment variables (`$VAR`, `$?`)
- `t_token` linked list oluşturma

### 5. **Parser Stage (`parse_pipeline`)**
```c
t_pipeline *parse_pipeline(t_gc *gc, t_token *tokens, int *incomplete_pipe)
```
- **Syntax Validation** (`validate_syntax`)
- **Command Parsing** (`parse_command`):
  - Arguments parsing (`parse_args`)
  - Redirection handling (`handle_redirection`)
  - Heredoc processing (`handle_heredoc_redirection`)
- **Pipeline Structure Creation**

### 6. **Expander Stage**
- **Variable Expansion** (`expand_variables`):
  - `$VAR` environment variable expansion
  - `$?` exit status expansion
  - Quote context awareness
- **Word Splitting** (unquoted expansions için)
- **Token Concatenation** (adjacent tokens)

### 7. **Executor Stage (`execute_pipeline`)**
```c
int execute_pipeline(t_gc *gc, t_env *env, t_pipeline *pipeline)
```

#### 7.1. **Pipeline Type Detection**
- **Single Command** → `execute_single_command_pipeline`
- **Multi Command** → `execute_pipeline_commands`

#### 7.2. **Command Execution (`execute_command`)**
- **Redirection Setup** (`setup_redirections`)
- **Command Type Detection**:
  - **Built-in** → `execute_builtin`
  - **External** → `fork_and_exec`
  - **Empty** → redirection-only processing

#### 7.3. **Process Management**
- **Fork & Exec** (`fork_and_exec`)
- **Pipe Setup** (multi-command pipeline'lar için)
- **Signal Handling** (child vs parent)
- **Wait & Status Collection** (`wait_for_processes`)

### 8. **Built-in Command Execution**
- Direct function calls (`builtin_echo`, `builtin_cd`, etc.)
- Environment manipulation (`env_add_str`, `env_remove_key`)
- Working directory changes

### 9. **External Command Execution**
- **Command Path Resolution** (`find_command_path`)
- **Environment Array Preparation** (`env_to_array`)
- **Child Process Setup**:
  - Signal handler reset (`setup_signals_child_default`)
  - File descriptor redirection
  - `execve` system call

### 10. **Cleanup ve Memory Management**
- **Garbage Collection** (`gc_free_all`) - her iteration sonrası
- **Environment Cleanup** (`env_cleanup`) - program sonunda
- **History Management** (readline)

## Özel İşlemler

### Heredoc İşleme
1. **Delimiter Detection** (lexer'da)
2. **Quote Status Check** (quoted/unquoted delimiter)
3. **Child Process Fork** (signal isolation için)
4. **Input Collection** (readline ile)
5. **Expansion** (unquoted delimiter'lar için)
6. **Pipe Communication** (parent-child arası)

### Signal Yönetimi
- **Interactive Mode**: SIGINT → new prompt, SIGQUIT → ignore
- **Command Execution**: Default signal handling
- **Heredoc Child**: Özel SIGINT handling (exit with 130)

### Pipeline Execution
1. **Pipe Creation** (`pipe()` system call)
2. **Child Process Chain** (her komut için ayrı process)
3. **File Descriptor Management** (stdin/stdout redirection)
4. **Synchronization** (`waitpid` ile)
5. **Exit Status Propagation** (last command'ın status'u)

## Veri Yapıları

### Ana Struct'lar
- `t_gc` - Garbage Collector (otomatik bellek yönetimi)
- `t_env` - Çevre değişkenleri yönetimi
- `t_token` - Lexical token'lar
- `t_cmd` - Komut yapısı (args, redirections, etc.)
- `t_pipeline` - Komut pipeline'ı
- `t_heredoc` - Heredoc zincirleri

### 42 Norm Uyumluluğu

#### İsimlendirme Kuralları
- **Struct'lar**: `s_` prefix (`s_token`, `s_cmd`)
- **Typedef'ler**: `t_` prefix (`t_token`, `t_pipeline`)
- **Enum'lar**: `e_` prefix (`e_token_type`)
- **Global'ler**: `g_` prefix (`g_exit_status`)
- **Snake_case** only - camelCase yasak

#### Fonksiyon Kısıtlamaları
- **Maksimum 25 satır** per fonksiyon
- **Maksimum 4 parametre** per fonksiyon  
- **Maksimum 5 değişken** per fonksiyon
- **Maksimum 5 fonksiyon** per .c dosyası
- Return değerleri parantez içinde: `return (value);`

#### Formatting
- **80 sütun max** per satır
- **4-char tab** (space değil)
- Ayrı satırlarda küme parantezleri
- Asterisk'ler değişken isimlerine yapışık: `char *str`

#### Yasak Yapılar
- `external functions` harici fonksiyonlar yasak
- `for` döngüleri (instead `while` kullan)
- `do...while`, `switch`, `case`, `goto`
- Ternary operatörler (`? :`)
- Variable Length Arrays (VLAs)

## Önemli Implementation Detayları

### Bellek Yönetimi
- **Garbage Collector** sistemi (`gc/gc.c`)
- Tüm allocation'lar `gc_track(gc, malloc(...))` ile wrap edilir
- Shell çıkışında veya hatada otomatik cleanup

### Process Yönetimi
- **Single komutlar**: Ana process'te çalışır (built-in'ler için)
- **Pipeline'lar**: Child process'ler ile pipe setup
- **Heredoc'lar**: Signal isolation için her zaman fork eder

### Signal Handling
- **İnteraktif mod**: SIGINT yeni prompt, SIGQUIT ignore
- **Komut execution**: Child'lar default signal handler'ları inherit eder
- **Heredoc**: Child process'te özel signal setup

### Quote ve Expansion Kuralları
- **Single quotes**: Expansion yok
- **Double quotes**: `$VAR` ve `$?` expand, invalid varlar için literal `$`
- **Unquoted**: Full expansion + word splitting
- **Heredoc delimiter'lar**: Quoted ise heredoc content'te expansion disable

## Dizin Yapısı ve Sorumluluklar

```
.
├── builtin/          # Built-in komutlar (cd, echo, env, etc.)
├── env/              # Çevre değişkeni yönetimi
├── lexer/            # Tokenization ve karakter sınıflandırma
├── parser/           # Syntax analizi ve AST benzer yapısı
├── expander/         # Değişken expansion logic
├── execution/        # Process yönetimi, I/O redirection
├── signals/          # Signal handler setup
├── utils/            # Karakter tip kontrolü utilities
├── gc/               # Garbage collector
├── libft/            # Standard C library fonksiyonları
└── tests/            # Test script'leri
```

## Build ve Test

### Derleme
```bash
make                # Debug symbols ile build (-g)
make clean          # Sadece object'leri temizle
make fclean         # Libft dahil full temizlik
```

### Debug - Readline'den gelen still reachablelar göz ardı edilmiştir
```bash
valgrind --suppressions=valgrind.supp ./minishell
```

### Test Scripts
- `tests/test_heredoc.sh` - Heredoc edge case'leri
- `tests/test_bash_comparison.sh` - Bash ile output karşılaştırması
- `tests/final_test.sh` - Comprehensive functionality test

## Önemli Gotcha'lar

### Norm Compliance
- Değişkenler fonksiyon başında declare edilmeli
- Inline declaration yasak: `int i = 0;` (global/static/const hariç)
- Tab kullan, space değil
- Function prototype'lar explicit parameter isimleri gerektirir

### Fork Safety
- Child process'lerde `exit()` öncesi her zaman `gc_free_all()` ve `env_cleanup()`

### File Descriptor Management
- Tüm açılan FD'leri track et
- Helper fonksiyonlar kullan: `close_pipes()`
- Redirection FD'lerini kullanım sonrası kapat

### Token Concatenation
- Space olmadan bitişik token'lar (`echo$HOME` gibi) tek argümana concatenate edilmeli
- `has_space_before` flag'ini kontrol et

### Exit Status
- Komut exit code'ları (0-255) vs signal termination (128+signal) ayrımını yap
- `$?` expansion için son komutun status'unu kullan

Bu proje, sistem programlama, process yönetimi, signal handling ve lexical analysis konularında derinlemesine öğrenme sağlar while strict 42 Norm standartlarına uygun kod yazma pratiği yapar.

## Praktik Örnekler ve Kullanım Senaryoları

### Temel Komut Örnekleri
```bash
# Basit komutlar
minishell> pwd
/home/codespace/workspace

minishell> echo "Hello World"
Hello World

minishell> echo -n "No newline"
No newline

# Çevre değişkenleri
minishell> echo $HOME
/home/codespace

minishell> echo "$USER works in $PWD"
codespace works in /home/codespace/workspace
```

### Pipeline Örnekleri
```bash
# Basit pipeline
minishell> ls | grep ".c" | wc -l
15

# Kompleks pipeline
minishell> cat file.txt | grep "error" | sort | uniq -c | head -5
   3 error: file not found
   2 error: permission denied
   1 error: syntax error
```

### Redirection Örnekleri
```bash
# Output redirection
minishell> echo "test" > output.txt
minishell> cat < output.txt
test

# Append redirection
minishell> echo "line2" >> output.txt
minishell> cat output.txt
test
line2

# Multiple redirections
minishell> cat < input.txt > output.txt 2> error.log
```

### Heredoc Örnekleri
```bash
# Basit heredoc
minishell> cat << EOF
> This is line 1
> This is line 2
> EOF
This is line 1
This is line 2

# Heredoc with variables
minishell> cat << EOF
> Current user: $USER
> Current directory: $PWD
> EOF
Current user: codespace
Current directory: /home/codespace/workspace

# Quoted heredoc (no expansion)
minishell> cat << 'EOF'
> Current user: $USER
> Current directory: $PWD  
> EOF
Current user: $USER
Current directory: $PWD
```

## Hata Durumları ve Debugging

### Yaygın Hatalar ve Çözümleri

#### 1. **Memory Leaks**
```bash
# Problem: Garbage collector kullanılmamış
char *str = malloc(100);  // ❌ Memory leak

# Çözüm: GC ile track et
char *str = gc_track(gc, malloc(100));  // ✅ Otomatik cleanup
```

#### 2. **Signal Handling Sorunları**
```c
// Problem: Child process'te signal handler ayarlanmamış
// Çözüm: Her context için doğru signal setup
setup_signals_interactive();     // Interactive mode
setup_signals_child_default();   // Child processes
setup_signals_heredoc_child();   // Heredoc children
```

#### 3. **File Descriptor Leaks**
```c
// Problem: FD'ler kapatılmamış
int fd = open("file.txt", O_RDONLY);  // ❌ Leak risk

// Çözüm: Her açılan FD'yi kapat
int fd = open("file.txt", O_RDONLY);
if (fd != -1) {
    // Use fd
    close(fd);  // ✅ Always close
}
```

### Debug Komutları
```bash
# Memory leak kontrolü
valgrind --leak-check=full --suppressions=valgrind.supp ./minishell

# Sistem çağrıları trace etme
strace -e trace=execve,fork,pipe,dup2 ./minishell

# GDB ile debugging
gdb ./minishell
(gdb) run
(gdb) bt        # Backtrace
(gdb) info registers
```

## Performance Optimizasyonları

### Memory Management
- **Lazy allocation**: Sadece gerekli olduğunda allocate et
- **Bulk operations**: Tek seferde birden fazla token process et
- **Reuse patterns**: Sık kullanılan string'leri cache'le

### Process Management
- **Fork optimization**: Gereksiz fork'lardan kaçın
- **Pipe management**: Kullanılmayan pipe end'lerini hemen kapat
- **Wait optimization**: waitpid() ile selective waiting

## Gelişmiş Özellikler ve Extensibility

### Mevcut Genişletilebilir Alanlar

#### 1. **Built-in Command Extensions**
```c
// Yeni built-in eklemek için:
// 1. builtin_manager.c içinde is_builtin() güncelle
// 2. execute_builtin() içinde handler ekle
// 3. builtin/ klasöründe implementation yaz

int builtin_history(t_gc *gc, t_env *env, char **args)
{
    // History implementation
    return (0);
}
```

#### 2. **Custom Variable Types**
```c
// Özel değişken türleri eklenebilir
// Örnek: $RANDOM, $LINENO gibi
char *expand_special_var(t_gc *gc, char *var_name)
{
    if (ft_strcmp(var_name, "RANDOM") == 0)
        return (gc_track(gc, ft_itoa(rand() % 32768)));
    return (NULL);
}
```

#### 3. **Advanced Redirection**
```c
// Gelecekte eklenebilir:
// - Process substitution: <(cmd)
// - Co-processes: |&
// - Network redirections: /dev/tcp/host/port
```

## Test Coverage ve Validation

### Test Kategorileri

#### 1. **Unit Tests**
- Lexer token generation
- Parser syntax validation  
- Expander variable resolution
- Executor command handling

#### 2. **Integration Tests**
- End-to-end pipeline execution
- Complex redirection scenarios
- Signal handling validation
- Memory management verification

#### 3. **Edge Cases**
```bash
# Empty commands
minishell> 
minishell>     # Only whitespace

# Syntax errors
minishell> |     # Pipe at beginning
minishell> cmd | # Incomplete pipe
minishell> > file # Redirection without command

# Resource limits
minishell> very_long_command_with_many_arguments...
minishell> cmd1 | cmd2 | cmd3 | ... | cmd100  # Many pipes
```

### Test Automation
```bash
# Comprehensive test suite
make test                    # Run all tests
make test-norm              # 42 Norm compliance
make test-memory            # Memory leak detection
make test-functionality     # Feature validation
make test-comparison        # Bash behavior comparison
```

## Proje Teslim Kriterleri

### Zorunlu Gereksinimler ✅
- [x] Interactive shell with prompt
- [x] Working history
- [x] Command search and launch  
- [x] Local and environment variables
- [x] Built-ins: echo, cd, pwd, export, unset, env, exit
- [x] Redirections: <, >, <<, >>
- [x] Pipes |
- [x] Environment variable expansion
- [x] Exit status handling
- [x] Signal handling: ctrl-C, ctrl-D, ctrl-\

### 42 Norm Compliance ✅
- [x] Function length ≤ 25 lines
- [x] Function parameters ≤ 4
- [x] Function variables ≤ 5
- [x] Functions per file ≤ 5
- [x] Proper naming conventions
- [x] No forbidden functions/constructs

## Geliştirici Notları

### Kod Kalitesi İçin Öneriler
1. **Consistent Error Handling**: Her fonksiyon error case'leri handle etmeli
2. **Modular Design**: Her component bağımsız test edilebilmeli  
3. **Documentation**: Complex logic'ler için inline comment'ler
4. **Code Review**: Peer review süreci implement edilmeli

### Performans Metrikleri
- **Memory Usage**: Normal kullanımda <10MB
- **Response Time**: Basit komutlar <100ms
- **Throughput**: Pipeline'lar verimli process etmeli
- **Resource Cleanup**: Hiç memory leak olmamalı

Bu proje, sistem seviyesi programming'in neredeyse tüm aspectlerini kapsayan comprehensive bir learning experience sağlar.
