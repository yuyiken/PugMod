<h3 align="center">Counter-Strike 1.6 Match Plugin</h3>
<p align="center">Counter-Strike 1.6 Match Plugin for ReHLDS and MetaMod.</p>

<p align="center">
    <a href="https://github.com/SmileYzn/pugmod/issues">
    <img alt="GitHub Issues" src="https://img.shields.io/github/issues-raw/smileyzn/pugmod?style=flat-square">
    </a>
    <a href="https://github.com/SmileYzn/pugmod/actions">
    <img alt="GitHub Workflow Status" src="https://img.shields.io/github/actions/workflow/status/SmileYzn/pugmod/build.yml?branch=main&label=Build&style=flat-square">
    </a>
</p>

<h4>Description</h4>
<p>A metamod plugin to run automnated competitive Counter-Strike 1.6 matches</p>

<h4>Requisites</h4>
<ul>
    <li>ReHLDS</li>
    <li>ReGameDLL_CS</li>
    <li>Metamod</li>
</ul> 

<h4>Features</h4>
<ul>
    <li>CS:DM mode that can be enabled while match is not started</li>
    <li>CS:DM mode with options via <b>'M'</b> key</li>
    <li>CS:DM weapons menu <b>'G'</b> key</li>
    <li>CS:DM spawn editor</li>
    <li>Ready System: Player must type <b>.ready</b> on chat to start game</li>
    <li>Automatic Timer: Match starts while all palyers are connected</li>
    <li>Support maxrounds (mr) match with best of 30 rounds</li>
    <li>Infinite Overtime support</li>
    <li>Vote for Overtime play, sudden death round, or end game tied</li>
    <li>Server slots management with spectators and HLTV slots support</li>
    <li>Vote Map</li>
    <li>Players can choose the method of picking teams</li>
    <li>Knife Round supported</li>
    <li>A complete admin menu to manage match and server</li>
    <li>Admin can kick, ban, kill, change map, change team of player, control match</li>
    <li>Player and Admin commands in game</li>
    <li>Commands in server console</li>
    <li>Settings and configurations via console variable cvars</li>
    <li>Weapon restriction control</li>
    <li>Anti Retry</li>
    <li>Game name changer on server list change</li>
    <li>Ask to record a demo on client side while match is started/running</li>
    <li>Ask to end pug while an player disconnect from game</li>
    <li>Round end stats in screen and commands</li>
    <li>A complete menu for players to: Vote Kick, Vote Map or Vote to stop match</li>
    <li>Translation system support</li>
    <li>Anti Flood system for chat and radio messages</li>
    <li>Automatic ban of players that rage quit</li>
    <li>Supports for Windows and Linux Systems</li>
</ul> 

<h4>Settings</h4>

<details>
    <summary>Pug Mod Variables</summary>
    
| Variable | Default | Description |
|----------|----------------|------------|
| pug_tag | "PUG" | Log Tag |
| pug_language | "br" | Server language |
| pug_cmd_prefix_player | "." | Player command prefix |
| pug_cmd_prefix_admin | "!" | Admin command prefix |
| pug_cmd_flood_time | "0.75" | Time between commands to consider a flood attempt |
| pug_cmd_flood_repeat | "3.0" | Maximum flood attempts before blocking |
| pug_cmd_flood_next_time | "4.0" | Anti-flood block time |
| pug_players_min | "5" | Minimum players per team |
| pug_players_max | "5" | Maximum players per team in game |
| pug_rounds | "30" | Total rounds per map |
| pug_rounds_ot | "6" | Total overtime rounds per match |
| pug_ot_type | "0" | Overtime type (0 Vote / 1 Play overtime / 2 Allow game end tied / 3 Sudden death) |
| pug_ready_type | "1" | Ready system mode (0 Disabled / 1 Automatic / 2 Players use ready) |
| pug_ready_list | "0" | Ready system list (0 Full list / 1 Number only) |
| pug_ready_time_limit | "60.0" | Time in seconds to start the match in warmup mode |
| pug_team_type | "0" | Team system mode (0 Vote / 1 Captains / 2 Shuffle / 3 None / 4 Balance / 5 Switch / 6 Knife) |
| pug_team_option | "abcdef" | Options in the team choice vote menu |
| pug_restrict_item | "000000000000000000000000000000000000000" | Restrict weapons at the indicated slot |
| pug_vote_delay | "15.0" | The pug vote time limit |
| pug_knife_round | "0" | Force knife round on the current map |
| pug_vote_end | "1" | Prompt end of match when a player disconnects |
| pug_score_text | "0" | Type of score text in chat |
| pug_score_teams | "1" | Keep total team score in the scoreboard |
| pug_score_players | "1" | Keep player scores in the scoreboard |
| pug_motd_file | "cstrike/addons/pugmod/motd.html" | Help file for commands via MOTD |
| pug_motd_admin_file | "cstrike/addons/pugmod/motd_admin.html" | Admin help file for commands via MOTD |
| pug_demo_record | "1" | Ask for client-side demo recording when match starts |
| pug_retry_time | "20.0" | Anti-retry ban time |
| pug_round_stats | "abcd" | Flags to enable stats commands (.hp, .dmg, .rdmg, .sum) |
| pug_round_end_stats | "3" | Display round-end statistics (0 Inactive / 1 Victims / 2 Attackers / 3 Both) |
| pug_dm_enable | "1" | Enable Deathmatch mode |
| pug_dm_hide_kill_feed | "0" | Hide kill feed from other players |
| pug_dm_hit_indicator | "1" | Hit indicator on screen |
| pug_dm_hs_mode | "0" | Only accept headshots |
| pug_dm_hud_kd_ratio | "1" | Show kill/frag rate and HS rate on HUD |
| pug_dm_kill_fade | "1" | Flash the screen on kill (HS) |
| pug_dm_kill_hp | "15" | Recover health after a frag |
| pug_dm_kill_hp_hs | "40" | Recover health after a headshot |
| pug_dm_kill_repair_armor | "1" | Recover armor after a frag |
| pug_dm_kill_hp_msg | "1" | Show HP recovered after a frag |
| pug_dm_kill_sound | "1" | Enable sound on frag (HS) |
| pug_dm_money_frag | "1" | Show frags instead of money |
</details>