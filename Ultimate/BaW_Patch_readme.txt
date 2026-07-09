Hi everyone!
Like you all know, I hate you. And hated you always. May Zeus bring his wrath on your heads. But! It is a special time. Time of gifts and MIRACLES. And I came here today to make you a special gift about this old, but still great game. Gift that will open your eyes and help you see the truth, that behind the bad graphics is hidden something way better.

There are two main reasons, which make the game look worse than it should be:

1)   Low Resolution Textures
The game was designed for 256 x 256 high-resolution textures, but often uses low-resolution (128 x 128), because it “thinks” that there is not enough video memory left. The problem is that the game makes a bad check. It accepts negative values. And DirectDraw of 64-bit Windowses often gives too high values. They are so high, that they are seen as negative and become the reason why low-resolution textures are being forced by the game. Even if Maximum Detail is chosen by the player.

2)   Draw distance of the Landscape.
In the game the landscape “morphs” when you zoom out far enough. The details become much worse. In version 1.00 this distance was pretty short and there was no way to make it better. But in version 1.10 and higher the distance was increased (if Maximum Detail is chosen in the settings).
But because of another bad check, the game “morphs” the landscape much faster than it should and keeps the distance like it was in 1.00. Not using the new feature.
Actually, before choosing the distance, the game checks for three things:

1.   If the “Maximum Detail” is chosen
2.   If the family of the processor is 15.
3.   If SSE2 is supported.

The problem is in the processor check. Most newer processors are much stronger than those of family 15, but are still seen by the game as too weak for good draw distance.

This patch solves both problems by fixing/skipping the checks and allowing the game graphics to be much better. Most players see the bad textures and low draw distance as something natural, because they never saw anything better. Try the patch and check, how the game was originally designed to be. Just put it into the folder with runblack.exe, launch it and type “Fix” in the patch window two times. Then close it and launch the game. Patch works in any Windows starting with Windows 95. It works for the Creature Isle expansion too. I hope, you will enjoy it. This great game deserves it.
Here are few words from the creator of the patch (I am not the creator):
“Now I have one less reason to avoid using Windows 7. But I am not going to abandon my Windows XP so easily.".

P.S. Keep in mind that this patch will fix the game only if the game was cracked. So, if you are playing with CD/DVD, it will not work.

Happy New Year!

-------------------------

Note: This patch is applied by default but if you need to use it, rename "BWUltimate.exe" to "CreatureIsle.exe", apply patch and then rename back to "BWUltimate.exe".