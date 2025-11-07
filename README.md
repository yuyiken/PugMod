## Counter-Strike 1.6 Match Plugin

<p align="center">Counter-Strike 1.6 Match Plugin for ReHLDS and MetaMod.</p>

<p align="center">
    <a href="https://github.com/SmileYzn/pugmod/issues"><img alt="GitHub Issues" src="https://img.shields.io/github/issues-raw/smileyzn/pugmod?style=flat-square"></a>
    <a href="https://github.com/SmileYzn/pugmod/actions"><img alt="GitHub Workflow Status" src="https://img.shields.io/github/actions/workflow/status/SmileYzn/pugmod/build.yml?branch=main&label=Build&style=flat-square"></a>
</p>

<h3>Description</h3>
<p>A metamod plugin to run automnated competitive Counter-Strike 1.6 matches</p>

<h3>Requisitos</h3>
<ul>
    <li>ReHLDS</li>
    <li>ReGameDLL_CS</li>
    <li>Metamod</li>
</ul> 

<h3>Features</h3>
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

| Variable | Default | Possible |
|---|---:|---|
| pug_tag | "PUG" | qualquer string (log tag) |
| pug_language | "br" | "br", "en" (ou qualquer código de idioma suportado) |
| pug_cmd_prefix_player | "." | qualquer string/char (prefixo de comandos de jogador) |
| pug_cmd_prefix_admin | "!" | qualquer string/char (prefixo de comandos admin) |
| pug_cmd_flood_time | "0.75" | número (segundos entre comandos para detectar flood) |
| pug_cmd_flood_repeat | "3.0" | número (tentativas máximas antes de bloquear) |
| pug_cmd_flood_next_time | "4.0" | número (tempo do bloqueio anti-flood em segundos) |
| pug_players_min | "5" | inteiro >=0 (mínimo por time) |
| pug_players_max | "5" | inteiro >=0 (máximo por time em jogo) |
| pug_rounds | "30" | inteiro (rounds por mapa) |
| pug_rounds_ot | "6" | inteiro (rounds de overtime total) |
| pug_ot_type | "0" | 0=Vote, 1=Play overtime, 2=Allow tie, 3=Sudden death |
| pug_ready_type | "1" | 0=Disabled, 1=Automatic (Timer), 2=Players use ready |
| pug_ready_list | "0" | 0=Full list, 1=Numbers only |
| pug_ready_time_limit | "60.0" | segundos (timer para iniciar a partida em warmup) |
| pug_team_type | "0" | 0=Vote,1=Captains,2=Shuffle,3=None,4=Balance Skill,5=Switch,6=Knife Round |
| pug_team_option | "abcdef" | combinação das letras a-f (a=Captains,b=Shuffle,c=None,d=Balance,e=Switch,f=Knife) |
| pug_restrict_item | "000000000000000000000000000000000000000" | string por slot (cada char: 0=inactive,1=active) |
| pug_vote_delay | "15.0" | segundos (delay para votações) |
| pug_knife_round | "0" | 0=Inactive, 1=Active |
| pug_vote_end | "1" | 0=Inactive, 1=Active (prompt end on disconnect) |
| pug_score_text | "0" | 0=Written style, 1=Both scores style |
| pug_score_teams | "1" | 0=Reset per period, 1=Keep total across periods |
| pug_score_players | "1" | 0=Reset per period, 1=Keep across periods |
| pug_motd_file | "cstrike/addons/pugmod/motd.html" | caminho ou URL do MOTD (string) |
| pug_motd_admin_file | "cstrike/addons/pugmod/motd_admin.html" | caminho ou URL (string) |
| pug_demo_record | "1" | 0=Inactive, 1=Active (ask client to record on match start) |
| pug_retry_time | "20.0" | segundos (anti-retry ban time); 0 = inactive (comentário sugere que 0 inativa) |
| pug_round_stats | "abcd" | flags: a=.hp, b=.dmg, c=.rdmg, d=.sum (combinação de letras) |
| pug_round_end_stats | "3" | 0=Inactive,1=Victims,2=Attackers,3=Both |
| pug_dm_enable | "1" | 0=Inactive,1=Active (Deathmatch mode) |
| pug_dm_hide_kill_feed | "0" | 0=Inactive,1=Active |
| pug_dm_hit_indicator | "1" | 0=Inactive,1=Active,2=Active+ThroughWalls |
| pug_dm_hs_mode | "0" | 0=Inactive,1=Only headshots |
| pug_dm_hud_kd_ratio | "1" | 0=Inactive,1=Show KD/HS rate |
| pug_dm_kill_fade | "1" | 0=Inactive,1=Flash screen on kill (HS) |
| pug_dm_kill_hp | "15" | inteiro (HP recover after frag); 0 = inactive |
| pug_dm_kill_hp_hs | "40" | inteiro (HP recover on HS); 0 = inactive |
| pug_dm_kill_repair_armor | "1" | 0=Inactive,1=Active,2=Only on HS |
| pug_dm_kill_hp_msg | "1" | 0=Inactive,1=Show HP recovered message |
| pug_dm_kill_sound | "1" | 0=Inactive,1=Play sound on kill (HS) |
| pug_dm_money_frag | "1" | 0=Inactive,1=Show frags instead of money |
