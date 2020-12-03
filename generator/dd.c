#define DD_FUNCTION_IMPL DD_FUNCTION
#define DD_PRIVATE_FUNCTION_IMPL DD_FUNCTION_IMPL

//~ NOTE(rjf): OS Implementation Functions
#if DD_OS
DD_PRIVATE_FUNCTION_IMPL DD_b32 _DD_OS_IMPL_FileIter_Increment(DD_FileIter *it, DD_String8 path, DD_FileInfo *out_info);
#endif

//~ NOTE(rjf): OS Layers

#if DD_WIN32
#include "dd_win32.c"
#endif

#if DD_POSIX
#include "dd_posix.c"
#endif

//~

static DD_Node _dd_nil_node =
{
    .kind         = DD_NodeKind_Null,
    .string       = {"`NIL DD NODE`", 13},
    .whole_string = {"`NIL DD NODE`", 13},
    .string_hash  = 0xdeadffffffffffull,
    .file         = {"`NIL DD NODE`", 13},
    .line         = 1,
};

DD_FUNCTION_IMPL DD_b32
DD_CharIsAlpha(DD_u8 c)
{
    return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}

DD_FUNCTION_IMPL DD_b32
DD_CharIsDigit(DD_u8 c)
{
    return (c >= '0' && c <= '9');
}

DD_FUNCTION_IMPL DD_b32
DD_CharIsSymbol(DD_u8 c)
{
    return (c == '~' || c == '!' || c == '@' || c == '#' || c == '$' ||
            c == '%' || c == '^' || c == '&' || c == '*' || c == '(' ||
            c == ')' || c == '-' || c == '=' || c == '+' || c == '[' ||
            c == ']' || c == '{' || c == '}' || c == ':' || c == ';' ||
            c == ',' || c == '<' || c == '.' || c == '>' || c == '/' ||
            c == '?' || c == '|' || c == '\\');
}

DD_FUNCTION_IMPL DD_u8
DD_CharToUpper(DD_u8 c)
{
    return (c >= 'a' && c <= 'z') ? ('A' + (c - 'a')) : c;
}

DD_FUNCTION_IMPL DD_u8
DD_CharToLower(DD_u8 c)
{
    return (c >= 'A' && c <= 'Z') ? ('a' + (c - 'Z')) : c;
}

DD_FUNCTION_IMPL DD_String8
DD_S8(DD_u8 *str, DD_u64 size)
{
    DD_String8 string;
    string.str = str;
    string.size = size;
    return string;
}

DD_FUNCTION_IMPL DD_b32
DD_StringIsZero(DD_String8 str)
{
    return str.str == 0 || str.size == 0;
}

DD_FUNCTION_IMPL DD_String8
DD_StringSubstring(DD_String8 str, DD_u64 min, DD_u64 max)
{
    if(max > str.size)
    {
        max = str.size;
    }
    if(min > str.size)
    {
        min = str.size;
    }
    if(min > max)
    {
        DD_u64 swap = min;
        min = max;
        max = swap;
    }
    str.size = max - min;
    str.str += min;
    return str;
}

DD_FUNCTION_IMPL DD_String8
DD_StringPrefix(DD_String8 str, DD_u64 max)
{
    return DD_StringSubstring(str, 0, max);
}

DD_FUNCTION_IMPL DD_String8
DD_StringSuffix(DD_String8 str, DD_u64 min)
{
    return DD_StringSubstring(str, min, str.size);
}

DD_FUNCTION_IMPL DD_b32
DD_StringMatch(DD_String8 a, DD_String8 b, DD_StringMatchFlags flags)
{
    int result = 0;
    if(a.str && b.str && a.size && b.size && a.size == b.size)
    {
        result = 1;
        for(DD_u64 i = 0; i < a.size; i += 1)
        {
            DD_b32 match = (a.str[i] == b.str[i]);
            if(flags & DD_StringMatchFlag_CaseInsensitive)
            {
                match |= (DD_CharToLower(a.str[i]) == DD_CharToLower(b.str[i]));
            }
            if(match == 0)
            {
                result = 0;
                break;
            }
        }
    }
    return result;
}

DD_FUNCTION_IMPL DD_b32
DD_StringFindSubstring(DD_String8 str, DD_String8 substring, DD_u64 occurrence, DD_StringMatchFlags flags, DD_u64 *start)
{
    DD_b32 found = 0;
    DD_u64 idx = 0;
    DD_u64 occurrence_idx = 0;
    
    for(DD_u64 i = 0; i < str.size; i += 1)
    {
        if(i + substring.size <= str.size && str.str[i] == substring.str[0])
        {
            DD_String8 substr_from_str = DD_StringSubstring(str, i, i+substring.size);
            if(DD_StringMatch(substr_from_str, substring, flags))
            {
                occurrence_idx += 1;
                if(occurrence_idx >= occurrence)
                {
                    idx = i;
                    found = 1;
                    if(!(flags & DD_StringMatchFlag_FindLast))
                    {
                        break;
                    }
                }
            }
        }
    }
    
    if(found && start)
    {
        *start = idx;
    }
    
    return found;
}

DD_FUNCTION_IMPL DD_b32
DD_StringFindLastSubstring(DD_String8 str, DD_String8 substring, DD_StringMatchFlags flags, DD_u64 *start)
{
    return DD_StringFindSubstring(str, substring, 0, flags | DD_StringMatchFlag_FindLast, start);
}

DD_FUNCTION_IMPL DD_String8
DD_WithoutExtension(DD_String8 string)
{
    DD_u64 period_pos = 0;
    if(DD_StringFindLastSubstring(string, DD_S8Lit("."), 0, &period_pos))
    {
        string.size = period_pos;
    }
    return string;
}

DD_FUNCTION_IMPL DD_String8
DD_WithoutFolder(DD_String8 string)
{
    DD_u64 fslash_pos = 0;
    DD_u64 bslash_pos = 0;
    DD_b32 slash = 0;
    slash |= DD_StringFindLastSubstring(string, DD_S8Lit("/"),  0, &fslash_pos);
    slash |= DD_StringFindLastSubstring(string, DD_S8Lit("\\"),  0, &bslash_pos);
    if(slash)
    {
        DD_u64 p = fslash_pos > bslash_pos ? fslash_pos : bslash_pos;
        p += 1;
        string.str += p;
        string.size -= p;
    }
    return string;
}

