# TODO

## Misc bugs

- we dont check if a tag already exists before adding currently

- when mouse enters viewport, deselect any nuklear windows open (heiarchy fixed location ones ofc)

- changing Z in editor only changes the value, it does not re sort the list.
  TODO: impl re sorting the list on z value change

- all #defines we should add a #ifndef before defining them so they can technically be set by user if they want to

- CCD substepping should be calculated based on the framecap

- create editor settings with defaults if it doesnt exist

- editor gotta get rid of bools tracking windows open, use the ui.c is exist function instead

- locking the viewport should be a refcount for how many windows are currently locking, that way windows can specify viewport should lock with them but if we open a ton of them at once there is no issue closing one, we still know to lock until all blocking are closed

- easily duplicate selected entity or delete it and other quick action shortcuts

- bug with changing scenes the title bar goes inactive until focusing another nuklear window then re attempting

## moving selected entity

CTRL clicking and dragging or shift clicking and dragging could move and scale the selected entity respectively without having to open the properties window

## thoughts

way to tottally save state of editor, so that we could have a play button that we click which just resumes running the scene at its current point as if it wasnt in editor- that way we dont have to build in order to quickly test like a physics collision

## premature optimization is the root of all evil

- who cares if we are reloading duplicate assets for the next scene, we could easily update this for fun later on, not force the optimization now when it doesnt matter. the engine is so lightweight anyways

## considerations

a very soft rule is please no spaces in any file names (they will break in most cases)

we wont see colliders or paintbounds on cameras because thery dont get rendererd

## json best practice

- we currently mix fields of underscore and spaces, we need to pick one and rewrite all to be the same

## Documentation

go back sometime and add < to every struct field so the docs include the field comment
use groups more

## color font stuff

in this case selecting fonts and colors could just be a dropdown in each editor menu possible

combo boxes reading from the persisted style tracking state... ?

## window activeness

we can actually change the active window inside nuklear.h:5674 with ctx->active.
The issue is that we dont have a reliable way to detect whether the user is doing something that we wouldnt want to defocus... other than locked viewport

## kalgdhljkdhglk

editor save prefs when closed, like the ratio and location of the editor window. also serialize all the overlays and options
better algorithm for selecting objects when clicking over them (unity has ruined me)
nuklear showing 1px margin scrollbars sometimes horizontally (depends on scaling)
reapproach editor scaling to be better to give higher size viewport eventually, also maybe repaint during rescale so its not so yuck
add existing project in launcher

## view ratios

to be honest, I am only partially the way there with the work im doing on the window ratio scaling and the letter and pillar boxing. in the future, we should be allowing IN EDITOR custom camera sizes and window sizes so that the camera size is automatically projected to a maintained window size and everything works with every valid value

## misc ten thousand

you should really remove the console logic from logging

when you do that you should overhaul it to be better and also interface with instead of toggle you can "set" a value to a input number or boolean

## nuklear shit

combine multiple colors node editor
or use something for node editor

## refacotr

editor_ui.c is a bloated mess - i moved a little bit of it just now but its not great. if you ever have to touch it again (you will) then refactor it

## misc 202304309505943

viewport seems to overlap title bar for bottom two editor panels

## cache size counter

i like the idea of this
also lua context size
also a console command to list cached textures by origin of type and name

## problems

so I think the idea of the cache is alright, scenes are not massive so its ok to store literally anything that will ever be used in it. The thing is we dont get to intelligently destroy cache items becuase we have no idea how many things are actually using them. if you ever revisit the cache system you need to implement something like refcounting, which shouldnt be too bad since we have knowledge of the preprocessed scene file but can also add to the count in runtime if we get a cache hit

deltatime cooldown on editor inputs before updating them

TODO left;
cooldown so no spamming cache reload
same thing with images src

improve the console its lowkey ass. should be genuinely dynamic and auto select bottom input on open, and remember its last positions

maybe build and run should save first

## other QOL

like unity, if we are dragging a nuklear input please wrap the mouse around to the window border so we dont have to let go to redo

## existing known bugs

when renaming src input image dissapears without missing texture

god i hate the lines scaling thing

