# FSPDS 

## Flipnote Studio Player for Nintendo DS

*"Unburied" dekvkitPro from a 3yo hard disk backup, so I decided to give it a go :)*

This simple ROM allows you to play flipnotes on Nintendo DS (Lite) using your flashcard.

![ROM in action: show thumbnail](https://github.com/NotImplementedLife/Resources/blob/master/FSPDS/images/ss01.png "Show thumbnail")

*Thumbnail of one of the sample flipnotes (Cat on a skateboard) displayed by FSPDS*

## How to use

- Create a new folder named ```/flipnotes``` at the root of your SD card
- Copy your favorite flipnotes to ```/flipnotes``` directory
- Copy ```FSPDS.nds``` anywhere on your SD card
- Insert your flashcard into the DS, then turn on the console
- Launch ```FSPDS.nds```
- Select a file from the menu using the D-pad
- Press ```A``` to play it
- Use ```L```/```R``` buttons to navigate through bottom-screen tabs

## Bugs/Limitations

- FSPDS doesn't detect/play flipnotes larger than 1MB (this is intentional behavior)
- Don't place more than 1024 files in ```/flipnotes``` directory, otherwise FSPDS won't be capable to see them all 
(also intentional behavior, probably will be changed in the future)
- No sound currently
- It's possible to encounter some graphical glitches (they'll be fixed soon)

- Tested on R4(i) flashcards