DD_FUNCTION_IMPL DD_String8
DD_ExtensionString(DD_String8 string)
{
    DD_u64 period_pos = 0;
    if(DD_StringFindLastSubstring(string, DD_S8Lit("."), 0, &period_pos))
    {
        string.str += period_pos+1;
        string.size -= period_pos+1;
    }
    return string;
}

DD_FUNCTION_IMPL DD_String8
DD_FolderString(DD_String8 string)
{
    DD_u64 fslash_pos = 0;
    DD_u64 bslash_pos = 0;
    DD_b32 slash = 0;
    slash |= DD_StringFindLastSubstring(string, DD_S8Lit("/"),  0, &fslash_pos);
    slash |= DD_StringFindLastSubstring(string, DD_S8Lit("\\"),  0, &bslash_pos);
    if(slash)
    {
        DD_u64 p = fslash_pos > bslash_pos ? fslash_pos : bslash_pos;
        string.size = p;
    }
    return string;
}

DD_FUNCTION_IMPL char *
DD_CStringFromString8(DD_String8 string)
{
    char *result = calloc(string.size+1, 1);
    for(DD_u64 i = 0; i < string.size; i += 1)
    {
        result[i] = string.str[i];
    }
    return result;
}

DD_FUNCTION_IMPL DD_String8
DD_PushStringFV(char *fmt, va_list args)
{
    DD_String8 result = {0};
    va_list args2;
    va_copy(args2, args);
    DD_u64 needed_bytes = vsnprintf(0, 0, fmt, args)+1;
    result.str = calloc(needed_bytes, 1);
    result.size = needed_bytes-1;
    vsnprintf(result.str, needed_bytes, fmt, args2);
    return result;
}

DD_FUNCTION_IMPL DD_String8
DD_PushStringF(char *fmt, ...)
{
    DD_String8 result = {0};
    va_list args;
    va_start(args, fmt);
    result = DD_PushStringFV(fmt, args);
    va_end(args);
    return result;
}

DD_FUNCTION_IMPL char *
DD_PushCStringFV(char *fmt, va_list args)
{
    char *result = 0;
    va_list args2;
    va_copy(args2, args);
    DD_u64 needed_bytes = vsnprintf(0, 0, fmt, args)+1;
    result = calloc(needed_bytes, 1);
    vsnprintf(result, needed_bytes, fmt, args2);
    return result;
}

DD_FUNCTION_IMPL char *
DD_PushCStringF(char *fmt, ...)
{
    char *result = 0;
    va_list args;
    va_start(args, fmt);
    result = DD_PushCStringFV(fmt, args);
    va_end(args);
    return result;
}

DD_FUNCTION_IMPL void
DD_PushStringToList(DD_String8List *list, DD_String8 string)
{
    DD_String8Node *node = calloc(1, sizeof(*node));
    node->next = 0;
    node->string = string;
    if(list->last == 0)
    {
        list->first = list->last = node;
    }
    else
    {
        list->last->next = node;
        list->last = list->last->next;
    }
}

DD_FUNCTION_IMPL void
DD_PushStringListToList(DD_String8List *list, DD_String8List to_push)
{
    if(to_push.first)
    {
        if(list->last == 0)
        {
            *list = to_push;
        }
        else
        {
            list->last->next = to_push.first;
            list->last = to_push.last;
        }
    }
}

DD_FUNCTION_IMPL DD_String8List
DD_SplitString(DD_String8 string, int split_count, DD_String8 *splits)
{
    DD_String8List list = {0};
    
    DD_u64 split_start = 0;
    for(DD_u64 i = 0; i < string.size; i += 1)
    {
        DD_b32 was_split = 0;
        for(int split_idx = 0; split_idx < split_count; split_idx += 1)
        {
            DD_b32 match = 0;
            if(i + splits[split_idx].size <= string.size)
            {
                match = 1;
                for(DD_u64 split_i = 0; split_i < splits[split_idx].size && i + split_i < string.size; split_i += 1)
                {
                    if(splits[split_idx].str[split_i] != string.str[i + split_i])
                    {
                        match = 0;
                        break;
                    }
                }
            }
            if(match)
            {
                DD_String8 split_string = DD_S8(string.str + split_start, i - split_start);
                DD_PushStringToList(&list, split_string);
                split_start = i + splits[split_idx].size;
                i += splits[split_idx].size - 1;
                was_split = 1;
                break;
            }
        }
        
        if(was_split == 0 && i == string.size - 1)
        {
            DD_String8 split_string = DD_S8(string.str + split_start, i+1 - split_start);
            DD_PushStringToList(&list, split_string);
            break;
        }
    }
    
    return list;
}

DD_FUNCTION_IMPL DD_String8List
DD_SplitStringByString(DD_String8 string, DD_String8 split)
{
    return DD_SplitString(string, 1, &split);
}

DD_FUNCTION_IMPL DD_String8List
DD_SplitStringByCharacter(DD_String8 string, DD_u8 character)
{
    return DD_SplitStringByString(string, DD_S8(&character, 1));
}

DD_FUNCTION_IMPL int
DD_IntFromString(DD_String8 string)
{
    char *str = DD_CStringFromString8(string);
    return atoi(str);
}

DD_FUNCTION_IMPL float
DD_FloatFromString(DD_String8 string)
{
    char *str = DD_CStringFromString8(string);
    return atof(str);
}

DD_FUNCTION_IMPL DD_u64
DD_HashString(DD_String8 string)
{
    DD_u64 result = 5381;
    for(DD_u64 i = 0; i < string.size; i += 1)
    {
        result = ((result << 5) + result) + string.str[i];
    }
    return result;
}

DD_FUNCTION_IMPL DD_u64
DD_CalculateCStringLength(char *cstr)
{
    DD_u64 i = 0;
    for(; cstr[i]; i += 1);
    return i;
}

DD_PRIVATE_FUNCTION_IMPL void
_DD_NodeTable_Initialize(DD_NodeTable *table)
{
    if(table->table_size == 0)
    {
        table->table_size = 4096;
        table->table = calloc(table->table_size, sizeof(DD_NodeTableSlot *));
    }
}

