# 2018-10-16

## DCS

+ Built cross-compiler successfully with binutils and gcc.
+ Used examples from [OSDev: Bare Bones][1]
+ Compiled examples on cross-compiler successfully.
+ Installed GRUB. Licensed under GNU General Public license but requires bootloader code to be published. See [Bare Bones][1] for more info.
+ Created a bootable iso using GRUB.
+ Attempted to install QEMU locally, but disk quota was exceeded.
+ Added locations to PATH in .profile

[1]: https://wiki.osdev.org/Bare_Bones

#2018-10-17

##Home

+ Basic C recap

#2018-10-18

##DCS

+ **Supervisor meeting.**
+ DCS administrators increased disk quota.
+ QEMU was successfully installed.
+ Bare bones kernel was built and run successfully.

# 2018-10-20

##Home

+ Researched Linux distributions. Arch Linux will be used for development.
+ Downloaded image and researched partitions.

# 2018-10-24

##Home

+ Installed Arch onto 50GB partition (except bootloader).

# 2018-10-25

##DCS

+ Supervisor meeting cancelled by Adam Chester.
+ Bare bones kernel functionality extended
  + Newline support
  + Terminal scrolling
+ Implemented bouncing balls simulation.
+ Read [OSDev: Going Further on x86][2] to understand potential issues and to get a "broader perspective".
+ Reviewed [OSDev: Meaty Skeleton][3] and planned first steps.

[2]: https://wiki.osdev.org/Going_Further_on_x86
[3]: https://wiki.osdev.org/Meaty_Skeleton

##Next

+ Start writing new OS using Meaty Skeleton as a reference. Understand why design choices have been made.

# 2019-02-04

##Home

+ Finally got going again.
+ Added GDT.
  + Examples on OSDev and from [Bran's Kernel Development Tutuorial][4] used intel assembly, but gcc uses AT&T style. Converted all but long jump instruction.
  + Used extremely helpful [StackOverflow post][5] which contained working GDT loading.
  + Kernel is in flat protected mode, so used setup for that.

[4]: http://www.osdever.net/bkerndev/Docs/gdt.htm
[5]: https://stackoverflow.com/questions/49438550/assembly-executing-a-long-jump-with-an-offset-with-different-syntax

#2019-02-17

+ Implemented IDT and ISRs
  + Used mainly Bran's tutorial

#2019-02-21

+ Added IRQs
+ Added timer support
  + Had to use different timer mode to tutorial
+ Added keyboard support
+ Added number printf support

#2019-02-23

+ Added paging
+ Moved kernel to higher half
+ Added long and unsigned number printf support
