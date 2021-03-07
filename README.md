

# <img src="icon.bmp"></img> FSPDS V0.1

## Flipnote Studio Player for Nintendo DS

*"Unburied" dekvkitPro from a 3yo hard disk backup, so I decided to give it a go :)*

This simple ROM allows you to play flipnotes on Nintendo DS (Lite) using your flashcard.

## How to use

- Create a new folder named ```/flipnotes``` at the root of your SD card
- Copy your favorite flipnotes to ```/flipnotes``` directory
- Copy ```FSPDS.nds``` anywhere to your SD card
- Insert your flashcard into the DS, then turn on the console
- Launch ```FSPDS.nds```
- Select a file from the menu using the D-pad
- Press ```A``` to play it
- Use ```L```/```R``` buttons to navigate through bottom-screen tabs

## Screenshots
<p align="center">
    <img width="600" src="README_Resources/ss01.png" alt="ROM in action: show thumbnail"></img>
    <br/>
    <i>Thumbnail of one of the sample flipnotes (Cat on a skateboard) displayed by FSPDS</i>
</p>


<p align="center">
    <img width="600" src="README_Resources/ss02.png" alt="ROM in action: show thumbnail"></img>
    <br/>
    <i>Capture of "eBay Song" flipnote by RANDOM☆GUY played by FSPDS</i>
</p>

## Bugs/Limitations

- FSPDS doesn't detect/play flipnotes larger than 1MB (this is intentional behavior)
- Don't place more than 1024 files in ```/flipnotes``` directory, otherwise FSPDS won't be capable to see them all 
(also intentional behavior, probably will be changed in the future)
- No sound currently
- ~~It's possible to encounter some graphical glitches (they'll be fixed soon)~~ [*Hopefully not!*]

### Tested on DeSmuMe emulator and R4(i) flashcards

## Credits

- Guys from [DSiBrew](https://dsibrew.org/wiki/Flipnote_Files/PPM "PPM Format") and [Flipnote Collective](https://github.com/Flipnote-Collective/flipnote-studio-docs/wiki/PPM-format "PPM Format") for their awesome documentation on .PPM file format.
- [devkitPro](https://github.com/devkitPro) for their compiler, ndslib and examples
- The authors behind [GodMode9i](https://github.com/DS-Homebrew/GodMode9i "GodMode9i") for the idea of overwriting character fonts
- [Drenn1/GameYob](https://github.com/Drenn1/GameYob "Drenn1/GameYob") for helping me figure out how to work with makefiles.