DD_FUNCTION_IMPL DD_NodeTableSlot *
DD_NodeTable_Lookup(DD_NodeTable *table, DD_String8 string)
{
    _DD_NodeTable_Initialize(table);
    
    DD_NodeTableSlot *slot = 0;
    DD_u64 hash = DD_HashString(string);
    DD_u64 index = hash % table->table_size;
    for(DD_NodeTableSlot *candidate = table->table[index]; candidate; candidate = candidate->next)
    {
        if(candidate->hash == hash)
        {
            slot = candidate;
            break;
        }
    }
    return slot;
}

DD_FUNCTION_IMPL DD_b32
DD_NodeTable_Insert(DD_NodeTable *table, DD_NodeTableCollisionRule collision_rule, DD_String8 string, DD_Node *node)
{
    _DD_NodeTable_Initialize(table);
    
    DD_NodeTableSlot *slot = 0;
    DD_u64 hash = DD_HashString(string);
    DD_u64 index = hash % table->table_size;
    
    for(DD_NodeTableSlot *candidate = table->table[index]; candidate; candidate = candidate->next)
    {
        if(candidate->hash == hash)
        {
            slot = candidate;
            break;
        }
    }
    
    if(slot == 0 || (slot != 0 && collision_rule == DD_NodeTableCollisionRule_Chain))
    {
        slot = calloc(1, sizeof(*slot));
        if(slot)
        {
            slot->next = table->table[index];
            table->table[index] = slot;
        }
    }
    
    if(slot)
    {
        slot->node = node;
        slot->hash = hash;
    }
    
    return !!slot;
}

DD_FUNCTION_IMPL DD_Token
DD_TokenZero(void)
{
    DD_Token token = {0};
    return token;
}

DD_FUNCTION_IMPL DD_Tokenizer
DD_Tokenizer_Start(DD_String8 filename, DD_String8 file_contents)
{
    DD_Tokenizer tokenizer = {0};
    tokenizer.at = file_contents.str;
    tokenizer.at_line = 1;
    tokenizer.filename = filename;
    tokenizer.file_contents = file_contents;
    return tokenizer;
}

DD_FUNCTION_IMPL DD_Token
DD_Tokenizer_Peek(DD_Tokenizer *tokenizer)
{
    DD_Token token = {0};
    DD_u8 *one_past_last = tokenizer->file_contents.str + tokenizer->file_contents.size;
    
    enum
    {
        DD_ReadMode_Normal,
        DD_ReadMode_SingleLineComment,
        DD_ReadMode_MultiLineComment,
    };
    int read_mode = DD_ReadMode_Normal;
    int multiline_comment_nest = 0;
    
    for(DD_u64 i = 0; tokenizer->at + i < one_past_last; i += 1)
    {
        
        if(read_mode == DD_ReadMode_SingleLineComment)
        {
            if(tokenizer->at[i] == '\n')
            {
                read_mode = DD_ReadMode_Normal;
            }
        }
        else if(read_mode == DD_ReadMode_MultiLineComment)
        {
            if(tokenizer->at[i] == '/' && tokenizer->at[i+1] == '*')
            {
                multiline_comment_nest += 1;
            }
            if(tokenizer->at[i] == '*' && tokenizer->at[i+1] == '/')
            {
                multiline_comment_nest -= 1;
            }
            if(multiline_comment_nest <= 0)
            {
                read_mode = DD_ReadMode_Normal;
            }
        }
        else
        {
#define DD_TokenizerLoop(name, start, cond) DD_u64 name = start; for(; ((cond)) && tokenizer->at + name < one_past_last; name += 1)
            
            // NOTE(rjf): Single-Line Comments
            if(tokenizer->at[i] == '/' && tokenizer->at[i+1] == '/')
            {
                read_mode = DD_ReadMode_SingleLineComment;
            }
            
            // NOTE(rjf): Multi-Line Comments
            else if(tokenizer->at[i] == '/' && tokenizer->at[i+1] == '*')
            {
                read_mode = DD_ReadMode_MultiLineComment;
                multiline_comment_nest = 1;
            }
            
            // NOTE(rjf): Identifier
            else if(DD_CharIsAlpha(tokenizer->at[i]))
            {
                DD_TokenizerLoop(j, i+1,
                                 DD_CharIsAlpha(tokenizer->at[j]) ||
                                 DD_CharIsDigit(tokenizer->at[j]) ||
                                 tokenizer->at[j] == '_');
                token.kind = DD_TokenKind_Identifier;
                token.string.str = tokenizer->at + i;
                token.string.size = j - i;
                token.outer_string = token.string;
                break;
            }
            
            // NOTE(rjf): Numeric Literal
            else if(DD_CharIsDigit(tokenizer->at[i]) ||
                    (tokenizer->at + 1 < one_past_last &&
                     tokenizer->at[i] == '-' && DD_CharIsDigit(tokenizer->at[i+1])))
            {
                DD_TokenizerLoop(j, i+1,
                                 DD_CharIsAlpha(tokenizer->at[j]) ||
                                 DD_CharIsDigit(tokenizer->at[j]) ||
                                 tokenizer->at[j] == '.');
                token.kind = DD_TokenKind_NumericLiteral;
                token.string.str = tokenizer->at + i;
                token.string.size = j - i;
                token.outer_string = token.string;
                break;
            }
            
            // NOTE(rjf): Symbol
            else if(DD_CharIsSymbol(tokenizer->at[i]))
            {
                token.kind = DD_TokenKind_Symbol;
                token.string.str = tokenizer->at + i;
                token.string.size = 1;
                token.outer_string = token.string;
                break;
            }
            
            // NOTE(rjf): Multi-Line String Literal
            else if(tokenizer->at+i+2  < one_past_last &&
                    tokenizer->at[i] == '"' && tokenizer->at[i+1] == '"' && tokenizer->at[i+2] == '"')
            {
                DD_TokenizerLoop(j, i+3, !(tokenizer->at + j + 2 < one_past_last &&
                                           tokenizer->at[j] == '"' && tokenizer->at[j+1] == '"' && tokenizer->at[j+2] == '"'));
                token.kind = DD_TokenKind_StringLiteral;
                token.string.str = tokenizer->at + i + 3;
                token.string.size = j - (i + 3);
                token.outer_string.str = tokenizer->at + i;
                token.outer_string.size = (j + 3) - i;
                break;
            }
            
            // NOTE(rjf): Single-Line String Literal
            else if(tokenizer->at[i] == '"')
            {
                DD_TokenizerLoop(j, i+1, tokenizer->at[j] != '"');
                token.kind = DD_TokenKind_StringLiteral;
                token.string.str = tokenizer->at + i + 1;
                token.string.size = j - (i + 1);
                token.outer_string.str = tokenizer->at + i;
                token.outer_string.size = (j - i) + 1;
                break;
            }
            
            // NOTE(rjf): Single-Line Char Literal
            else if(tokenizer->at[i] == '\'')
            {
                DD_TokenizerLoop(j, i+1, tokenizer->at[j] != '\'');
                token.kind = DD_TokenKind_CharLiteral;
                token.string.str = tokenizer->at + i + 1;
                token.string.size = j - (i + 1);
                token.outer_string.str = tokenizer->at + i;
                token.outer_string.size = (j - i) + 1;
                break;
            }
            
            // NOTE(rjf): Multi-Line Char Literal
            else if(tokenizer->at+i+2  < one_past_last &&
                    tokenizer->at[i] == '\'' && tokenizer->at[i+1] == '\'' && tokenizer->at[i+2] == '\'')
            {
                DD_TokenizerLoop(j, i+3, !(tokenizer->at + j + 2 < one_past_last &&
                                           tokenizer->at[j] == '\'' && tokenizer->at[j+1] == '\'' && tokenizer->at[j+2] == '\''));
                token.kind = DD_TokenKind_StringLiteral;
                token.string.str = tokenizer->at + i + 3;
                token.string.size = j - (i + 3);
                token.outer_string.str = tokenizer->at + i;
                token.outer_string.size = (j + 3) - i;
                break;
            }
            
#undef DD_TokenizerLoop
        }
        
    }
    return token;
}