on x11 (wayland not tested or windows) sometimes fullscreen mode will literally disable your fucking monitor - maybe this stopped with last fix?

## rambling part ten billion

- memory leak when reload spamming over time
- right click popup menu to duplicate delete rename etc on entity buttons in list
- drop downs listing fonts and colors from the cache instead of typing in

## ecs

null checks for assigning and removing and blah blah fix all the ecs it doesnt have enough sanity checks - this is neccessary for the lua scripting system

## macro debugging

for all the stuff we only use debugging consider settings macros for it so its left out otherwise. also can cmake set macros? we could do that for platform dependance

## logging

rename logging symbols everywhere to be YE_LL_LEVEL

## lua scripting

read through references, you are doing bad practice for a lot of things like the naming convention

## doing the plugin system

how could we persist certain scripts or entities outside of refreshing when loading scenes? seperate list?

to call a lua function defined in a trick from lua scripts in engine it doesnt technically need to be registered yet because we can invoke it through a C helper function by name across lua states. but if we wanted to we could add a custom trick register lua callback

maybe in the future include src url in trick.yoyo which can auto update if you ask it to

something something inclues let other tricks build off of other tricks like modding sdks

## building

editor should not crutch on a build path existing, use its own

other:

progress bar would be cool, or a new console window with the build output

## editor

delete button push styles macro or function

## tricks plugin manager

add to meta last edited and then track so if user adds new item we auto refresh next launch instead of foccing them to click refresh

You need to majorly refactor this plugin manager. this is some of the worst code ive written in a long time. I need to stop programmign when I cant think straight.

## cmake build api shit

cmake does not like to expose macros defined in source files to other source files, it has to be in headers. I wish I could just allow #define USE_PRE_LOOP to work but it seems like the user will have to go into a header file somewhere to define it no matter what if we really want it to work

```c
/*
    This file should have automatically been placed in your custom/include
    directory. This file contains all the C api definitions for the basic
    scripting callbacks and default game behavior.

    The intended use of this api is to make it easy to define custom C behavior,
    for example: if you wanted to load some custom data before the game initializes
    you could do the following in a new file like custom/src/mybehavior.c:
    ```c
    // include the api
    #include "yoyo_c_api.h"
    
    // tell the compiler you are using this function (the engine will recognize this)
    #define YOYO_PRE_INIT

    // implement the function as defined by its signature in the header
    void yoyo_pre_init(){
        // any behavior you want!!
    }
    ```
*/
```

## default tricks

there should be a popup in the launcher that lets you choose some default tricks to use, downloading them via submodule and adding them to the project

## new build system

go through and make sure you turn off unnecesary build targets like all the test and possibly SDL features that you arent using in the engine

## something

editor path is hard coded into settings now, rework this system

## misc tricks thoughts

ALSO TODO TOMORROW: editor build will freeze totally during the build. why?

TODO: fuck, i forgot to allow the game to link its own custom libraries. Force this to be through plugins?

## editor improvements

the editor should be a lot better about being project agnostic, so at any time we can open a new project, or maybe not. unity doesnt do that.

## todo tomorrow

games need to be able to link their own custom libaries, this might go hand in hand with the new trick stuff (tricks also need to be able to link their own deps).

this possibly takes the form of you exposing some cmake paths for tricks to manually do whatever with.

alt thought: tricks could technically be containers for libraries or dependancies, you would just need to port those deps to be inside a trick...

maybe you just construct a vendor folder and link anything in there with the game, while adding a new target before building the game to link against whatever deps..

this seems to lead to custom cmakelists.txt for the user to edit no matter what, not sure there is a way to do this all automatically

## trick auto updates

would be really hype to identify if we downloaded a trick from github and click check for update

## yep files

in the future we could hash the integer handles to get a unique id which would decrease file size slightly

a .yepignore would be nice to know which files we deliberately dont want to pack

## new systems that need done

## what if we exposed a really easy to use save data system?

- could just be a small wrapper on top of the existing json functionality
- this should come after 1.0, maybe even as an extension

## new platforms

- macos support seems to work out of box, need toolchain for osxcross

## misc x9999

- icon rework kinda goes hand in hand with engine resource stuff because by default rn we are using an icon in egnine resources

## youre gonna hate me for this

