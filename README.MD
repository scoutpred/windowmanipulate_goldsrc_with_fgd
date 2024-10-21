Hello!

This is a small tech demo of window manipulation in GoldSrc Engine. What does it do currently as of writing:

-An entity that shows a prompt. Threaded so the game runs while the prompt shows.

-An entity that moves your window. Threaded so the game still runs while the prompt shows.

-An entity that changes your wallpaper.

This gives a foundation for the 4th Wall genre of mods for Half-Life. Perhaps this can be ported to Source Engine too.

The fact that this is open source allows modders to modify and/or fix the code, then be able to implement them in their mods.

# [Credits]

Inspired by games such as OneShot, Rhythm Heaven, and Stanley's Parable.

HL Updated repository by TWHL - https://github.com/twhl-community/halflife-updated

Original HL .fgd - https://developer.valvesoftware.com/wiki/Half-Life.fgd

ChatGPT for the majority of the codes.

# [Disclaimer]

Kindly check the source code. The program manipulates the registry mainly for the wallpaper changes, as well as getting information of the original wallpaper's settings in order to revert to original wallpaper properly.

The creator of OneShot actually did this to his game as per this thread - https://steamcommunity.com/app/420530/discussions/0/1729828401683446528/

What does it contain? there are two folders:

[One is the "projects" folder.]
You could simply copy the contents of that folder to your [hldll -> Source Files -> dlls] within your project.sln, then build it. I kept it open-source should you make changes.
Be sure you build it in your own mod folder.

[The "clztplayground" folder.]
This is a mod folder specifically for demo purposes. Simply put this folder in the root of the Half-Life Directory. It contains a map named "techdemo4thwall" where all features from the "projects" folder are implemented.

It's not much, but you get the idea. This codebase is in need of improvement and more features overall. Things like:

-Being able to be played in multiple platforms, e.g. Mac or Linux or Android via Xash.

-An entity that allows you to revert back to the original wallpaper when you touch a trigger_once or multiple or multisource/manager entities. Apparently I haven't implemented this.

-A "Y/N" prompt window, allowing for choice-based games, tailoring your story.

-File I/O manipulation, if you played "Imscared" you know what I mean.

-Various bug fixes should you find one.

-Also, the mod is bugging me with footsteps I don't even understand, but bear with it. It's a short demo.

-Could use some optimization.

I use J.A.C.K. as my map editor. You can check the map within the mod folder's map folder. Check the entities used in the map for more information. Treat it as a small documentation.

===

I only ask one thing if you are going to make something out of this.

If you are going to credit me, only credit me for the idea alone, which is the idea of implementing 4th Wall elements in the GoldSrc engine. Nothing more, nothing less. You get what I mean.

Codes go mostly for the external resources such as ChatGPT and Stack Overflow, the TWHL community and their HL Updated codebase, the Valve Dev community for the FGD, and the 4th wall ideas from other games. I don't deserve credit for those.

I'm willing to study further about C++ with this codebase to understand better on what I made. Feel free to share some so I can study more. :)

If you have suggestions, corrections, violent reactions, or anything, feel free to DM me on Discord: Clazzette, or maybe somewhere in this repo. Just make a thread.

Thank you!