DD_FUNCTION_IMPL DD_b32
DD_Tokenizer_PeekMatch(DD_Tokenizer *tokenizer, DD_String8 string)
{
    int result = 0;
    DD_Token token = DD_Tokenizer_Peek(tokenizer);
    if(DD_StringMatch(token.string, string, 0))
    {
        result = 1;
    }
    return result;
}

DD_PRIVATE_FUNCTION_IMPL void
_DD_Tokenizer_Bump(DD_Tokenizer *tokenizer, DD_Token token)
{
    tokenizer->at = token.outer_string.str + token.outer_string.size;
}

DD_FUNCTION_IMPL void
DD_Tokenizer_Next(DD_Tokenizer *tokenizer)
{
    DD_Token token = DD_Tokenizer_Peek(tokenizer);
    if(token.kind)
    {
        _DD_Tokenizer_Bump(tokenizer, token);
    }
}

DD_FUNCTION_IMPL DD_b32
DD_Tokenizer_Require(DD_Tokenizer *tokenizer, DD_String8 string)
{
    int result = 0;
    DD_Token token = DD_Tokenizer_Peek(tokenizer);
    if(DD_StringMatch(token.string, string, 0))
    {
        result = 1;
        _DD_Tokenizer_Bump(tokenizer, token);
    }
    return result;
}

DD_FUNCTION_IMPL DD_b32
DD_Tokenizer_RequireKind(DD_Tokenizer *tokenizer, DD_TokenKind kind, DD_Token *out_token)
{
    int result = 0;
    DD_Token token = DD_Tokenizer_Peek(tokenizer);
    if(token.kind == kind)
    {
        result = 1;
        _DD_Tokenizer_Bump(tokenizer, token);
        if(out_token)
        {
            *out_token = token;
        }
    }
    return result;
}

DD_FUNCTION_IMPL DD_b32
DD_NodeIsNil(DD_Node *node)
{
    return node == &_dd_nil_node;
}

DD_FUNCTION_IMPL DD_Node *
DD_NilNode(void) { return &_dd_nil_node; }

DD_FUNCTION_IMPL DD_Node *
DD_MakeNode(DD_NodeKind kind, DD_String8 file, DD_u64 line, DD_Token token)
{
    DD_Node *node = calloc(1, sizeof(*node));
    if(node)
    {
        node->kind = kind;
        node->string = token.string;
        node->whole_string = token.outer_string;
        node->file = file;
        node->line = line;
    }
    else
    {
        node = DD_NilNode();
    }
    return node;
}

DD_FUNCTION_IMPL DD_Node *
DD_MakeNode_Tokenizer(DD_NodeKind kind, DD_Tokenizer *tokenizer, DD_Token token)
{
    return DD_MakeNode(kind, tokenizer->filename, tokenizer->at_line, token);
}

DD_FUNCTION_IMPL void
DD_PushNodeToList(DD_NodeList *list, DD_Node *parent, DD_Node *node)
{
    if(!DD_NodeIsNil(node) && !DD_NodeIsNil(parent))
    {
        if(list->last == 0)
        {
            list->first = list->last = node;
            node->next = node->prev = 0;
        }
        else
        {
            list->last->next = node;
            node->prev = list->last;
            list->last = list->last->next;
        }
        node->parent = parent;
    }
}

DD_FUNCTION void
DD_PushListToList(DD_NodeList *list, DD_Node *parent, DD_NodeList to_push)
{
    if(to_push.first && to_push.last)
    {
        for(DD_Node *node = to_push.first; node; node = node->next)
        {
            if(!DD_NodeIsNil(node))
            {
                node->parent = parent;
            }
        }
        if(list->last == 0)
        {
            *list = to_push;
        }
        else
        {
            list->last->next = to_push.first;
            list->last = to_push.last;
        }
    }
}

DD_FUNCTION_IMPL void
DD_PushChild(DD_Node *parent, DD_Node *new_child)
{
    DD_PushNodeToList(&parent->children, parent, new_child);
}

DD_FUNCTION_IMPL void
DD_PushTag(DD_Node *node, DD_Node *tag)
{
    DD_PushNodeToList(&node->tags, node, tag);
}

DD_FUNCTION void
DD_PushChildren(DD_Node *parent, DD_NodeList children)
{
    DD_PushListToList(&parent->children, parent, children);
}

DD_FUNCTION void
DD_PushTags(DD_Node *node, DD_NodeList tags)
{
    DD_PushListToList(&node->tags, node, tags);
}

