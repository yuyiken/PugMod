<h3 align="center">Counter-Strike 1.6 Match Plugin</h3>
<p align="center">Counter-Strike 1.6 Match Plugin for ReHLDS and MetaMod</p>

<p align="center">
    <a href=""><img src="https://img.shields.io/github/actions/workflow/status/SmileYzn/pugmod/build.yml?branch=main&label=Build&style=flat-square"></a>
    <a href="https://github.com/SmileYzn/pugmod/issues"><img src="https://img.shields.io/github/issues-raw/smileyzn/pugmod?style=flat-square"></a>
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
    <li>Game name changer on server list change</li>
    <li>Ask to record a demo on client side while match is started/running</li>
    <li>Ask to end pug while an player disconnect from game</li>
    <li>Round end stats in screen and commands</li>
    <li>A complete menu for players to: Vote Kick, Vote Map or Vote to stop match</li>
    <li>Translation system support</li>
    <li>Anti Flood system for chat and radio messages</li>
    <li>Automatic ban of players that rage quit from game</li>
    <li>Smoke Bug Fix</li>
    <li>Supports for Windows and Linux Systems</li>
</ul>

<h4>Settings</h4>

<details>
    <summary>Pug Mod Variables</summary>

| CVAR | Default |
|------|---------|
| pug_tag | PUG |
| pug_language | en |
| pug_cmd_prefix_player | . |
| pug_cmd_prefix_admin | ! |
| pug_cmd_flood_time | 0.75 |
| pug_cmd_flood_repeat | 3.0 |
| pug_cmd_flood_next_time | 4.0 |
| pug_players_min | 5 |
| pug_players_max | 5 |
| pug_rounds | 30 |
| pug_rounds_ot | 6 |
| pug_ot_type | 0 |
| pug_ready_type | 1 |
| pug_ready_list | 0 |
| pug_ready_time_limit | 60.0 |
| pug_team_type | 0 |
| pug_team_option | abcdef |
| pug_restrict_item | 000000000000000000000000000000000000000 |
| pug_vote_delay | 15.0 |
| pug_vote_percent | 0.70 |
| pug_knife_round | 0 |
| pug_vote_end | 1 |
| pug_score_text | 0 |
| pug_score_teams | 1 |
| pug_score_players | 1 |
| pug_motd_file | cstrike/addons/pugmod/motd.html |
| pug_motd_admin_file | cstrike/addons/pugmod/motd_admin.html |
| pug_demo_record | 1 |
| pug_rage_quit_time_limit | 0 |
| pug_rage_quit_ban_time | 1440 |
| pug_round_stats | abcde |
| pug_round_end_stats | e |
| pug_player_vote_kick | 1 |
| pug_player_vote_map | 1 |
| pug_player_vote_pause | 1 |
| pug_player_vote_restart | 1 |
| pug_player_vote_cancel | 1 |
| pug_player_vote_surrender | 1 |
| pug_pause_timeout | 60 |
| pug_smoke_bug_fix | 1 |
| pug_dm_enable | 1 |
| pug_dm_hide_kill_feed | 0 |
| pug_dm_hit_indicator | 1 |
| pug_dm_hs_mode | 0 |
| pug_dm_hud_kd_ratio | 1 |
| pug_dm_kill_fade | 1 |
| pug_dm_kill_hp | 15 |
| pug_dm_kill_hp_hs | 40 |
| pug_dm_kill_repair_armor | 1 |
| pug_dm_kill_hp_msg | 1 |
| pug_dm_kill_sound | 1 |
| pug_dm_money_frag | 1 |
</details>