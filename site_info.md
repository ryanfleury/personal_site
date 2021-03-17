title:          "Ryan Fleury"
desc:           "A personal collection of my projects, blog posts, and other stuff."
canonical_url:  "https://ryanfleury.net"
author:         "Ryan Fleury"
twitter_handle: "@ryanjfleury"

link_dictionary:
{
    ("The Melodist", "melodist.html")
        ("Data Desk", "data_desk.html")
        ("Dion Systems", "https://dion.systems")
        ("Dion", "dion.html")
        ("Handmade Network", "https://handmade.network")
        ("ryan.j.fleury@gmail.com", "mailto:ryan.j.fleury@gmail.com")
}

header:
"""
<div class="header">
<div class="header_title">    
<a href="projects.html">Ryan Fleury</a>
</div>
<div class="header_links">
<a href="projects.html" class="header_link">Projects</a>
<a href="blog.html" class="header_link">Blog</a>
<a href="about.html" class="header_link">About</a>
<a href="contact.html" class="header_link">Contact</a>
</div>
</div>
<div class="separator">
</div>
"""

footer:
"""
<div class="separator reverse"></div>
<div class="footer">
<a class="header_link" href="https://github.com/ryanfleury">GitHub</a>
<a class="header_link" href="https://youtube.com/ryanfleury">YouTube</a>
<a class="header_link" href="https://twitter.com/ryanjfleury">Twitter</a>
</div>
"""