DD_FUNCTION_IMPL DD_ParseCtx
DD_Parse_Begin(void)
{
    DD_ParseCtx ctx = {0};
    return ctx;
}

DD_FUNCTION_IMPL void
DD_Parse_Filename(DD_ParseCtx *ctx, DD_String8 filename)
{
    DD_String8 file_contents = DD_LoadEntireFile(filename);
    DD_Parse_Tokenizer(ctx, DD_Tokenizer_Start(filename, file_contents));
}

DD_FUNCTION_IMPL void
DD_Parse_String(DD_ParseCtx *ctx, DD_String8 filename, DD_String8 string)
{
    DD_Parse_Tokenizer(ctx, DD_Tokenizer_Start(filename, string));
}

DD_PRIVATE_FUNCTION_IMPL void
_DD_Error(DD_ParseCtx *ctx, DD_Tokenizer *tokenizer, char *fmt, ...)
{
    DD_Error *error = calloc(1, sizeof(*error));
    error->filename = tokenizer->filename;
    error->line = tokenizer->at_line;
    va_list args;
    va_start(args, fmt);
    error->string = DD_PushStringFV(fmt, args);
    va_end(args);
}

typedef DD_u32 _DD_ParseSetFlags;
enum
{
    _DD_ParseSetFlag_Paren    = (1<<0),
    _DD_ParseSetFlag_Brace    = (1<<1),
    _DD_ParseSetFlag_Bracket  = (1<<2),
    _DD_ParseSetFlag_Implicit = (1<<3),
};
DD_PRIVATE_FUNCTION_IMPL DD_NodeList _DD_ParseSet(DD_ParseCtx *ctx, DD_Tokenizer *tokenizer, DD_Node *parent, _DD_ParseSetFlags flags);
DD_PRIVATE_FUNCTION_IMPL DD_NodeList _DD_ParseTagList(DD_ParseCtx *ctx, DD_Tokenizer *tokenizer);

DD_FUNCTION_IMPL DD_Node *
DD_Parse(DD_ParseCtx *ctx, DD_Tokenizer *tokenizer)
{
    DD_Node *result = 0;
    DD_NodeList tags = _DD_ParseTagList(ctx, tokenizer);
    DD_Token token = {0};
    
    // NOTE(rjf): Identifier
    if(DD_Tokenizer_RequireKind(tokenizer, DD_TokenKind_Identifier, &token))
    {
        // NOTE(rjf): Named Set
        if(DD_Tokenizer_Require(tokenizer, DD_S8Lit(":")))
        {
            result = DD_MakeNode_Tokenizer(DD_NodeKind_Set, tokenizer, token);
            result->children = _DD_ParseSet(ctx, tokenizer, result,
                                            _DD_ParseSetFlag_Paren   |
                                            _DD_ParseSetFlag_Brace   |
                                            _DD_ParseSetFlag_Bracket |
                                            _DD_ParseSetFlag_Implicit);
            goto end_parse;
        }
        
        // NOTE(rjf): Plain identifier
        result = DD_MakeNode_Tokenizer(DD_NodeKind_Identifier, tokenizer, token);
        goto end_parse;
    }
    
    // NOTE(rjf): Unnamed Sets
    else if(DD_Tokenizer_PeekMatch(tokenizer, DD_S8Lit("(")) ||
            DD_Tokenizer_PeekMatch(tokenizer, DD_S8Lit("{")) ||
            DD_Tokenizer_PeekMatch(tokenizer, DD_S8Lit("[")))
    {
        result = DD_MakeNode_Tokenizer(DD_NodeKind_Set, tokenizer, DD_TokenZero());
        result->children = _DD_ParseSet(ctx, tokenizer, result,
                                        _DD_ParseSetFlag_Paren   |
                                        _DD_ParseSetFlag_Brace   |
                                        _DD_ParseSetFlag_Bracket);
        goto end_parse;
    }
    
    // NOTE(rjf): Numeric Literal
    else if(DD_Tokenizer_RequireKind(tokenizer, DD_TokenKind_NumericLiteral, &token))
    {
        result = DD_MakeNode_Tokenizer(DD_NodeKind_NumericLiteral, tokenizer, token);
        goto end_parse;
    }
    
    // NOTE(rjf): String Literal
    else if(DD_Tokenizer_RequireKind(tokenizer, DD_TokenKind_StringLiteral, &token))
    {
        result = DD_MakeNode_Tokenizer(DD_NodeKind_StringLiteral, tokenizer, token);
        goto end_parse;
    }
    
    // NOTE(rjf): Char Literal
    else if(DD_Tokenizer_RequireKind(tokenizer, DD_TokenKind_CharLiteral, &token))
    {
        result = DD_MakeNode_Tokenizer(DD_NodeKind_CharLiteral, tokenizer, token);
        goto end_parse;
    }
    
    // NOTE(rjf): Symbol
    else if(DD_Tokenizer_RequireKind(tokenizer, DD_TokenKind_Symbol, &token))
    {
        result = DD_MakeNode_Tokenizer(DD_NodeKind_Symbol, tokenizer, token);
        goto end_parse;
    }
    
    end_parse:;
    if(result)
    {
        DD_PushListToList(&result->tags, result, tags);
    }
    return result;
}

