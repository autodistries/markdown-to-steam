[table comment:mediaUrlPrefix:https://codeberg.org/catsoft/markdown-to-steam/media/branch/main/][/table]
[h1] markdown-to-steam [/h1]

Converts regular markdown to steam-flavored markdown.
This tool mainly follows [url=https://steamcommunity.com/comment/Recommendation/formattinghelp]steam's syntax guide[/url].

[h2] Usage [/h2]

[*] Configure base media links
    [noparse]![images with](their link)[/noparse] that don't start with [noparse]http[/noparse] get that prepended to their link. Set the following at the start of your Markdown file :
[code]
[table comment:mediaUrlPrefix:https://codeberg.org/catsoft/RainWorldMods/media/branch/main/BackgroundPreview/][/table]
[/code]
[*] run [noparse]./markdown-to-steam file.md [output.file][/noparse] 
[*] profit

[h2] Supports :  [/h2]

[h3] headings (up to level 3) [/h3]

[*] [strike]striked text[/strike]
[*] [b]bold text[/b]
[*] [i]italic text[/i] (with  [noparse]*[/noparse] or [noparse]_[/noparse])
[*] [u]underlined text[/u] (with [noparse]__[/noparse]  or [noparse]<u><u>[/noparse])
[*] [code]Code blocks[/code] (Not inline, big blocks only)
[*] [noparse]tiny [i]code[/i] *blocks*[/noparse]  (will not convert, and steam will not render)

[hr][/hr]
[/list][*] [noparse]---[/noparse] separators
[*] lists (first level only)
[*] [noparse][table comment:comments will show up in processed html as a zero-width table][/table][/noparse]
[noparse][table comment:like this][/table][/noparse]
[table comment:hidden][/table]
[*] [noparse]![embed](images, videos, and links)[/noparse] be presented as an embed video if the format is [strike]avif or[/strike] webm, or as links else: 
    Links and hyperlinks will be prepended with whatever link you choose, if they don't start with http*. 
[*] **escaped syntax**. You should escape any symbols you don't want the converter to interpret. Symbols include *, _,~, `, |, and #, - if you want to put them on the start of a line 


[/list][h2] Does not support [/h2]
[*] Multi-level lists
[*] Tables


[/list][h2] image and links examples [/h2]

current page: [url=https://codeberg.org/catsoft/markdown-to-steam/media/branch/main/./README.md]here[/url];

webp media :

[url=https://codeberg.org/catsoft/markdown-to-steam/media/branch/main/./output.webp]media text[/url]


webm media :
media text: 
[video]https://codeberg.org/catsoft/markdown-to-steam/media/branch/main/./output.webm[/video]


avif media (full path) :
[url=https://codeberg.org/catsoft/markdown-to-steam/media/branch/main/output.avif]media text[/url]


see [url=https://codeberg.org/catsoft/markdown-to-steam/media/branch/main/./out.md]out.md[/url] for a textual result of this tool, and [url=https://steamcommunity.com/sharedfiles/filedetails/?id=3446184067]on  the Workshop[/url] for the render
