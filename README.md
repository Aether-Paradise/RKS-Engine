# About `RKS-Engine`

**`RKS-Engine`** is NOT a game. It is a ROMHack base forked from [ROM-Hacking Hideout's `pokeemerald-expansion`](https://github.com/rh-hideout/pokeemerald-expansion). Please support their work, they've done amazing things for years now.

> ***PLEASE NOTE: RKS-Engine is not yet ready for use. Any potential merge conflicts you may get will be your responsability.***

The goal of this project is to introduce features and code changes that aren't within the scope of pokeemerald-expansion. I would highly recommend that any PRs for new features and changes should be discussed with the Expansion Senate over at the [ROM Hacking Hideout (RHH) Discord server](https://discord.gg/6CzjAG6GZk) before coming here, as there's the possibility that it may be accepted and then trickle down to this repo. I don't plan to remove any features from Expansion, but some implementations may differ.

***Documentation is still work in progress.***

## New Features in RKS-Engine
- ***Blank slate:*** Hoenn and Kanto maps have been removed to make room for your new maps!
- ***Demo Maps***: Try out all features available in environments that you play around in!
- ***Extended Easy Chat Support***: Added support for Gen4+ Species, Moves and Abilities.
  - Does not currently support Bard Songs.
- ***Extended Pokédex support***: Forms are now integrated into Emerald's Pokédex.
  - Flags now track species instead of National Dex number.
  - If an alternate form is obtained before the base form, it will be shown in the list properly.
  - You can press R to cycle between seen forms.
  - Form changes will update the dex flag.
  - Allows to switch what the "base" form is by changing the order in the form table.
- ***More available Saveblock space by default:***
  - Most features that utilize SaveBlock space have been disabled by default. Check `include/config/save.h` to reenable them.
- ***Reordered Species and Item IDs***: Pokémon and Items are now sorted by their introduction order to the series.
- ***Config cleanup***: Configs has been removed due to redundancy:
  - Always `TRUE`:
    - `FREE_EXTRA_SEEN_FLAGS_SAVEBLOCK1`
    - `FREE_EXTRA_SEEN_FLAGS_SAVEBLOCK2`
    - `EXPANSION_INTRO`
- ***Integrated new feature branches:***
  - [Day/Night System in battles](https://github.com/Kasenn/pokeemerald-expansion-kasen/tree/battle-dns) - *by Kasen*
  - [Help Windows](https://github.com/LinathanZel/pokeemerald-expansion/tree/help_window) - *by LinathanZel*
  - ***New UIs enabled via config:***
    - [Sword and Shield Party Menu](https://github.com/montmoguri/pokeemerald-expansion/wiki/Sword-&-Shield-Party-Menu) - *by Montblanc*
      - Includes extended support for Expansion 12v12 battles.

<!--
# About `pokeemerald-expansion`

![Gif that shows debugging functionality that is unique to pokeemerald-expansion such as rerolling Trainer ID, Cheat Start, PC from Debug Menu, Debug PC Fill, Pokémon Sprite Visualizer, Debug Warp to Map, and Battle Debug Menu](https://github.com/user-attachments/assets/cf9dfbee-4c6b-4bca-8e0a-07f116ef891c) ![Gif that shows overworld functionality that is unique to pokeemerald-expansion such as indoor running, BW2 style map popups, overworld followers, DNA Splicers, Gen 1 style fishing, OW Item descriptions, Quick Run from Battle, Use Last Ball, Wild Double Battles, and Catch from EXP](https://github.com/user-attachments/assets/383af243-0904-4d41-bced-721492fbc48e) ![Gif that shows off a number of modern Pokémon battle mechanics happening in the pokeemerald-expansion engine: 2 vs 1 battles, modern Pokémon, items, moves, abilities, fully customizable opponents and partners, Trainer Slides, and generational gimmicks](https://github.com/user-attachments/assets/50c576bc-415e-4d66-a38f-ad712f3316be)
-->
<!-- If you want to re-record or change these gifs, here are some notes that I used: https://files.catbox.moe/05001g.md -->
<!--

**`pokeemerald-expansion`** is a GBA ROM hack base that equips developers with a comprehensive toolkit for creating Pokémon ROM hacks. **`pokeemerald-expansion`** is built on top of [pret's `pokeemerald`](https://github.com/pret/pokeemerald) decompilation project. **It is not a playable Pokémon game on its own.**

# [Features](FEATURES.md)

**`pokeemerald-expansion`** offers hundreds of features from various [core series Pokémon games](https://bulbapedia.bulbagarden.net/wiki/Core_series), along with popular quality-of-life enhancements designed to streamline development and improve the player experience. A full list of those features can be found in [`FEATURES.md`](FEATURES.md).

# [Credits](CREDITS.md)

 [![](https://img.shields.io/github/all-contributors/rh-hideout/pokeemerald-expansion/upcoming)](CREDITS.md)

If you use **`pokeemerald-expansion`**, please credit **RHH (Rom Hacking Hideout)**. Optionally, include the version number for clarity.

```
Based off RHH's pokeemerald-expansion 1.16.0 https://github.com/rh-hideout/pokeemerald-expansion/
```

Please consider [crediting all contributors](CREDITS.md) involved in the project!

# Choosing `pokeemerald` or **`pokeemerald-expansion`**

- **`pokeemerald-expansion`** supports multiplayer functionality with other games built on **`pokeemerald-expansion`**. It is not compatible with official Pokémon games.
- If compatibility with official games is important, use [`pokeemerald`](https://github.com/pret/pokeemerald). Otherwise, we recommend using **`pokeemerald-expansion`**.
- **`pokeemerald-expansion`** incorporates regular updates from `pokeemerald`, including bug fixes and documentation improvements.

# [Getting Started](INSTALL.md)

❗❗ **Important**: Do not use GitHub's "Download Zip" option as it will not include commit history. This is necessary if you want to update or merge other feature branches.

If you're new to git and GitHub, [Team Aqua's Asset Repo](https://github.com/Pawkkie/Team-Aquas-Asset-Repo/) has a [guide to forking and cloning the repository](https://github.com/Pawkkie/Team-Aquas-Asset-Repo/wiki/The-Basics-of-GitHub). Then you can follow one of the following guides:

## 📥 [Installing **`pokeemerald-expansion`**](INSTALL.md)
## 🏗️ [Building **`pokeemerald-expansion`**](INSTALL.md#Building-pokeemerald-expansion)
## 🚚 [Migrating from **`pokeemerald`**](INSTALL.md#Migrating-from-pokeemerald)
## 🚀 [Updating **`pokeemerald-expansion`**](INSTALL.md#Updating-pokeemerald-expansion)

# [Documentation](https://rh-hideout.github.io/pokeemerald-expansion/)

For detailed documentation, visit the [pokeemerald-expansion documentation page](https://rh-hideout.github.io/pokeemerald-expansion/).

# [Contributions](CONTRIBUTING.md)
If you are looking to [report a bug](CONTRIBUTING.md#Bug-Report), [open a pull request](CONTRIBUTING.md#Pull-Requests), or [request a feature](CONTRIBUTING.md#Feature-Request), our [`CONTRIBUTING.md`](CONTRIBUTING.md) has guides for each.

# [Community](https://discord.gg/6CzjAG6GZk)

[![](https://dcbadge.limes.pink/api/server/6CzjAG6GZk)](https://discord.gg/6CzjAG6GZk)

Our community uses the [ROM Hacking Hideout (RHH) Discord server](https://discord.gg/6CzjAG6GZk) to communicate and organize. Most of our discussions take place there, and we welcome anybody to join us!
-->