DD_PRIVATE_FUNCTION_IMPL DD_NodeList
_DD_ParseSet(DD_ParseCtx *ctx, DD_Tokenizer *tokenizer, DD_Node *parent, _DD_ParseSetFlags flags)
{
    DD_NodeList list = {0};
    
    DD_b32 brace = 0;
    DD_b32 paren = 0;
    DD_b32 bracket = 0;
    DD_b32 terminate_with_separator = !!(flags & _DD_ParseSetFlag_Implicit);
    
    if(flags & _DD_ParseSetFlag_Brace && DD_Tokenizer_Require(tokenizer, DD_S8Lit("{")))
    {
        brace = 1;
        terminate_with_separator = 0;
    }
    else if(flags & _DD_ParseSetFlag_Paren && DD_Tokenizer_Require(tokenizer, DD_S8Lit("(")))
    {
        paren = 1;
        terminate_with_separator = 0;
    }
    else if(flags & _DD_ParseSetFlag_Bracket && DD_Tokenizer_Require(tokenizer, DD_S8Lit("[")))
    {
        bracket = 1;
        terminate_with_separator = 0;
    }
    
    // NOTE(rjf): Parse children.
    if(brace || paren || bracket || terminate_with_separator)
    {
        for(DD_u64 child_idx = 0;; child_idx += 1)
        {
            DD_Node *child = DD_Parse(ctx, tokenizer);
            if(child)
            {
                DD_PushNodeToList(&list, parent, child);
            }
            
            // NOTE(rjf): Separators.
            {
                DD_b32 result = 0;
                result |= !!DD_Tokenizer_Require(tokenizer, DD_S8Lit(","));
                result |= !!DD_Tokenizer_Require(tokenizer, DD_S8Lit(";"));
                result |= !!DD_Tokenizer_Require(tokenizer, DD_S8Lit("->"));
                
                if(result && terminate_with_separator)
                {
                    goto end_parse;
                }
                
                // NOTE(rjf): Also use newlines as a termination separator.
                if(terminate_with_separator)
                {
                    for(DD_u64 i = 0; tokenizer->at + i < tokenizer->file_contents.str + tokenizer->file_contents.size; i += 1)
                    {
                        if(tokenizer->at[i] == '\n')
                        {
                            goto end_parse;
                        }
                        else if(tokenizer->at[i] > 32)
                        {
                            break;
                        }
                    }
                }
            }
            
            if(brace && DD_Tokenizer_Require(tokenizer, DD_S8Lit("}")))
            {
                goto end_parse;
            }
            else if(paren && DD_Tokenizer_Require(tokenizer, DD_S8Lit(")")))
            {
                goto end_parse;
            }
            else if(bracket && DD_Tokenizer_Require(tokenizer, DD_S8Lit("]")))
            {
                goto end_parse;
            }
            else if(child == 0)
            {
                if(brace)   { _DD_Error(ctx, tokenizer, "Missing }."); }
                if(paren)   { _DD_Error(ctx, tokenizer, "Missing )."); }
                if(bracket) { _DD_Error(ctx, tokenizer, "Missing ]."); }
                goto end_parse;
            }
        }
    }
    
    end_parse:;
    return list;
}

DD_PRIVATE_FUNCTION_IMPL DD_NodeList
_DD_ParseTagList(DD_ParseCtx *ctx, DD_Tokenizer *tokenizer)
{
    DD_NodeList list = {0};
    
    for(;DD_Tokenizer_Require(tokenizer, DD_S8Lit("@"));)
    {
        DD_Token name = {0};
        if(DD_Tokenizer_RequireKind(tokenizer, DD_TokenKind_Identifier, &name))
        {
            DD_Node *tag = DD_MakeNode_Tokenizer(DD_NodeKind_Tag, tokenizer, name);
            tag->children = _DD_ParseSet(ctx, tokenizer, tag, _DD_ParseSetFlag_Paren);
            DD_PushNodeToList(&list, 0, tag);
        }
        else
        {
            break;
        }
    }
    
    return list;
}

DD_FUNCTION_IMPL void
DD_Parse_Tokenizer(DD_ParseCtx *ctx, DD_Tokenizer tokenizer)
{
    DD_Node *root = DD_MakeNode(DD_NodeKind_Set, tokenizer.filename, 1, DD_TokenZero());
    DD_PushNodeToList(&ctx->roots, 0, root);
    for(;;)
    {
        DD_Node *node = DD_Parse(ctx, &tokenizer);
        if(node == 0)
        {
            break;
        }
        DD_PushNodeToList(&root->children, root, node);
    }
}

DD_FUNCTION_IMPL DD_ParseResult
DD_Parse_End(DD_ParseCtx *ctx)
{
    DD_ParseResult result = {0};
    result.roots = ctx->roots;
    result.first_error = ctx->first_error;
    return result;
}

DD_FUNCTION_IMPL DD_Node *
DD_NodeInList(DD_NodeList list, DD_String8 string)
{
    DD_Node *result = DD_NilNode();
    if(list.first && list.last)
    {
        for(DD_Node *node = list.first; node; node = node->next)
        {
            if(DD_StringMatch(string, node->string, 0))
            {
                result = node;
                break;
            }
        }
    }
    return result;
}

DD_FUNCTION_IMPL DD_Node *
DD_NthNodeInList(DD_NodeList list, int n)
{
    DD_Node *result = DD_NilNode();
    if(list.first && list.last && n >= 0)
    {
        int idx = 0;
        for(DD_Node *node = list.first; node; node = node->next, idx += 1)
        {
            if(idx == n)
            {
                result = node;
                break;
            }
        }
    }
    return result;
}

DD_FUNCTION_IMPL int
DD_IndexFromNode(DD_Node *node)
{
    int idx = 0;
    if(node && !DD_NodeIsNil(node))
    {
        for(DD_Node *last = node->prev; last; last = last->prev, idx += 1);
    }
    return idx;
}

DD_FUNCTION_IMPL DD_Node *
DD_NextNodeSibling(DD_Node *last, DD_String8 string)
{
    DD_Node *result = DD_NilNode();
    if(last)
    {
        for(DD_Node *node = last->next; node; node = node->next)
        {
            if(DD_StringMatch(string, node->string, 0))
            {
                result = node;
                break;
            }
        }
    }
    return result;
}

DD_FUNCTION_IMPL DD_Node *
DD_TagOnNode(DD_Node *node, DD_String8 tag_string)
{
    DD_Node *result = DD_NilNode();
    if(node)
    {
        result = DD_NodeInList(node->tags, tag_string);
    }
    return result;
}

DD_FUNCTION_IMPL DD_Node *
DD_ChildOnNode(DD_Node *node, DD_String8 child_string)
{
    DD_Node *result = DD_NilNode();
    if(node)
    {
        result = DD_NodeInList(node->children, child_string);
    }
    return result;
}

DD_FUNCTION_IMPL DD_Node *
DD_NthTagArg(DD_Node *node, DD_String8 tag_string, int n)
{
    DD_Node *result = DD_NilNode();
    if(n >= 0)
    {
        DD_Node *tag = DD_TagOnNode(node, tag_string);
        result = DD_NthNodeInList(tag->children, n);
    }
    return result;
}