we should probably actually have compiler macros and shit to compile out the engine being in editor mode now that its add subdired
this wouldnt be too bad because its more just cutting out lines that we dont use and conditionally specifying paths and means of access, we can still leave the actual editor functionality in terms of visual debugging effects in the editor struct but turn it into like debugging field or something

## editor gdb

additional arg for gdb or something so that we instantly print out logs isntead of them not showing up

## editor improvements x2

the layout and panels could be cleaned up

would be nice to have overlays and preferences (paintbounds) sync to settings

custom borderless layout like UE5

stretch viewport gone

condense options

actually be able to drag and resize things

run the game without building (should be possible)

(way mega stretch) lua editor inside the editor

save confirmations and popups are really important

build currently blocks while executing:

```c
/*
    Investigation into fixing blocking builds:
    - pthreads
    - pipes to communicate with build script
*/
```

## new mixer

in terms of music tracks, we really only need one right now, but in the future we could expand the system to have a table of them for easy blending between different intensities and instruments

there are fading and other channel effects that could be fun to wrap around (and pretty easy too)

## qol editor

little widget icons for stuff like audio listeners and emitters

## misc 9123

I dont think every component has its relativity serialized rn

bumped nuklear ver for disabled feature, check what else has been added and if we want to use it. also implement the disabled on applicable places

check your style impl because with disabled widgets the checkbox does not fade color

editor needs scene configureation/settings panel, add shortcut + menu bar + button in bottom maybe

prefabs would be nice but not sure how to best integrate that system

move over to build numbers for major? or semver... semver is hard to keep up with and I want to create major builds more often than build numbers would serve...

## mixer audio source

it might be smart to use the height as the falloff start range, because unity does that and its also smart to be able to controll where it sounds full volume and where it starts to fall off (this should not affect the mix positioning which is still relevant even if we are not falling off yet)

audio emitters are constantly playing, and just muted if outside range

## otha

fading in and out scene loader transitions

mixer start falloff range

audio listener specification rather than default camera center

## tmaps

### editor tmaps

honestly, lets just keep this basic tilemap impl for now, and in the future that could be a reworkable thing.
It involves more manual labor on the developer using the renderer but its really just typing in the pixels each time.

To make a true tilemap akin to unity it requires us to represent a ton of tiles in one renderer on one entitity potentially, unless we are doing like an entity painter thing,

It is technically possible to represent a grid logically in the editor, and make a brush tool to paint entities from presets, so maybe that could compound with this tilemap system

## LHJGFKJHDFKJGHKJDFGHJKFDHJG

audio chunk count removed or reworked

## slk fjlkgjh dfk gjhlk

should make a just normal run button for non rebuild conf changes

fade in / fade out would be extremely nice for scene transitions and splash screen

typing script path that doesnt exist creates it in editor

## future enhancements