style:
"""
:root
{
	--highlight-color:                #e09a4f;
    --background-color:               rgb(56, 60, 64);
    --page-container-color:           rgb(76, 81, 87);
    --page-container-alternate-color: rgb(99, 117, 128);
    --default-text-color:             rgb(225, 229, 235);
    --weak-text-color:                rgb(120, 120, 120);
    --title-color:                    var(--highlight-color);
    --sub-title-color:                rgb(150, 120, 80);
    --date-color:                     rgb(120, 120, 120);
    --content-width:                  45rem;
    /* NOTE: Need to update size media queries if this changes, because CSS sucks. */
}

@font-face
{
    font-family: 'Header';
    src: url('./data/PT_Sans-Narrow-Web-Bold.ttf') format('truetype');
}

@font-face
{
    font-family: 'HeaderLink';
    src: url('./data/Share-Regular.ttf') format('truetype');
}

@font-face
{
    font-family: 'Title';
    src: url('./data/PT_Sans-Narrow-Web-Bold.ttf') format('truetype');
}

@font-face
{
    font-family: 'Body';
	src: url('./data/PT_Serif-Web-Regular.ttf') format('truetype');
}

@font-face
{
    font-family: 'Mono';
	src: url('./data/Inconsolata-Regular.ttf') format('truetype');
}

*
{
    padding: 0;
    margin: 0;
}

body
{
    font-family: 'Body';
    background-color: var(--background-color);
}

.title
{
    padding-top: 0em;
    color: var(--title-color);
    padding-left: 0;
    padding-right: 0;
	padding-bottom: 0.5em;
    font-family: 'Title';
    /*text-transform: uppercase;*/
    font-size: 1.5em;
}

.subtitle
{
    color: var(--sub-title-color);
    padding-left: 0;
    padding-right: 0;
	padding-bottom: 1em;
    font-family: 'Body';
    font-size: 1.2em;
}

.date
{
    color: var(--date-color);
    padding-left: 0;
    padding-right: 0;
	padding-bottom: 1em;
    font-family: 'Body';
    font-size: 0.8em;
    font-style: italic;
}

p
{
    color: var(--default-text-color);
	font-size: 1.2em;
    padding-bottom: 1em;
    padding-top: 1em;
    padding-left: 0;
    padding-right: 0;
}

pre
{
    padding-left: 0;
    padding-right: 0;
    font-family: 'Mono';
}

hr
{
	margin-left: auto;
	margin-right: auto;
	padding-top: 2em;
	margin-bottom: 1em;
	border: none;
	border-bottom: 1px solid;
	height: 1px;
	width: 50%;
	color: var(--page-container-color);
}

::selection
{
	background: var(--highlight-color);
	color: #ffffff;
}

::-moz-selection
{
	background: var(--highlight-color);
	color: #ffffff;
}

.link
{
	color: var(--highlight-color);
	text-decoration: none;
}

.link:hover
{
	border-bottom: 1px solid;
	border-bottom-width: 3px;
}

.standalone_link_container
{
	color: var(--highlight-color);
	text-decoration: none;
	margin-left: 0;
	padding-bottom: 1.5em;
    font-family: 'HeaderLink';
}

.lister_link
{
	font-size: 1.2em;
	margin-left: 0;
    margin-right: 0;
	margin-top: 1em;
	padding-left: 1em;
	padding-right: 0;
	padding-top: 1em;
	padding-bottom: 1em;
	color: var(--default-text-color);
	text-decoration: none;
	background: var(--page-container-color);
	border-radius: 2px;
	width: calc(100% - 1em);
	display: block;
	transition: all 0.2s ease;
    font-family: 'Header';
}

.lister_link:hover
{
	background: var(--page-container-alternate-color);
	transition: all 0.2s ease;
    transform: translateY(-1.5px);
    box-shadow: 0em 3px 10px rgba(30, 30, 30, 0.2);
}

.page_content
{
    max-width: var(--content-width);
    margin-left: auto;
    margin-right: auto;
    /*background-color: var(--page-container-color);*/
    padding-top: 2em;
    padding-bottom: 3em;
}

.header
{
    max-width: var(--content-width);
	max-height: 2em;
	padding-left: 0em;
    margin-left: auto;
    margin-right: auto;
    margin-top: 2em;
    padding-top: 0em;
    vertical-align: middle;
    border-bottom: var(--page-container-color) 1px solid;
}

.header_title
{
    font-family: 'Header';
    font-size: 2.5em;
    color: var(--highlight-color);
    text-transform: uppercase;
    display: inline-block;
    text-align: bottom;
	line-height: 0;
	padding-bottom: 2em;
}

.header_title a
{
    color: inherit;
    text-decoration: none;
}

.header_links
{
    display: inline-block;
    margin-top: 0em;
    vertical-align: bottom;
    padding-left: 4em;
    text-align: right;
    float: right;
}

.header_link
{
	vertical-align: middle;
    font-family: 'HeaderLink';
    font-size: 1.2em;
    color: var(--highlight-color);
    text-decoration: none;
    margin-left:0.5em;
    margin-right:0.5em;
	line-height: 0;
    text-align: right;
	border-bottom: 3px solid transparent;
}

.header_link:hover
{
    border-bottom: 3px solid;
}

.footer
{
    color: var(--weak-text-color);
    max-width: var(--content-width);
    margin-left: auto;
    margin-right: auto;
	padding-right: 0;
    padding-top: 1em;
    padding-bottom:4em;
    text-align: right;
    font-family: 'HeaderLink';    
    border-top: var(--page-container-color) 1px solid;
}

.page_banner
{
    height: 20em;
    margin-bottom: 1em;
    background-size: auto 200%;
	background-position-x: center;
    background-position-y: center;
    border-radius: 0.2em;
}

.youtube
{
    text-align: center;
    margin-left: auto;
    margin-right: auto;
    margin-bottom: 1em;
	margin-top: 1em;
    padding-top: 2em;
    padding-bottom: 2em;
    background-color: #1e1e1e;
}

.code
{
    margin-left: auto;
    margin-right: auto;
	margin-top: 1em;
    margin-bottom: 1em;
    padding-top: 2em;
    padding-bottom: 2em;
    padding-left: 2em;
    padding-right: 2em;
    background-color: #1e1e1e;
    color: #ba8e4c;
    font-family: 'Mono';
	overflow: auto;
	font-size: 1em;
}

.code_text
{
	font-family: 'Mono';
}

.img_container
{
    /*background-color: #1e1e1e;*/
    text-align: center;
    margin-left: auto;
    margin-right: auto;
    margin-bottom: 1em;
    margin-top: 1em;
    padding-top: 1em;
    padding-bottom: 1em;
}

.thumbnail_image_container
{
    display: inline-block;
	padding-top: 1em;
	padding-left: 0;
}

.thumbnail_image
{
	width: 31%;
	overflow: hidden;
	margin-right: 0.5em;
	margin-bottom: 0.2em;
	opacity: 0.8;
	transition: opacity 0.1s ease;
	border-radius: 4px;
}

.thumbnail_image:hover
{
	opacity: 1;
	transition: opacity 0.1s ease;
}

.img
{
    max-width: 60%;
    max-height: 20em;
	border-radius: 2px;
}

.separator
{
    display: none;
    position: relative;
    max-width: 70rem;
    height: 70px;
    margin-left: auto;
    margin-right: auto;
	margin-bottom: -1px;
}

.separator::before
{
    position: absolute;
    content: "";
    width: 100%;
    height: 100%;
    background-color: #ffffff;
    clip-path: polygon(100% 0, 0 0, 0 100%);
}

.separator::after
{
    position: absolute;
    content: "";
    width: 100%;
    height: 100%;
    background-color: #dfdfdf;
    clip-path: polygon(100% 0, 0 100%, 100% 100%);
}

.separator.reverse::before
{
    background-color: #dfdfdf;
}

.separator.reverse::after
{
    background-color: #ffffff;
}

.feature_button
{
    margin-left: 0;
    margin-right: 0;
    margin-top: 2em;
    width: 100%;
    height: 20rem;
	max-height: 20em;
    position: relative;
    overflow: hidden;
    border-radius: 3px;
    transition: all 0.2s ease;
    opacity: 0.8;
}

.feature_button_image
{
    background-repeat: no-repeat;
    background-size: auto 200%;
	background-position-x: center;
    background-position-y: center;
    height: 20em;
	transition: all 0.2s ease, background-position 0ms;
}

.feature_button .feature_button_image
{
	background-size: auto 200%;
    opacity: 0.8;
	filter: blur(2px);
    -webkit-filter: blur(2px);
}

.feature_button:hover .feature_button_image
{
	background-size: auto 205%;
    opacity: 1.0;
	filter: blur(0px);
    -webkit-filter: blur(0px);
}

.feature_button_text
{
    position: absolute;
    left: 1em;
    top: 50%;
	bottom: 50%;
    font-family: 'Header';
    font-size: 4rem;
	line-height: 0;
    text-decoration: none;
    /*text-transform: uppercase;*/
    color: #ffffff;
    text-shadow: 4px 4px 12px #000;
}

.inline_code
{
	font-family: 'Mono';
    background-color: #1e1e1e;
    color: #ba8e4c;
    padding-right: 0.2em;
    padding-left: 0.2em;
    border-radius: 0.1em;
}

.lister_search
{
    margin-left: auto;
    margin-right: auto;
    padding: 0;
    padding-left: 0.2em;
    padding-top: 0.2em;
    padding-bottom: 0.2em;
    border-radius: 0.2em;
    box-sizing:border-box;
    -moz-box-sizing:border-box;
    border: 0px;
    width: 100%;
    font-family: 'Body';
    
    font-size: 1.1rem;
    background-color: #1e1e1e;
    color: #ba8e4c;
    
    border: 1px solid #ba8e4c60;
}

.lister_search:focus
{
    outline:0px;
    border: 1px solid #ba8e4c;
}

.lister_item
{
    margin-left: 0;
    margin-right: 0;
    margin-top: 2em;
    width: 100%;
    height: 20rem;
	max-height: 20em;
    position: relative;
    overflow: hidden;
    border-radius: 3px;
    transition: all 0.2s ease;
    opacity: 0.8;
}

.lister_item_img
{
    background-repeat: no-repeat;
    background-size: auto 200%;
	background-position-x: center;
    background-position-y: center;
    height: 20em;
	transition: all 0.2s ease, background-position 0ms;
    padding: 0;
}

.lister_item .lister_item_img
{
	background-size: auto 150%;
    opacity: 0.8;
}

.lister_item:hover .lister_item_img
{
	background-size: auto 155%;
    opacity: 1.0;
}

.lister_item_link
{
    text-decoration: none;
}

.lister_item_text
{
    position:absolute;
    bottom:0;
    padding: 1em 1em 1em 1em;
    background-color: rgba(0, 0, 0, 0.85);
    width: calc(100% - 2em);
}

.lister_item_title
{
    font-family: 'Header';
    font-size: 1.5rem;
    color: #ffffff;
    text-shadow: 4px 4px 12px #000;
    padding-bottom: 0.5em;
}

.lister_item_date
{
    font-family: 'Body';
    font-size: 0.8rem;
    color: #ffffff;
    text-shadow: 4px 4px 12px #000;
    padding-bottom: 0.9em;
    font-style: italic;
}

.lister_item_desc
{
    font-family: 'Body';
    font-size: 1rem;
    color: #ffffff;
    text-shadow: 4px 4px 12px #000;
}


@media only screen and (max-width: 49rem)
{
    .page_content
    {
        padding-left: 2em;
        padding-right: 2em;
    }
    
    .header
    {
        padding-left: 2em;
        padding-right: 2em;
    }
    
    .footer
    {
        padding-left: 2em;
        padding-right: 2em;
    }
    
    .feature_button_text
    {
        font-size: 1.5rem;
    }
    
    .header
    {
        height: auto;
        max-height: none;
    }
    
    .header_title
    {
        display: block;
        font-size: 3em;
        text-align: center;
        margin-left: auto;
        margin-right: auto;
        padding-top: 0.5em;
        padding-bottom: 2em;
    }
    
    .header_link
    {
        display: block;
        padding-bottom: 0.5em;
        margin-bottom: 2em;
        text-align: center;
        font-size: 2em;
    }
    
    .header_link:hover
    {
        border: none;
        border-bottom: 3px solid transparent;
    }
    
    .header_links
    {
        margin-left: auto;
        margin-right: auto;
        display: block;
        float: none;
        padding-left: 0;
        padding-right: 0;
    }
    
    .footer
    {
        padding-top: 4em;
    }
    
}
"""
