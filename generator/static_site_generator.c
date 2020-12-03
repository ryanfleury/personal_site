#include "dd.h"
#include "dd.c"

typedef struct SiteInfo SiteInfo;
struct SiteInfo
{
    DD_Node *title;
    DD_Node *desc;
    DD_Node *canonical_url;
    DD_Node *author;
    DD_Node *twitter_handle;
    DD_Node *link_dictionary;
    DD_Node *header;
    DD_Node *footer;
    DD_Node *style;
};

typedef struct PageInfo PageInfo;
struct PageInfo
{
    DD_Node *title;
    DD_Node *desc;
    DD_Node *date;
    DD_Node *parent;
    DD_Node *header_image;
};

static PageInfo ParsePageInfo(DD_Node *page);
static SiteInfo ParseSiteInfo(DD_Node *site);
static DD_String8 MakeDateString(DD_Node *date);
static void GeneratePageContent(DD_NodeTable *index_table, SiteInfo *site_info, PageInfo *page_info, FILE *file, DD_Node *node);

int main(int argument_count, char **arguments)
{
    
    //~ NOTE(rjf): Parse command line arguments.
    DD_String8 site_info_path = {0};
    DD_String8 page_dir_path = {0};
    DD_CommandLine cmdln = DD_CommandLine_Start(argument_count, arguments);
    if(!DD_CommandLine_FlagString(&cmdln, DD_S8Lit("--siteinfo"), &site_info_path) ||
       !DD_CommandLine_FlagString(&cmdln, DD_S8Lit("--pagedir"), &page_dir_path))
    {
        fprintf(stderr, "USAGE: %s --siteinfo <path to site info file> --pagedir <path to directory with pages> ...\n", arguments[0]);
        goto end;
    }
    
    //~ NOTE(rjf): Load JS.
    DD_String8 js_string = DD_LoadEntireFile(DD_PushStringF("%.*s/site.js", DD_StringExpand(page_dir_path)));
    
    //~ NOTE(rjf): Parse site info.
    SiteInfo site_info = {0};
    {
        printf("Parsing site metadata at \"%.*s\"...\n", DD_StringExpand(site_info_path));
        DD_ParseCtx ctx = DD_Parse_Begin();
        DD_Parse_Filename(&ctx, site_info_path);
        DD_ParseResult parse = DD_Parse_End(&ctx);
        site_info = ParseSiteInfo(parse.roots.first);
    }
    
    //~ NOTE(rjf): Parse pages.
    DD_ParseResult parse = {0};
    {
        DD_ParseCtx ctx = DD_Parse_Begin();
        DD_FileInfo file_info = {0};
        printf("Searching for site pages at \"%.*s\"...\n", DD_StringExpand(page_dir_path));
        for(DD_FileIter it = {0}; DD_FileIter_Increment(&it, page_dir_path, &file_info);)
        {
            if(DD_StringMatch(file_info.extension, DD_S8Lit("dd"), DD_StringMatchFlag_CaseInsensitive) &&
               !DD_StringMatch(DD_WithoutFolder(DD_WithoutExtension(file_info.path)),
                               DD_WithoutFolder(DD_WithoutExtension(site_info_path)),
                               DD_StringMatchFlag_CaseInsensitive))
            {
                printf("Processing site page at \"%.*s\"...\n", DD_StringExpand(file_info.path));
                DD_String8 folder = DD_FolderString(page_dir_path);
                DD_String8 path = DD_PushStringF("%.*s/%.*s",
                                                 DD_StringExpand(folder),
                                                 DD_StringExpand(file_info.path));
                printf("%.*s\n", DD_StringExpand(path));
                DD_Parse_Filename(&ctx, path);
            }
        }
        parse = DD_Parse_End(&ctx);
    }
    
    //~ NOTE(rjf): Generate index table.
    DD_NodeTable index_table = {0};
    {
        for(DD_Node *root = parse.roots.first; root; root = root->next)
        {
            for(DD_Node *node = root->children.first; node; node = node->next)
            {
                if(node->children.first && DD_StringMatch(node->string, DD_S8Lit("index"), DD_StringMatchFlag_CaseInsensitive))
                {
                    for(DD_Node *index_string = node->children.first; index_string; index_string = index_string->next)
                    {
                        DD_NodeTable_Insert(&index_table, DD_NodeTableCollisionRule_Chain, index_string->string, root);
                    }
                    goto end_index_build;
                }
            }
            end_index_build:;
        }
    }
    
    //~ NOTE(rjf): Generate stylesheet.
    if(site_info.style)
    {
        FILE *file = fopen("style.css", "w");
        if(file)
        {
            fprintf(file, "%.*s", DD_StringExpand(site_info.style->string));
            fclose(file);
        }
    }
    
    //~ NOTE(rjf): Generate JS.
    {
        FILE *file = fopen("site.js", "w");
        if(file)
        {
            fprintf(file, "%.*s", DD_StringExpand(js_string));
            fclose(file);
        }
    }
    
    //~ NOTE(rjf): Generate files for all roots.
    for(DD_Node *root = parse.roots.first; root; root = root->next)
    {
        PageInfo page_info = ParsePageInfo(root);
        
        DD_String8 name_without_extension = DD_WithoutFolder(DD_WithoutExtension(root->file));
        FILE *file = fopen(DD_PushCStringF("%.*s.html", DD_StringExpand(name_without_extension)), "w");
        if(file)
        {
            fprintf(file, "<!DOCTYPE html>\n");
            fprintf(file, "<html lang=\"en\">\n");
            
            DD_String8 site_title      = site_info.title ? site_info.title->string : DD_ZeroString();
            DD_String8 title           = page_info.title ? page_info.title->string : DD_ZeroString();
            DD_String8 url             = site_info.canonical_url ? site_info.canonical_url->string : DD_ZeroString();
            DD_String8 author          = site_info.author ? site_info.author->string : DD_ZeroString();
            DD_String8 twitter_handle  = site_info.twitter_handle ? site_info.twitter_handle->string : DD_ZeroString();
            
            // NOTE(rjf): Generate heading.
            {
                fprintf(file, "<head>\n");
                fprintf(file, "<meta charset=\"utf-8\">\n");
                fprintf(file, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><meta name=\"author\" content=\"%.*s\">\n", DD_StringExpand(author));
                fprintf(file, "<meta property=\"og:title\" content=\"%.*s\">\n", DD_StringExpand(title));
                fprintf(file, "<meta name=\"twitter:title\" content=\"%.*s\">\n", DD_StringExpand(title));
                fprintf(file, "<link rel=\"canonical\" href=\"%.*s\">\n", DD_StringExpand(url));
                fprintf(file, "<meta property=\"og:type\" content=\"website\">\n");
                fprintf(file, "<meta property=\"og:url\" content=\"%.*s\">\n", DD_StringExpand(url));
                fprintf(file, "<meta property=\"og:site_name\" content=\"%.*s\">\n", DD_StringExpand(site_title));
                fprintf(file, "<meta name=\"twitter:card\" content=\"summary\">\n");
                fprintf(file, "<meta name=\"twitter:site\" content=\"%.*s\">\n", DD_StringExpand(twitter_handle));
                fprintf(file, "<link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">\n");
                fprintf(file, "<script src=\"site.js\"></script>\n");
                if(!DD_StringIsZero(title))
                {
                    if(!DD_StringIsZero(site_title))
                    {
                        fprintf(file, "<title>%.*s | %.*s</title>\n", DD_StringExpand(title), DD_StringExpand(site_title));
                    }
                    else
                    {
                        fprintf(file, "<title>%.*s</title>\n", DD_StringExpand(title));
                    }
                }
                else if(!DD_StringIsZero(site_title))
                {
                    fprintf(file, "<title>%.*s</title>\n", DD_StringExpand(site_title));
                }
                fprintf(file, "</head>\n");
            }
            
            // NOTE(rjf): Generate body.
            {
                fprintf(file, "<body>\n");
                
                // NOTE(rjf): Generate header.
                if(site_info.header)
                {
                    fprintf(file, "%.*s", DD_StringExpand(site_info.header->string));
                }
                
                fprintf(file, "<div class=\"page_content\">\n");
                
                // NOTE(rjf): Parent page back button.
                if(page_info.parent)
                {
                    fprintf(file, "<div class=\"standalone_link_container\"><a class=\"link\" href=\"%.*s.html\">← Back</a></div>", DD_StringExpand(page_info.parent->string));
                }
                
                // NOTE(rjf): Banner.
                if(page_info.header_image)
                {
                    fprintf(file, "<div class=\"page_banner\" style=\"background-image: url('%.*s');\"></div>",
                            DD_StringExpand(page_info.header_image->string));
                }
                
                // NOTE(rjf): Title.
                if(!DD_StringIsZero(title))
                {
                    fprintf(file, "<h1 class=\"title\">%.*s</h1>", DD_StringExpand(title));
                }
                
                // NOTE(rjf): Main description/subtitle.
                if(page_info.desc)
                {
                    fprintf(file, "<h2 class=\"subtitle\">%.*s</h2>", DD_StringExpand(page_info.desc->string));
                }
                
                // NOTE(rjf): Date.
                if(page_info.date)
                {
                    DD_String8 date_string = MakeDateString(page_info.date);
                    if(!DD_StringIsZero(date_string))
                    {
                        fprintf(file, "<h3 class=\"date\">%.*s</h3>", DD_StringExpand(date_string));
                    }
                }
                
                // NOTE(rjf): The rest of the page content should be generated from the page nodes.
                for(DD_Node *node = root->children.first; node; node = node->next)
                {
                    GeneratePageContent(&index_table, &site_info, &page_info, file, node);
                }
                
                fprintf(file, "</div>\n");
                
                // NOTE(rjf): Generate footer.
                if(site_info.footer)
                {
                    fprintf(file, "%.*s", DD_StringExpand(site_info.footer->string));
                }
                
                fprintf(file, "</body>\n");
            }
            
            fprintf(file, "</html>\n");
            fclose(file);
        }
    }
    
    end:;
    return 0;
}

static PageInfo
ParsePageInfo(DD_Node *page)
{
    PageInfo info = {0};
    for(DD_Node *node = page->children.first; node; node = node->next)
    {
        if(node->kind == DD_NodeKind_Set && node->children.first)
        {
            if(DD_StringMatch(node->string, DD_S8Lit("title"), DD_StringMatchFlag_CaseInsensitive))
            {
                info.title = node->children.first;
            }
            else if(DD_StringMatch(node->string, DD_S8Lit("desc"), DD_StringMatchFlag_CaseInsensitive))
            {
                info.desc = node->children.first;
            }
            else if(DD_StringMatch(node->string, DD_S8Lit("date"), DD_StringMatchFlag_CaseInsensitive))
            {
                info.date = node;
            }
            else if(DD_StringMatch(node->string, DD_S8Lit("parent"), DD_StringMatchFlag_CaseInsensitive))
            {
                info.parent = node->children.first;
            }
            else if(DD_StringMatch(node->string, DD_S8Lit("header_image"), DD_StringMatchFlag_CaseInsensitive))
            {
                info.header_image = node->children.first;
            }
        }
    }
    return info;
}

static SiteInfo
ParseSiteInfo(DD_Node *site)
{
    SiteInfo info = {0};
    for(DD_Node *node = site->children.first; node; node = node->next)
    {
        if(node->kind == DD_NodeKind_Set && node->children.first)
        {
            if(DD_StringMatch(node->string, DD_S8Lit("title"), DD_StringMatchFlag_CaseInsensitive))
            {
                info.title = node->children.first;
            }
            else if(DD_StringMatch(node->string, DD_S8Lit("desc"), DD_StringMatchFlag_CaseInsensitive))
            {
                info.desc = node->children.first;
            }
            else if(DD_StringMatch(node->string, DD_S8Lit("canonical_url"), DD_StringMatchFlag_CaseInsensitive))
            {
                info.canonical_url = node->children.first;
            }
            else if(DD_StringMatch(node->string, DD_S8Lit("author"), DD_StringMatchFlag_CaseInsensitive))
            {
                info.author = node->children.first;
            }
            else if(DD_StringMatch(node->string, DD_S8Lit("twitter_handle"), DD_StringMatchFlag_CaseInsensitive))
            {
                info.twitter_handle = node->children.first;
            }
            else if(DD_StringMatch(node->string, DD_S8Lit("link_dictionary"), DD_StringMatchFlag_CaseInsensitive))
            {
                info.link_dictionary = node;
            }
            else if(DD_StringMatch(node->string, DD_S8Lit("header"), DD_StringMatchFlag_CaseInsensitive))
            {
                info.header = node->children.first;
            }
            else if(DD_StringMatch(node->string, DD_S8Lit("footer"), DD_StringMatchFlag_CaseInsensitive))
            {
                info.footer = node->children.first;
            }
            else if(DD_StringMatch(node->string, DD_S8Lit("style"), DD_StringMatchFlag_CaseInsensitive))
            {
                info.style = node->children.first;
            }
        }
    }
    return info;
}

static DD_String8
MakeDateString(DD_Node *date)
{
    DD_String8 result = {0};
    if(date)
    {
        DD_Node *year = 0;
        DD_Node *month = 0;
        DD_Node *day = 0;
        
        for(DD_Node *child = date->children.first; child; child = child->next)
        {
            if(child->kind == DD_NodeKind_NumericLiteral)
            {
                if      (year  == 0) year  = child;
                else if (month == 0) month = child;
                else if (day   == 0) day   = child;
                else
                {
                    break;
                }
            }
        }
        
        if(year && month && day)
        {
            char *month_names[] =
            {
                "January", "February", "March", "April", "May", "June", "July", "August",
                "September", "October", "November", "December",
            };
            int month_idx = DD_IntFromString(month->string)-1;
            if(month_idx >= 0 && month_idx < sizeof(month_names)/sizeof(month_names[0]))
            {
                result = DD_PushStringF("%.*s %s %.*s",
                                        DD_StringExpand(day->string),
                                        month_names[month_idx],
                                        DD_StringExpand(year->string));
            }
        }
    }
    
    return result;
}

static void
GeneratePageContent(DD_NodeTable *index_table, SiteInfo *site_info, PageInfo *page_info, FILE *file, DD_Node *node)
{
    switch(node->kind)
    {
        case DD_NodeKind_StringLiteral:
        case DD_NodeKind_CharLiteral:
        {
            char *html_tag = "p";
            char *style = "paragraph";
            if(!DD_NodeIsNil(DD_TagOnNode(node, DD_S8Lit("title"))))
            {
                html_tag = "h1";
                style = "title";
            }
            else if(!DD_NodeIsNil(DD_TagOnNode(node, DD_S8Lit("subtitle"))))
            {
                html_tag = "h2";
                style = "subtitle";
            }
            else if(!DD_NodeIsNil(DD_TagOnNode(node, DD_S8Lit("code"))))
            {
                html_tag = "pre";
                style = "code";
            }
            
            DD_String8 splits[] =
            {
                DD_S8Lit("\n\n"),
            };
            DD_String8List strlist = DD_SplitString(node->string, sizeof(splits)/sizeof(splits[0]), splits);
            
            for(DD_String8Node *strnode = strlist.first; strnode; strnode = strnode->next)
            {
                fprintf(file, "<%s class=\"%s\">", html_tag, style);
                for(DD_u64 i = 0; i < strnode->string.size; i += 1)
                {
                    if(strnode->string.str[i] == '@')
                    {
                        DD_ParseCtx ctx = DD_Parse_Begin();
                        DD_Tokenizer tokenizer = DD_Tokenizer_Start(node->file, DD_S8(strnode->string.str + i, strnode->string.size - i));
                        DD_u8 *at = tokenizer.at;
                        DD_Node *substyle = DD_Parse(&ctx, &tokenizer);
                        DD_ParseResult result = DD_Parse_End(&ctx);
                        if(substyle)
                        {
                            if(!DD_NodeIsNil(DD_TagOnNode(substyle, DD_S8Lit("i"))))
                            {
                                fprintf(file, "<i>%.*s</i>", DD_StringExpand(substyle->string));
                            }
                            else if(!DD_NodeIsNil(DD_TagOnNode(substyle, DD_S8Lit("b"))))
                            {
                                fprintf(file, "<strong>%.*s</strong>", DD_StringExpand(substyle->string));
                            }
                            else if(!DD_NodeIsNil(DD_TagOnNode(substyle, DD_S8Lit("code"))))
                            {
                                fprintf(file, "<span class=\"inline_code\">%.*s</span>", DD_StringExpand(substyle->string));
                            }
                            else if(!DD_NodeIsNil(DD_TagOnNode(substyle, DD_S8Lit("link"))))
                            {
                                DD_Node *text = DD_NthChild(substyle, 0);
                                DD_Node *link = DD_NthChild(substyle, 1);
                                fprintf(file, "<a class=\"link\" href=\"%.*s\">%.*s</a>",
                                        DD_StringExpand(link->string),
                                        DD_StringExpand(text->string));
                            }
                        }
                        i += (tokenizer.at - at - 1);
                    }
                    else
                    {
                        DD_b32 dict_word = 0;
                        if(site_info->link_dictionary)
                        {
                            DD_Node *text = DD_NilNode();
                            DD_Node *link = DD_NilNode();
                            for(DD_Node *dict_link = site_info->link_dictionary->children.first;
                                dict_link; dict_link = dict_link->next)
                            {
                                text = DD_NthChild(dict_link, 0);
                                link = DD_NthChild(dict_link, 1);
                                DD_String8 substring = DD_StringSubstring(strnode->string, i, i+text->string.size);
                                if(DD_StringMatch(substring, text->string, 0))
                                {
                                    fprintf(file, "<a class=\"link\" href=\"%.*s\">%.*s</a>",
                                            DD_StringExpand(link->string),
                                            DD_StringExpand(text->string));
                                    dict_word = 1;
                                    i += text->string.size-1;
                                    break;
                                }
                            }
                        }
                        
                        if(!dict_word)
                        {
                            fprintf(file, "%c", strnode->string.str[i]);
                        }
                    }
                }
                fprintf(file, "</%s>\n", html_tag);
            }
        }break;
        
        case DD_NodeKind_Set:
        {
            if(!DD_NodeIsNil(DD_TagOnNode(node, DD_S8Lit("list"))))
            {
                fprintf(file, "<ul class=\"list\">\n");
                for(DD_Node *child = node->children.first; child; child = child->next)
                {
                    if(child->children.first == 0)
                    {
                        fprintf(file, "<li class=\"list_item\">\n");
                    }
                    GeneratePageContent(index_table, site_info, page_info, file, child);
                    if(child->children.first == 0)
                    {
                        fprintf(file, "</li>\n");
                    }
                }
                fprintf(file, "</ul>\n");
            }
            else if(!DD_NodeIsNil(DD_TagOnNode(node, DD_S8Lit("img"))))
            {
                DD_Node *src = DD_NthChild(node, 0);
                DD_Node *alt = DD_NthChild(node, 1);
                fprintf(file, "<div class=\"img_container\"><img class=\"img\" src=\"%.*s\"></img></div>\n", DD_StringExpand(src->string));
            }
            else if(!DD_NodeIsNil(DD_TagOnNode(node, DD_S8Lit("youtube"))))
            {
                DD_Node *id = DD_NthChild(node, 0);
                fprintf(file, "<iframe width=\"560\" height=\"315\" src=\"https://www.youtube.com/embed/%.*s\" frameborder=\"0\" allow=\"accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture\" allowfullscreen></iframe>\n",
                        DD_StringExpand(id->string));
            }
            else if(!DD_NodeIsNil(DD_TagOnNode(node, DD_S8Lit("lister"))))
            {
                static int lister_idx = 0;
                fprintf(file, "<input autofocus id=\"lister_search_%i\" class=\"lister_search\" oninput=\"SearchInput(event, %i)\" onkeydown=\"SearchKeyDown(event, %i)\" placeholder=\"Filter...\"></input>", lister_idx, lister_idx, lister_idx);
                fprintf(file, "<ul id=\"lister_%i\" class=\"lister\">\n", lister_idx);
                lister_idx += 1;
                
                DD_Node *index_string = 0;
                for(DD_u64 idx = 0; !DD_NodeIsNil(index_string = DD_NthChild(node, idx)); idx += 1)
                {
                    for(DD_NodeTableSlot *slot = DD_NodeTable_Lookup(index_table, index_string->string);
                        slot; slot = slot->next)
                    {
                        if(slot->node)
                        {
                            PageInfo info = ParsePageInfo(slot->node);
                            
                            DD_String8 filename = slot->node->file;
                            DD_String8 filename_no_ext = DD_WithoutExtension(DD_WithoutFolder(filename));
                            DD_String8 link = DD_PushStringF("%.*s.html", DD_StringExpand(filename_no_ext));
                            DD_String8 name = info.title->string;
                            DD_String8 date = MakeDateString(info.date);
                            
                            fprintf(file, "<a class=\"lister_item_link\" href=\"%.*s\">\n", DD_StringExpand(link));
                            fprintf(file, "<li class=\"lister_item\">\n");
                            
                            if(info.header_image)
                            {
                                fprintf(file, "<div class=\"lister_item_img\" style=\"background-image:url('%.*s');\">",
                                        DD_StringExpand(info.header_image->string));
                            }
                            
                            fprintf(file, "<div class=\"lister_item_text\">\n");
                            fprintf(file, "<div class=\"lister_item_title\">\n");
                            fprintf(file, "%.*s\n", DD_StringExpand(name));
                            fprintf(file, "</div>\n");
                            
                            if(!DD_StringIsZero(date))
                            {
                                fprintf(file, "<div class=\"lister_item_date\">\n");
                                fprintf(file, "%.*s\n", DD_StringExpand(date));
                                fprintf(file, "</div>\n");
                            }
                            
                            if(info.desc)
                            {
                                fprintf(file, "<div class=\"lister_item_desc\">\n");
                                fprintf(file, "%.*s\n", DD_StringExpand(info.desc->string));
                                fprintf(file, "</div>\n");
                            }
                            if(info.header_image)
                            {
                                fprintf(file, "</div>\n");
                            }
                            fprintf(file, "</div>\n");
                            fprintf(file, "</a>\n");
                            
                            fprintf(file, "</li>\n");
                        }
                    }
                }
                fprintf(file, "</ul>\n");
            }
        }break;
        
        default: break;
    }
}
