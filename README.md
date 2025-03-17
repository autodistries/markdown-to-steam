<!-- mediaUrlPrefix:https://codeberg.org/catsoft/markdown-to-steam/media/branch/main/ -->
# markdown-to-steam

Converts regular markdown to steam-flavored markdown.\
This tool mainly follows [steam's syntax guide](https://steamcommunity.com/comment/Recommendation/formattinghelp).

## Usage

- Configure base media links
    `![images with](their link)` that don't start with `http` get that prepended to their link. Set the following at the start of your Markdown file :
```
<!-- mediaUrlPrefix:https://codeberg.org/catsoft/RainWorldMods/media/branch/main/BackgroundPreview/ -->
```
- run `./markdown-to-steam file.md [output.file]` 
- profit

## Supports : 

### headings (up to level 3)

- ~~striked text~~\
- **bold text**\
- *italic text* (with  `*` or `_`)\
- __underlined text__ (with `__`  or `<u><\u>`)\
- ```Code blocks``` (Not inline, big blocks only)\
- ``tiny [i]code[/i] *blocks*``  (will not convert, and steam will not render)\

----
- `---` separators\
- lists (first level only)
- `<!-- comments will show up in processed html as a zero-width table -->`\
`<!-- like this -->`
<!-- hidden -->
- `![embed](images, videos, and links)` be presented as an embed video if the format is ~~avif or~~ webm, or as links else: 
    Links and hyperlinks will be prepended with whatever link you choose, if they don't start with http\*. 
- \*\*escaped syntax\*\*. You should escape any symbols you don't want the converter to interpret. Symbols include \*, \_,\~, \`, \|, and \#, \- if you want to put them on the start of a line 


## Does not support
- Multi-level lists
- Tables


## image and links examples

current page: [here](./README.md);

webp media :

![media text](./output.webp)


webm media :
![media text](./output.webm)


avif media (full path) :
![media text](https://codeberg.org/catsoft/markdown-to-steam/media/branch/main/output.avif)


see [out.md](./out.md) for a textual result of this tool, and [on  the Workshop](https://steamcommunity.com/sharedfiles/filedetails/?id=3446184067) for the render