DD_FUNCTION_IMPL DD_Node *
DD_NthChild(DD_Node *node, int n)
{
    DD_Node *result = DD_NilNode();
    if(node && n >= 0)
    {
        result = DD_NthNodeInList(node->children, n);
    }
    return result;
}

static DD_Expr _dd_nil_expr = {0};
static DD_Type _dd_nil_type = {0};

DD_FUNCTION_IMPL DD_Expr *
DD_NilExpr(void)
{
    return &_dd_nil_expr;
}

DD_FUNCTION_IMPL DD_Type *
DD_NilType(void)
{
    return &_dd_nil_type;
}

DD_FUNCTION_IMPL DD_Expr *
DD_MakeExpr(DD_Node *node, DD_ExprKind kind, DD_Expr *left, DD_Expr *right)
{
    DD_Expr *expr = calloc(1, sizeof(*expr));
    if(expr)
    {
        expr->node = node;
        expr->kind = kind;
        expr->sub[0] = left;
        expr->sub[1] = right;
    }
    else
    {
        expr = DD_NilExpr();
    }
    return expr;
}

DD_FUNCTION_IMPL DD_Type *
DD_MakeType(DD_Node *node, DD_TypeKind kind, DD_Type *parent, DD_Type *operand, DD_Expr *expr)
{
    DD_Type *type = calloc(1, sizeof(*type));
    if(type)
    {
        type->node = node ? node : DD_NilNode();
        type->kind = kind;
        type->parent = parent;
        type->operand = operand;
        type->expr = expr;
    }
    else
    {
        type = DD_NilType();
    }
    return type;
}

typedef struct _DD_NodeParseCtx _DD_NodeParseCtx;
struct _DD_NodeParseCtx
{
    DD_Node *at;
};

DD_PRIVATE_FUNCTION_IMPL DD_b32
_DD_RequireNodeKind(_DD_NodeParseCtx *ctx, DD_NodeKind kind, DD_Node **out)
{
    DD_b32 result = 0;
    if(ctx->at && ctx->at->kind == kind)
    {
        result = 1;
        if(out)
        {
            *out = ctx->at;
        }
        ctx->at = ctx->at->next;
    }
    return result;
}

DD_PRIVATE_FUNCTION_IMPL DD_b32
_DD_RequireNode(_DD_NodeParseCtx *ctx, DD_String8 string)
{
    DD_b32 result = 0;
    if(ctx->at && DD_StringMatch(string, ctx->at->string, 0))
    {
        result = 1;
        ctx->at = ctx->at->next;
    }
    return result;
}

DD_FUNCTION_IMPL DD_Expr *
DD_ParseAsExpr(DD_Node *node)
{
    DD_Expr *expr = DD_NilExpr();
    
    return expr;
}

DD_FUNCTION_IMPL DD_Type *
DD_ParseAsType(DD_Node *node)
{
    DD_Type *first_type = 0;
    DD_Type *last_type = 0;
#define _DD_PushType(p) { if(last_type) { last_type->operand = (p); last_type = last_type->operand; } else { first_type = last_type = (p); } }
    _DD_NodeParseCtx ctx = { node->children.first };
    for(;;)
    {
        DD_Node *atom = 0;
        DD_Node *expr = 0;
        if(_DD_RequireNode(&ctx, DD_S8Lit("*")))
        {
            DD_Type *ptr = DD_MakeType(0, DD_TypeKind_Pointer, last_type, 0, 0);
            _DD_PushType(ptr);
        }
        else if(_DD_RequireNodeKind(&ctx, DD_NodeKind_Set, &expr))
        {
            DD_Type *array = DD_MakeType(0, DD_TypeKind_Array, last_type, 0, DD_ParseAsExpr(expr));
            _DD_PushType(array);
        }
        else if(_DD_RequireNodeKind(&ctx, DD_NodeKind_Identifier, &atom))
        {
            DD_Type *atom_type = DD_MakeType(atom, DD_TypeKind_Atom, last_type, 0, 0);
            _DD_PushType(atom_type);
        }
        else
        {
            break;
        }
    }
#undef _DD_PushType
    if(first_type == 0)
    {
        first_type = DD_NilType();
    }
    return first_type;
}

DD_FUNCTION_IMPL void
DD_OutputTree(FILE *file, DD_Node *node)
{
    for(DD_Node *tag = node->tags.first; tag; tag = tag->next)
    {
        fprintf(file, "@%.*s", DD_StringExpand(tag->string));
        if(tag->children.first)
        {
            fprintf(file, "(");
            for(DD_Node *child = tag->children.first; child; child = child->next)
            {
                DD_OutputTree(file, child);
                fprintf(file, ", ");
            }
            fprintf(file, ")\n");
        }
        else
        {
            fprintf(file, " ");
        }
    }
    
    fprintf(file, "%.*s", DD_StringExpand(node->whole_string));
    if(node->children.first)
    {
        fprintf(file, ":\n{\n");
        for(DD_Node *child = node->children.first; child; child = child->next)
        {
            DD_OutputTree(file, child);
            fprintf(file, ",\n");
        }
        fprintf(file, "}\n");
    }
    else
    {
        fprintf(file, " ");
    }
}

DD_FUNCTION_IMPL void
DD_OutputExpr(FILE *file, DD_Expr *expr)
{
    
}

DD_FUNCTION_IMPL void
DD_OutputTree_C_String(FILE *file, DD_Node *node)
{
    fprintf(file, "\"");
    for(DD_u64 i = 0; i < node->string.size; i += 1)
    {
        if(node->string.str[i] == '\n')
        {
            fprintf(file, "\\n\"\n\"");
        }
        else
        {
            fprintf(file, "%c", node->string.str[i]);
        }
    }
    fprintf(file, "\"");
}

DD_FUNCTION_IMPL void
DD_OutputTree_C_Struct(FILE *file, DD_Node *node)
{
    if(node)
    {
        fprintf(file, "typedef struct %.*s %.*s;\n",
                DD_StringExpand(node->string),
                DD_StringExpand(node->string));
        fprintf(file, "struct %.*s\n{\n", DD_StringExpand(node->string));
        for(DD_Node *child = node->children.first; child; child = child->next)
        {
            DD_OutputTree_C_Decl(file, child);
        }
        fprintf(file, "};\n\n");
    }
}

