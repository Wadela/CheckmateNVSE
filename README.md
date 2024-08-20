~~~ Description ~~~

This NVSE plugin allows for multiple conditions to be displayed in a single dialogue topic. Dialogue options like the one below can now display in game:

"[Speech 100] [Science 100] Making this wasn't tedious in the slightest!" 

There are two main components to this mod, an NVSE plugin that adds custom instructions for dialogue and a plugin file that changes every single skill check in the game. 


~~~ Requirements ~~~

xNVSE 6.35
Fallout New Vegas DLC (for DLC version)


~~~ Installation ~~~

Install and enable with a mod manager, I recommend MO2. 

If you choose not to use a mod manager:
- Checkmate.dll goes into Data/NVSE/Plugins
- Checkmate.esp goes into Data/

~~~ Compatibility ~~~

- Not compatible with bNoSkillTags in lStewieAl's Tweaks and Engine Fixes; make sure bNoSkillTags = 0
- You're not going to see it do anything for TTW. If there is a lot of support I'll consider doing all the checks for Fallout 3... Maybe...
- Should work with most mods that don't touch dialogue


~~~ Making Your Own Versions ~~~

Making your own versions of Checkmate.esp isn't much different than just editing dialogue in vanilla. Feel free to upload your own .ESP's if you'd like. This plugin supports as many conditions as you would like to add. I haven't tested more than 5, though. 

1. Open the G.E.C.K.
2. In the Object Window, Find your NPC. You can use the filter to type the name of the character you'd like to edit
3. Double click to select your character then, open up the 'Dialogue' window
4. Find a Skill Check dialogue topic by navigating through the Topics pane. The pane with all the topics, priority, etc. It has a checkbox above it, "Top-level only"
5. If your Skill Check Topic has both a Pass and Fail option, you're going to edit both of them, start by selecting the [SUCCEEDED] option in the Topic Text pane in the center of the screen.
6. The first of two main things you need to change is the "AV/Perk" Dropdown menu. Normally this menu is what decides the name of the skill that your check starts with e.g. "[Speech 100]...". With this plugin however, that dropdown now behaves sort of like a true or false selection. As long as there is something in the box other than 'NONE', your skill will appear. I still recommend keeping it in line with what you're going to be adding.
7. The second main thing is the Conditions pane directly underneath the AV/Perk dropdown. Whatever Actor Value condition you enter will be what appears on the screen. If you are in need of this guide i'm going to assume you're sticking to modifying existing skill checks so right click duplicate the existing condition, then double click and change the skill to your liking--at the value you want to change it to. 
8. Repeat for the [FAILED] option but if you are keeping it at '<' for the condition, make sure to correctly select the 'OR' checkbox when applicable (in this example, if you have 3 skill checks select or for the first two; if you have 2 skill checks, select or for only the first)
9. Save your .esp and profit. 

NOTE: This plugin currently has a limitation with perks. If you'd like to add perks to your topic, you have to manually write them out (e.g. [Educated]...) in the topic override window as I removed the functionality of the AV/Perk dropdown. I'll look into it eventually, but that is just a current limitation. The logic for it still works though. You are also able to use the conversation editor for all of this if you choose.

TLDR: Edit skill check topics how you normally would, just add as many conditions as you want to add and make sure the AV/Perk dropdown is filled. 


~~~ Special Thanks ~~~

Thank you so much to all the people who took the time to help out!

A very big thank you to PistolPayback for having the patience of a saint, putting himself through torture just to help me learn. 
A similarly large thank you to Demorome for having the mental fortitude to look at my 'code' for extended periods of time.
Thank you to DLpnd for helping with the last minute testing and guidance.﻿﻿
Thank you to Spifferino for helping with the graphics.﻿
Thanks to lStewieAl for their help.
Thanks to c6dev for putting me on the right path.
Thanks to Stupidhead445, actually nah you made me waste 2 months of my life.
