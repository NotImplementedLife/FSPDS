# <img src="icon.bmp"></img> FSPDS - Flipnote Studio Player for Nintendo DS

<img alt="GitHub Top Language" src="https://img.shields.io/github/languages/top/NotImplementedLife/FSPDS" /> <img alt="Github License" src="https://img.shields.io/github/license/NotImplementedLife/FSPDS" /> <img alt="GitHub Issues" src="https://img.shields.io/github/issues/NotImplementedLife/FSPDS" /> <img alt="GitHub Closed Issues" src="https://img.shields.io/github/issues-closed/NotImplementedLife/FSPDS" /> <img alt="GitHub Last Commit" src="https://img.shields.io/github/last-commit/NotImplementedLife/FSPDS" /> 

<a href="https://notimplementedlife.itch.io/fspds">![itch.io](https://img.shields.io/badge/Itch-%23FF0B34.svg?style=for-the-badge&logo=Itch.io&logoColor=white)</a>
<a href="https://youtu.be/3NL79zHFvF8">![YouTube](https://img.shields.io/badge/YouTube-%23FF0000.svg?style=for-the-badge&logo=YouTube&logoColor=white)</a>

Note from January 2021 : *"Unburied" dekvkitPro from a 3yo hard disk backup, so I decided to give it a go :)*

This simple ROM allows you to play flipnotes on Nintendo DS (Lite) using your flashcard.

## Features

### Flipnote player

FSPDS allows DSi flipnote playback providing an experience similar to a common media payer. It supports pause/resume options, auto repeat, direct navigation to the previews/next flipnote and shuffling. It should be able to play any flipnote that runs correctly in the original Flipnote Studio.

### Small notes on the filesystem

For faster load times, FSPDS prescans the folders for flipnotes and stores their paths onto the SD card. Therefore, when the user changes the content of their flipnote folders, a directory reindex must be performed. This way, FSPDS cached data will be up to date with the actual files structure. Not reindexing might cause new flipnotes not showing up in the list, or deleted flipnotes still persist in a phantom state in the application, fact denoted by the _dead green frog_ thumbnail.


## Bugs/Limitations

- FSPDS doesn't detect/play flipnotes larger than 1MB (this is intentional behavior)
- The flipnote's BGM track should be smaller than 512KB in order to be played correctly. However, the DSi Flipnote Studio only allows
for 1 minute of raw audio (8192Hz) to be encoded into a flipnote, which means at most 240KB of sound data. Therefore, FSPDS should not have
problems playing original flipnotes. Spin-off flipnotes which contain huge audio data may not play correctly with this application.
- Crashes on the Flipnote Lenny ( ͡° ͜ʖ ͡°) files.

## Possible improvements

- More generous limitations when the application runs in DSi mode to support more "unofficial" flipnotes. Example: 4MB PPM file size, max 1MB audio etc.

## Tested on

- DeSmuME emulator
- no$gba with DSi NAND & virtual SD card
- Nintendo DS Lite with R4(i) flashcards
- Nintendo DSi 

<b>Tested on DeSmuMe emulator and R4(i) flashcards </b>

### Hardware vs emulator compatibility

- On the hardware, loading files from FAT system is substantialy faster;

## How to build

```
git clone https://github.com/NotImplementedLife/FSPDS.git
cd FSPDS
make
```

Python is required to run a build tool script. 

For non-Windows systems, the most challenging part would be running `DSC.Toolchain.AssetBuild.exe`. I'm not a Linux user myself and I can't provide reliable guidance in that problem, but some program like Wine or mono could be of some help, or you can even rebuild the executable directly from its [source code](https://github.com/DS-Creator-Dev/DSC.Toolchain).


## Screenshots

<p align="center">
    <img width="600" src="https://github.com/NotImplementedLife/FSPDS/assets/70803115/f924d76b-3bc4-4c0f-b04f-0c42a2db8568" alt="ROM in action: Title screen"></img>        
    <img width="600" src="https://github.com/NotImplementedLife/FSPDS/assets/70803115/965c76f8-8c71-46ec-8186-e052784b61d6" alt="ROM in action: Browse flipnotes"></img>
    <img width="600" src="https://github.com/NotImplementedLife/FSPDS/assets/70803115/f434f8e2-f3cb-4a62-a070-500ebe19aee0" alt="ROM in action: Pick a flipnotes directory"></img>        
    <img width="600" src="https://github.com/NotImplementedLife/FSPDS/assets/70803115/816fba2d-3c96-41da-95bc-7e05ee324e4a" alt="ROM in action: Help screen"></img>        
    <img width="600" src="https://github.com/NotImplementedLife/FSPDS/assets/70803115/f87bf35a-ba11-4c21-87ea-f6fe093b096a" alt="ROM in action: Flipnotes list 1"></img>        
    <img width="600" src="https://github.com/NotImplementedLife/FSPDS/assets/70803115/0daac322-0788-4a06-9b0b-0e24dea0ae1d" alt="ROM in action: Flipnotes list 2"></img>        
    <img width="600" src="https://github.com/NotImplementedLife/FSPDS/assets/70803115/248c3118-3f92-42e8-8b4e-73929af62f45" alt="ROM in action: Flipnote player"></img>    
</p>

## History

Following the successful attempt to create a flipnote editor for PC, _FlipnoteDesktop_, (now named [Flipnote.NET](https://github.com/NotImplementedLife/Flipnote.NET)),
I had a pretty fresh knowledge of the flipnote format and was wondering whether a regular Nintendo DS could handle flipnote playback or not. That time, I found myself
facing what I thought it was an incredibly complex task, with astounding challenges which could reveal deep holes in my knowledge about the DS homebrew and programming.
And my intuition was right... Considering how many questions I had regarding this problem, among which the most relevant one was _"is it even possible?"_, I wasn't sure
if starting this project was a good idea. But eventually, curiosity killed the cat and I convinced myself that the worst thing that could happen was to lose time creating a
crappy thing that I'd add to my remarkable collection of <i>ideas_that_I_like_but_I_am_too_dumb_for</i>. However, it turns out FSPDS was one of my very first serious projects I started to work on that didn't actually end up tossed in the abyss of failure, abandonment and forgetting.

I created this repository on January 27th, 2021, using a Code::Blocks template for libnds that I learned how to set up in 2016 when I first got into DS programming. For all those years I was extremely stubborn in my belief that this was the only good and real way to organize a C++ project. In reality, my deep subconscious knew that Code::Blocks was the "safe territory", as I was literally scared of Makefiles (after I learned what those things are), and my laptop's performance was easily outplayed by any sophisticated IDE such as Visual Studio.

To make an idea of the skills I possessed at the moment of starting the project, I only knew the very most basic thing about libnds. I knew how to "Hello world" with `consoleDemoInit();`, how to draw normal bitmaps and corrupted bitmaps (if I wasn't lucky enough to have the correct bits per pixel value already set by the template code I used), double buffering I read about on a forum and how to handle key inputs at the happiest level of calling the libnds functions and by the particularly fateful chance of knowing what bitwise operations are. One could say I was completely hopeless and I should give up or learn more and try later (and couldn't be more right). But you know the saying: what you don't know won't hurt you. In my courageous lack of experience I dared to say that what I knew was more than enough to achieve my goal.

It wasn't more until I created the visual and structural framework for the application. I opted for a tabsystem as I thought it was the easiest thing to do when the only UI refresh routine you master is roughly `consoleClear()`, `gotoxy()` and `iprintf()`. I'll never forget the moment an innocent Google search with debug purposes brought me to the [GodMode9i source code lines](https://github.com/DS-Homebrew/GodMode9i/blob/edd514aad5d7f775a778cc356a82d6b12a4bea4a/arm9/source/main.cpp#L115-L117) which overwrite the default console characters font in order to make custom symbols. To me that was an extraordinary breakthrough and in my excitement I felt like was doing magic. I ended up using the "font overwrite trick" (which otherwise wasn't anything than a VRAM write to a certain tile address) to create a fancy border around my tabs enhancing the look of my application.

Not so long after, I managed to put all the pieces in the right place and managed to create a working flipnote player prototype. At least, the visual part. In order to do that, I followed the [Flipnote Collective's specifications on PPM format](https://github.com/Flipnote-Collective/flipnote-studio-docs/wiki/PPM-format) to the letter, without realizing that the suggested code for frame decoding was just an example to understand the process, instead I simply assumed that was the most efficient approach. That problem made itself visible almost immediately through an annoyingly poor flipnote playback performance compared to the original Flipnote Studio. One of the flipnotes I used to perform tests on, authored by Mr. Mask featuring a fox inside a Zelda-like dungeon, which was normally a measured 21 seconds long as far as I can remember, was taking more than 50 seconds to completely play in FSPDS. The reason was obviously related to the inefficient code which caused VBlank logic to happen some frames later than usual, explaining the inflated flipnote play time. And refactoring it wasn't a straightforward task at that time, as it implied taking in account a handful of factors, like the line encoding and the frame diffing, which by itself was really scary. After an intense brainstorm and some experiments, I came up with an idea I was overwhelmingly proud of, and I still am to this day, even if it has meanwhile become obsolete. I took advantage of the fact that the flipnote maximum frame rate was 30fps, whilst the DS screen refresh rate is 60Hz, which meant a speed 8 flipnote would change its frame once at 2 VBlank periods, therefore I could theoretically split the frame decoding into two parts: let's say the first 96 lines are decoded in the first VBlank, and the next 96 lines are decoded during the next VBlank, and by splitting a time consuming task into two relatively identical jobs I could be lowering the chance to miss a VBlank interrupt during playback. And, surprisingly, it worked! I called that method "VBlank optimization". This brings us to March 7th, 2021, the date of the first release of FSPDS. It was a significant victory for me, but I knew I had yet another difficulty to face.

Version 0.1 of FSPDS was unable to play sounds. I started working on that just some days after the big release, wondering how hard could it be to add a `soundPlaySample` call at the moment of playback start. Little I knew about the weird thing that would determine me suspend the development for 9 entire months... Everything started when I implemented the pause/resume feature and found out that libnds functions `soundPause()/soundResume()` weren't doing exactly what one would expect them to do just by looking at their name. Indeed, `soundPause()` stops the sound as intended, but `soundResume()` doesn't play the sound from the moment it was stopped, instead it completely restarts the track! I ended up asking for help on [GBATemp](https://gbatemp.net/threads/libnds-soundresume-actually-restarts-the-whole-track.584761/) in the hope that anyone has face the problem before, and the good suggestion I got from [KiiWii](https://gbatemp.net/members/kiiwii.143142/) to keep track of the play time and manually "resume" the sound from the moment it was paused seemed pretty solid and logical, but I had absolutely no idea how to approach this in code. To my knowledge, there was no predefined function to do that, and not even maxmod provided what I was in need of, so I knew the only option was to go ahead and write it myself. I spent my time trying workarounds, and took long pauses followed by desperate attempts and experiments, until one day I was patient enough and inspired to look into how music is digitally encoded. Consequently during one of my tests I managed to successfully pause and resume a PCM track on the DS. It wasn't until January next year when I discovered that the last piece of my puzzle had been in plain sight for all this time, but I had no idea how to use it. I'm talking about a [ADPCM to PCM converter](https://github.com/miso-xyz/PPMLib/blob/c7548bf4cdb0e368af552c71a45eb9f96f2e3385/PPMLib/Extensions/AdpcmDecoder.cs#L46-L119), found by [RinLovesYou](https://github.com/RinLovesYou) while we were collaborating on a PPM library shortly after FlipnoteDesktop was released (sadly it never reached an usable state though). Anyways, having all the information I needed helped me come up with an intuitive plan: convert the flipnote background music from its default ADPCM format to the uncompressed 16-bit PCM, then have a timer with a certain frequency run along with the flipnote and use it to recover the soundtrack's progress when paused. After that, it was all a matter of small adjustments until I solved one of the hardest issues encountered so far in my life and reached the next milestone on 26 January 2022: FSPDS 0.2.0. 

After that, an user named [tomrow](https://github.com/tomrow) suggested a new feature to search for flipnote into multiple paths. This way, I found the perfect reason to overthink and step into a new realm of experiments: filesystem. You see, I got my hands on a monster archive online with more than 1000 flipnotes and loaded them into my application without a second thought. The result was to be expected: it crashed, or better said, freezed. The load was too high. I started to research the limit of the posibilities. First, I started to think that libFAT was slow. Then, FAT system itself was slow. The maximum number of files the can exist in a FAT directory is 65536. Am I doing too many `fopen()`s while reading thumbnails? I reworked the FSPDS filesystem manager (or "reader" to say the least) from the ground up. I implemented a method to load the flipnotes into chunks and wrote a background worker which made sure the chunks that were in the user's reach were properly loaded. _Yes, on a Nintendo DS..._ From my present perspective, I think it was an overengineered solution to an illusory problem. As I came to discover later, FAT really is slow at handling a high density of files, and I saw people online [recommending](https://serverfault.com/questions/98235/how-many-files-in-a-directory-is-too-many-downloading-data-from-net) to keep data organized _in more folders with less files_. I ultimately accepted that there was nothing to do on my end and eventually got rid of the unnecessarily complicate file manager I created.

Getting closer to the time of writing, one day I met a guy with an ambitious dream. He wanted an application like DS Game Maker that allows you to create your own DS games. I wanted that, too (and for quite a long time), so we started working on it. You have my word - writing a game maker is not a cup of tea. It looked like the development was slow and sometimes chaotic, we had a hard time trying to figure out what's the best approach to do one thing or another. Personally, it was an experience that guided me way farther into developing (better or worse) game development techniques I still favor today. However, the final goal was too difficult to achieve and I was expected to fail. I really hope [Kenyon Bowers](https://github.com/kenyonbowers) doesn't resent that. Our project [DS Creator](https://github.com/DS-Creator-Dev) was left in an indefinite state. But in the end all the struggle wasn't really in vain, as once time passed I suddenly decided to rebuild FSPDS on top of the remaints of our DSC engine. After all, it has the great feature of automatic resource allocation and management for basically every important graphics component (palettes, VRAM, sprites). These were meant to save a lot of time and prevent programmers from having to deal with hardcoded addresses manually. This is how 2023 finds FSPDS with a brand new look, a little shot to a 3D scene (I'm not familiar with OpenGL by any means, so I'm really proud of how little I could do) and - you won't believe that - a more efficient version of the player, which does not longer need VBlank optimization. The one trick I based all my hopes on is crazily stupid for how effective it is. Let me briefly explain: at some point in the frame decoding step, there is a process where a sequence of bits needs to be converted into bytes (or nibbles), in a way that a bit 0 decodes to byte (or nibble) 0x0, and a bit 1 decodes to byte (or nibble) 0x1, and instead of a `while` loop that iterates through each bit I used a values table where a number like 0b10101100 would correspond to its decoded counterpart 0x10101100. It was _that_ simple. Moreover, after the DSC engine experiments and hard training on the GBA I was more than accustomed to 4-bit tiles and casually noticed that the flipnote thumbail can be passed to VRAM for display with 0 modifications, and found an overall better way to draw flipnote frames. Moreover, my custom build of [DeSmuME](https://github.com/NotImplementedLife/desmume-wasm) with logging to emulator support was an indispensable tool in the process.

In the end, I did it. The unthinkable has been done. The world has a decent Flipnote Studio Player for the original Nintendo DS and DS Lite. It was a journey full of mysteries, highs and lows, hard work accompanied by sparkling ideas (was it divine intervention?), but I enjoyed it to the fullest. FSPDS was a huge experience for a mere side project that was meant to be. I thank everyone that helped me achieve this, directly or indirectly. And because nobody but my silly mind asked to create this, I can't help but hope that FSPDS might make someone happy and might be of any use sometime, somehow.


## Credits

- Guys from [DSiBrew](https://dsibrew.org/wiki/Flipnote_Files/PPM "PPM Format") and [Flipnote Collective](https://github.com/Flipnote-Collective/flipnote-studio-docs/wiki/PPM-format "PPM Format") for their awesome documentation on .PPM file format.
- [devkitPro](https://github.com/devkitPro) for their compiler, ndslib and examples
- The authors behind [GodMode9i](https://github.com/DS-Homebrew/GodMode9i "GodMode9i") for the idea of overwriting character fonts
- [Drenn1/GameYob](https://github.com/Drenn1/GameYob "Drenn1/GameYob") repo for helping me figure out how to work with makefiles.
- [RinLovesYou](https://github.com/miso-xyz/PPMLib/commits?author=RinLovesYou) for her [ADPCM to PCM Decode() method from PPMLib](https://github.com/miso-xyz/PPMLib/blob/c7548bf4cdb0e368af552c71a45eb9f96f2e3385/PPMLib/Extensions/AdpcmDecoder.cs#L46-L119)
- [tomrow](https://github.com/tomrow) for their suggestion on [Alternate flipnote browse locations](https://github.com/NotImplementedLife/FSPDS/issues/2) 
- [Moonshell2](https://github.com/lifehackerhansol/moonshell) source code for hints on making the screen saver during playback
- [DSC Engine](https://github.com/DS-Creator-Dev/DSCEngine) provides the main framework of this application