DD_FUNCTION_IMPL void
DD_OutputTree_C_Decl(FILE *file, DD_Node *node)
{
    if(node)
    {
        DD_Type *type = DD_ParseAsType(node);
        DD_OutputType_C_PreArray(file, type);
        fprintf(file, " %.*s", DD_StringExpand(node->string));
        DD_OutputType_C_Array(file, type);
    }
}

DD_FUNCTION_IMPL void
DD_OutputType_C_PreArray(FILE *file, DD_Type *type)
{
    for(DD_Type *base_type = type; base_type; base_type = base_type->operand)
    {
        if(base_type->kind == DD_TypeKind_Atom)
        {
            fprintf(file, "%.*s", DD_StringExpand(base_type->node->string));
            for(DD_Type *op = base_type->parent; op; op = op->parent)
            {
                switch(op->kind)
                {
                    case DD_TypeKind_Pointer: fprintf(file, "*"); break;
                    default: break;
                }
            }
            break;
        }
    }
}

DD_FUNCTION_IMPL void
DD_OutputType_C_Array(FILE *file, DD_Type *type)
{
    for(DD_Type *t = type; t; t = t->operand)
    {
        if(t->kind == DD_TypeKind_Array)
        {
            fprintf(file, "[");
            DD_OutputExpr(file, t->expr);
            fprintf(file, "]");
        }
    }
}

DD_FUNCTION_IMPL DD_CommandLine
DD_CommandLine_Start(int argument_count, char **arguments)
{
    DD_CommandLine cmdln = {0};
    cmdln.arguments = calloc(sizeof(DD_String8), argument_count-1);
    for(int i = 1; i < argument_count; i += 1)
    {
        cmdln.arguments[i-1] = DD_PushStringF("%s", arguments[i]);
    }
    cmdln.argument_count = argument_count-1;
    return cmdln;
}

DD_FUNCTION_IMPL DD_b32
DD_CommandLine_Flag(DD_CommandLine *cmdln, DD_String8 string)
{
    DD_b32 result = 0;
    for(int i = 0; i < cmdln->argument_count; i += 1)
    {
        if(DD_StringMatch(string, cmdln->arguments[i], 0))
        {
            result = 1;
            cmdln->arguments[i].str = 0;
            cmdln->arguments[i].size = 0;
            break;
        }
    }
    return result;
}

DD_FUNCTION_IMPL DD_b32
DD_CommandLine_FlagStrings(DD_CommandLine *cmdln, DD_String8 string, int out_count, DD_String8 *out)
{
    DD_b32 result = 0;
    for(int i = 0; i < cmdln->argument_count; i += 1)
    {
        if(DD_StringMatch(string, cmdln->arguments[i], 0))
        {
            cmdln->arguments[i].str = 0;
            cmdln->arguments[i].size = 0;
            if(cmdln->argument_count > i + out_count)
            {
                for(int out_idx = 0; out_idx < out_count; out_idx += 1)
                {
                    out[out_idx] = cmdln->arguments[i+out_idx+1];
                    cmdln->arguments[i+out_idx+1].str = 0;
                    cmdln->arguments[i+out_idx+1].size = 0;
                }
                result = 1;
                break;
            }
        }
    }
    return result;
}

DD_FUNCTION_IMPL DD_b32
DD_CommandLine_FlagIntegers(DD_CommandLine *cmdln, DD_String8 string, int out_count, DD_i64 *out)
{
    DD_b32 result = 0;
    for(int i = 0; i < cmdln->argument_count; i += 1)
    {
        if(DD_StringMatch(string, cmdln->arguments[i], 0))
        {
            cmdln->arguments[i].str = 0;
            cmdln->arguments[i].size = 0;
            if(cmdln->argument_count > i + out_count)
            {
                for(int out_idx = 0; out_idx < out_count; out_idx += 1)
                {
                    out[out_idx] = (DD_i64)DD_IntFromString(cmdln->arguments[i+out_idx+1]);
                    cmdln->arguments[i+out_idx+1].str = 0;
                    cmdln->arguments[i+out_idx+1].size = 0;
                }
                result = 1;
                break;
            }
        }
    }
    return result;
}

DD_FUNCTION_IMPL DD_b32
DD_CommandLine_FlagString(DD_CommandLine *cmdln, DD_String8 string, DD_String8 *out)
{
    return DD_CommandLine_FlagStrings(cmdln, string, 1, out);
}

DD_FUNCTION_IMPL DD_b32
DD_CommandLine_FlagInteger(DD_CommandLine *cmdln, DD_String8 string, DD_i64 *out)
{
    return DD_CommandLine_FlagIntegers(cmdln, string, 1, out);
}

DD_FUNCTION_IMPL DD_b32
DD_CommandLine_Increment(DD_CommandLine *cmdln, DD_String8 **string_ptr)
{
    DD_b32 result = 0;
    DD_String8 *string = *string_ptr;
    if(string == 0)
    {
        for(int i = 0; i < cmdln->argument_count; i += 1)
        {
            if(cmdln->arguments[i].str)
            {
                string = &cmdln->arguments[i];
                break;
            }
        }
    }
    else
    {
        int idx = (int)(string - cmdln->arguments);
        string = 0;
        for(int i = idx+1; i < cmdln->argument_count; i += 1)
        {
            if(cmdln->arguments[i].str)
            {
                string = &cmdln->arguments[i];
                break;
            }
        }
    }
    *string_ptr = string;
    result = !!string;
    return result;
}

DD_FUNCTION_IMPL DD_String8
DD_LoadEntireFile(DD_String8 filename)
{
    DD_String8 file_contents = {0};
    FILE *file = fopen(DD_CStringFromString8(filename), "r");
    if(file)
    {
        fseek(file, 0, SEEK_END);
        DD_u64 file_size = ftell(file);
        fseek(file, 0, SEEK_SET);
        file_contents.str = calloc(1, file_size+1);
        if(file_contents.str)
        {
            file_contents.size = file_size;
            fread(file_contents.str, 1, file_size, file);
        }
        fclose(file);
    }
    return file_contents;
}

#if DD_OS
DD_FUNCTION_IMPL DD_b32
DD_FileIter_Increment(DD_FileIter *it, DD_String8 path, DD_FileInfo *out_info)
{
    return _DD_OS_IMPL_FileIter_Increment(it, path, out_info);
}
#endif