- [ ] lua pre compile bytecode
- [ ] editor plugins (lua) which run in editor
- [ ] actual physics system with rigidbody simulation and solver
- [ ] gray out fields in editor when N/A
- [ ] fix the annoying renderer warp pixel bug, as well as the line scaling bug
- [ ] random scrollbars for panels that dont need it (horizontal mainly)
- [ ] asserts for all the things, better error handling (we dont want to be crash prone as I'm sure we are now)
- [ ] big ass refactoring and cleanup, especially of editor code
- [ ] dont rebuild clean object files
- [ ] camera zoom centered in center of viewport not screen

## things noticed from new project not streamlined

must open build settings to generate file or it doesnt exist just hitting build and run

should auto save or warn when building with no save

- for this would need to add a ton of lines everywhere we modify things that mark as dirty

DO NOT USE THE SCENE FILE CREATOR ITS TOTALLY BUGGED AND WILL WIPE YOUR SHIT

easy way for registering lua from C. like a fn quit built in

logging should be totally disabled to file unless we are in debug mode, also shouldnt log things we dont need to in release

add icons through cmake. make this go hand in hand with the current impl which is scuffy at best. we really only care about these icons on windows to lend credibility.

better error reporting for neccessary components - highlight the objects and components in red. ex: camera comps NEED transforms

## emscripten notes

Debugging web apps is a mixed bag. You should compile and link with -gsource-map, which embeds a ton of source-level debugging information into the build, and make sure the app source code is available on the web server, which is often a scary proposition for various reasons.

NO PLUGINS for emscripten (test later)

force zlib and other examples to not build

## other build stuff

we have a release and debug mode, but im not totally convinced it works. at the least the sigs arent stripped because thats a linker option that we are throwwing to compiler. idk what im doing at all

-03 optimizes out the animation system for some reason lmao

## editor saving mark

it kinda makes no sense to show saving... if we are single threaded and blocking and dont update screen before

we dont track every single modification (properties which modify ecs directly) for unsaved dirty marking. how can we get around this without manually creating before and after copies for each field in each frame for comparison?

## misc lskgjhfljfd

clear console button

## discoveries during making game

default build settings

remove engine build path? - its used by build script for engine add subdir. think about how to make this better

tricks/ folder template not generated and if not present cannot build

heiarchy panel put things in a tab thing from overview nuklear so they can scroll inside and we have sticky buttons up top

editor keep last open scene saved when reopened

## screen

better caching and tracking of screen size and aspect ratios and other scaling letterboxing things so we dont recompute it in like ten places

get screen size uses the primary monitor

## notes

you are responsible for making sure a button youre polling for exists, because if youre checking for api state it will return false if it doesnt exist

fix the stretch alignment

## bugs with things

changing wrap length should visually show cutoff or update text TODO

## skjhfjkdfhgkjd

track editor camera position before scene reload and move it back to smae spot after recreated

multi select and duplicate and transform many at once

nuklear preview the .rc icon? could eventually be a cool addition
allow linking custom user libs without modifying build.py directly - maybe this ends up being some way to let them add cmake commands themselves

keepalive macro embedded in engine timer system

## skjhfgkjdfgkljflkjhdflkg hfdkljh

you made a txt file in kate with ideas
could do the unreal thing now. might not be worth the effort for drag tho

## cool others

darwin builds

emscripten wasm, is it possible to make savedata system store in browser? - i saw unity or godot has some kinda browser savedata system with either cookies or local storage

issues with compiler optimization is probably casts. go through and fix them all.

## pre-acerola jam checklist

- I think rounding or cast errors with size_t or other are the cause of the compiler optimizations breaking animation system
- save build additional c flags per platform, so switching to windows will stash previous ones and restore them correctly when target changed
- maybe build.yoyo and settings.yoyo should merge into one settings.yoyo, feels weird that they are seperate

## other features from acerola jam that would be cool

- color tint controller
- ye_synchronize_position_relative called once on init and tracks one object onto another when they move during scene lifetime
- aseprite has a json export for sprite sheets, should add a way to import those automatically
- rounded colliders will be super important. probably also custom mesh colliders (could have made whole map out of one wall collider)
- utility function to pin entities onto others to track
- function to get transform locations but from the center of their renderer or collider or other utilities
- allow setting rotation point through clicking world space in editor and converting that to relative to entity renderer
- easier timer constructor, they are more common than anticipated
- case insensitivity in search bar
- easily close hiearchy when searching to collapse it
- enter key to submit imputs
- button to easily sync collider size to renderer size (rotation included)
- other collider shapes and skews
- expose renderer rect to manually cull instead of black boxes
- table of strings assosciated function pointers, use console commands to toggle engine debugging overlays

## bugs found during acerola jam

- physics system is fried with negative relativity for a collider, its offsetting the new position by the relative distance as well as the dx, dy
- physics system should treat each axis as a seperate calculation, so if youre hitting wall going north east, it should move you north if you are only colliding on the east axis
- copying entities fails if the name gets too long
- serialize or expose center of rotation in editor
- for missing assets, we used to have missing thing for images but yep will exit(1) if a header does not exist, so wrap this for images to ensure we get missing back if needed
- crashes on scene reloads a lot
- colliders are mega scuff, if more than one is touching things get weiiirdd
- adding intro seemed to cause inconsistant loading into entry scene

## im back

current

- dragging rect needs a small pixel gap to make sure we are above tolerance, currently any mouse movement triggers drag

wishlist:

- cpmake for caching downloads of deps
- it is possible to detect unsaved for changes in internal variables, save before we let nuklear handle input and check against original

## stuff

- SDL has a file drop event, could use it to auto import files into resources
  - <https://wiki.libsdl.org/SDL2/SDL_DropEvent>

## uhhh actually doing lua api

- need to close editor and re open for runtime.lua to sync,,,

## new intro

- fade in and out, mix the sound better to fit, redesign a lot

## random

i think editor is bundling its edicons into prod engine.yep's lol

## blah blah blah blah

`lua_script.c` is pretty rancid... weird functions and logic encapsulation

ye_run_console_command() api?

we should make onMount run after all entities are created so we can use getters... will still cause problems for user scripts using getters on objects created on mount by other user scripts but thats kinda on them...

## more luaalualaulalualua

update docs for all functions.. im messing things up :3

we need some kinda lua unit testing for the API to make sure we dont break anything... can automatically exercise every component and API call

## todo 999999

re insert validity checks? or not because its enforced engine C fn side

look for simplifications and refactoring of interface to make easy moving forwards to the other components. every single component we add produces more code to maintain and refactor in the future when we inevitably change it

## ajklhgdfskjhgkjdfgh

we need some entity functions to be private, check intellisense and make sure we start to hide any fields that are a no-no

## general engine enhancements

we need actual ontriggerenter on triggerexit ontriggerstay

## lua wrapper roadmap

- debug renderer system? optional
- add debug flags state wrap?

## future wishlist

a networking wrapper would be awesome.. lot of work though. major version increase for sure, or even a trick... depends how implemented

nested entities / dynamically multiple components? -- probably never feasibly without MAJOR re architecting

prefab system should have made it to 1.0, but oh well

keyframed animation system (like rigging movements not just image animations)

movie player component

some deeper runtime in game UI integration toolkit (maybe not even nuklear)

we should traverse creating entire scene when loading before proceeding to call ONMOUNT for each script component

easy save data system exposed to lua (some kinda json abstraction)

test suite for lua api

its totally doable to add gloval param overrides into the editor ui for scripts, using our own existing mechanisms. Similar to unity or unreal object field inspector

aseprite deep compatibility (super easy workflow, custom imports drag and drop)

## yap sesh commence

could try to optimize size of lua runtime, but its only 70kb rn so not a huge deal

## blah yap yap

FIX THE REALLY LONG STANDING BUG WHERE YOU HAVE TO RECONFIGURE TO REFLECT CHANGES IN YEP

editor heiarchy should paint bounds for components if the components tab is open by default

## optimization and code qual

dare i say, we could probably rewrite most of the lua api to implement the table returns in C

## examples

we need to make a bunch of examples to use yoyoengine before 1.0

here are some ideas:

- on screen keyboard visualizer, press keys and see them show up on screen (helps validate input system)

## my head is exploding

- we could add touchpad support (why)
- do not handle deadzone, thats for the game to do bruh

## finishing engine

I'm planning to do a bunch of small tweaks to bring QOL I couldn't do before

- Small tolerance when beginning editor selection drag
- re-evaluate all the debug paint toggles?
- ram, cpu usage, vram usage in metrics panel
- erase the muscle memory problem of constantly repacking and reconfiguring
  - really dig deep, we should not have to reconfigure.
  - for repacking just auto repack when changes detected, you already have timestamp infrastructure
- tricks update detection git, also just streamline that whole thing. pretty sure the code is dogshit
- scene info note and beautification (should make most editor panels not look like trash)

## improvements

- scene transition effects
- non blocking sockets for networking
- editor combo boxes for colors and fonts

## weird stuff

valgrind somehow runs with editor camera and origin as entities listed?
it also GP faults when scrolling or selecting in editor

youre an amateur developer if you cant fix the viewport jank (scrolling nuklear when not desired)

## bruh bruh bruh bruh

- dump all editor settings to file and load on init, put in editor state
  - stuff like drag tolerance before beginning selection

provide prefs to auto scale ratio of panels in editor
fix scrollbar reset when selecting entity in heiarchy, super annoying for selecting multiple
shift ctrl select entity to do range in heiarchy?

## fun

make better custom color themes, you should do a lookup table thing and port over the hard coded dark background stuff. plus also should have a parameter controlling the tint color of icons (black for light themes)

you could write a custom serializer and reader for a custom designer widget you show the user to let them customize their ui colors

you probably need a whole parameter pass for reading from config files for editor... you did something for engine so check that out and see if its reusable potentially.

when we open settings:

- dump json to struct modeling settings
- once we click save serialize model and apply

allow ratio changing for height of panels (serialize)

## polish

get rid of annoying warnings and errors on init, like default font lookup... default should be a reserved font name

## todo

lua docs highlighting class annotations needs syntax highlighting, even if you havea to do it manually

## eventually todo

start using conventional commits and add the github action to create releases from tags with patch notes.

add custom tinting and blend more per entity

filter search by entities that have components

known bug where debug bool ticks need renderer object on entity to display stuff like collider or audiosource bounds

## cross platform gripes

- yep packing dir traversal
- tricks deleting directory
- macos UI scaling in Nuklear
- thats it?

## alkapivo bugs

- need to run reconfigure on first editor build
- cmake is crazy weird, i think its lua target bricking stuff
- you have to manually set the path to your engine build in the editor, super odd

## BB/fighter stuff

- physics gravity fall towards floor? would cause lot of collisions persistantly, how does unity do it?
- lua scipting pass globals from editor panel.
- lua scripting we should have an implicit SELF table pushed to script that represents the entity they are attached to, since this wont change during the lifecycle of that script's execution

## slkgjdlskgd

collider aspect ratio lock?

## way later

no visual launcher in python that shit is ass

- editor should be the hub, since all you need is a folder. The editor itself can create the blank project templates, open projects, etc

## slkujglkdfhjl

copy paste component into other entity or entity comp

remove component text color red

should eventually do general editor nuklear style pass

help panels scattered around editor? ex: in lua global fields add help icon to explain

## welcome page

custom inline colors?

network calls to get latest versions/updates (should happen anyways)

## libcurl samples

```cmake
# # fetch openssl for libcurl
    # FetchContent_Declare(
    #     openssl
    #     GIT_REPOSITORY https://github.com/openssl/openssl.git
    #     GIT_TAG openssl-3.3
    #     GIT_PROGRESS TRUE
    # )
    # FetchContent_MakeAvailable(openssl)
    # target_link_libraries(${EXECUTABLE_NAME} PRIVATE libssl libcrypto)

    # # fetch and use libcurl for the editor

    # set(BUILD_CURL_EXE off CACHE BOOL "Build curl executable")
    # set(BUILD_SHARED_LIBS off CACHE BOOL "Build shared libraries")
    # set(BUILD_STATIC_LIBS on CACHE BOOL "Build static libraries")
    # set(CURL_DISABLE_INSTALL on CACHE BOOL "Disable installation of curl")

    # FetchContent_Declare(
    #     curl
    #     GIT_REPOSITORY https://github.com/curl/curl.git
    #     GIT_TAG curl-8_8_0
    #     GIT_PROGRESS TRUE
    # )
    # FetchContent_MakeAvailable(curl)
    # target_link_libraries(${EXECUTABLE_NAME} PRIVATE libcurl)
```

## skjldfdkjfhgkjdfhgjkdfhg

nuklear docs have stuff on font rasterizing params, we could fine tune to make font rendering cleaner especially now that text and icons are larger

when you read tomorrow impl the project manager fs ops and remove old thing plus move template around and polish it up (template has bad old shit like camera with no cam comp)

## nuklear malpractice

the custom window manager is not needed, you could nuklear-fu
with find_window etc

you should call nk_popup_close
and nk_popup_end to avoid dangling bad active window state

## editor persist data

~~/var/lib/yoyoengine for projects~~ NO, NOT ANYMORE
~/.local/share/yoyoengine has project_cache.yoyo for project list, projects are put wherever the user wants when opening or creating them
/opt/zoogies/yoyoengine for editor install

make and copy a desktop file so it shows up properly

## yoyojam

- local managed dir /var/lib/yoyoengine project management
- do not actually delete them when clicking delete, but tracking seperate projs is annoying
- open in how to not be annoying with git tracking
- init new projects with git?
- how do we actually want to relate engine source to projects? DECIDE THIS!
  - prob remove engine build path - or not, it just needs to be to local copy
  - what about when people want to fork but its a submodule?
- make auto update actually call script

## kjdsfhkjfdhgkjdfhgkjfd

expose uninstall and updater through GUI

## pickup tomorrow

editor create new project, you need to ideate about how engine is customized per project. if you do template you need to clone template from git, could submodule first then copy template. when you submodule you need to match latest tag and not main

^ lowkey kinda bad because you would need internet to make projects, whereas everything else is offline rn... but submodule would require online too...

go through and add browse to every field

## design decisions

creating projects should require internt, because you need it to cmake configure. so either way once you have a project downloaded you can be offline, but to configure and create it you need internet.

## lksfjh gkjfdsgkjdf

eventually when release is made will need to test locally without installed versions, maybe there are conflicts there

## FINISHING 1.0

### Major new systems/reworks

#### Callback event Subscriptions for C api

- Rather than macro wizardry, you should have one function which the user can use to register callbacks for engine events.

#### Switching to semver

- change every location in code
  - editor
    - macro version
    - auto update parser
  - engine
    - macro
    - splash screen auto generation
- engine and editor are one unit, maybe they have the same version

#### Polish

- persist editor settings

### Bug Squash Megalist

- When opening to welcome page, fullscreen the window and then load into a project. you will notice a weird viewport bug that is fixed by resizing the window.
- Resizing window spams terminal
- empty color or font change buffers spam errors
- editor repacking edicons into games (yepignore?)
- when zenity prompts, we minimze and then maximize which destroys the active reference in nuklear (even though the sdl backend does not touch this)
  - it might actually be popups doing this, not zenity
- <https://github.com/Immediate-Mode-UI/Nuklear/issues/653>
- console underneath group panels on welcome page

## QOL tasking megalist

- remove all absolute paths to /ryan/ in repo
- move recently opened projects to top in project manager
- for visual editor people, things like picking bad paths need some kind of feedback visually rather than just a console warning

## ----------------------------

## CONTINUE BRAINROT

- add some multithreading for building, so we have a progress bar and spit out logs in term. no reason user cant cancel build in progress as well

## steam of conc

- move engine resources to a static dir in engine install, while editor resources is its own thing that doesnt need moved into engine resources
- should let yep be generic, and users can pack whatever they want into standalone yeps, can use this for an editor yep
- make intro a scene

## plugins rework

use event system
should be drag and drop, so they build in isolation and as long as they exist inside the distributed tricks dir they will run, ie: a modder can drop in a new trick and it will be loaded

## huge future improvement projects

- Pipe between running game and editor to live debug and monitor

## tomorrow

weird ass inconsistant behavior

you should create dirs for installer and cache if not exists, gracefully handle

there should be local update option rather than installer from github so you can dist prereleases

## Nuklear fixes / enhancements (give back)

- label icon buttons which have flag to collapse back into just icons depending on resizing
- weird 0px scrollbar bug

yoyoengine specific nk

- fork nuklear, since you have your own changes. use fetchcontent and maintain outside of THIS repo

## things i could do

- maybe new system for editor to track current styles and expose them as drop downs rather than names
  - solves the problem of error spamming and also makes it easier for the user

## todoo

add identifier for the version of yoyoengine a project is configured for, and error if not matching.

might need to add a hub to manage different installs rather than the current approach... maybe the editor can live alongside the project? idek anymore

## automated cicd

- Add back lua runtime concat
- add some actual runnability tests for yoyoeditor
- maybe unit tests for the engine api

## now

- figure out relation of versions, is editor frozen at engine runtime?
  - its actually possible to move dirs outside and keep history... split into core and editor? then docs?
- editor should look in its local bin dir to get the project cache, create if not existant
- projects should track the default editor that created them, and warn if importing into a different one

## later

- move splash screen into scene file, allow clicking to skip, or any key press
- scene fading in and out? DONT OVERSCOPE!!

## lksdfjlkdfjgl

engine splash screen is so outdated lmao...

yoyolick/yoyoengine

## random ideas

toasts for errors

option to auto focus terminal when error shows in editor
