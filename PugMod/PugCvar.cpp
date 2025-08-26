#include "precompiled.h"

CPugCvar gPugCvar;

void CPugCvar::ServerActivate()
{
    // sv_restart
    this->m_SvRestart = g_engfuncs.pfnCVarGetPointer("sv_restart");

    // allow_spectators
    this->m_AllowSpectators = g_engfuncs.pfnCVarGetPointer("allow_spectators");

    // mp_maxmoney
    this->m_MpMaxMoney = g_engfuncs.pfnCVarGetPointer("mp_maxmoney");

    // mp_freezetime
    this->m_MpFreezeTime = g_engfuncs.pfnCVarGetPointer("mp_freezetime");

    // mp_auto_join_team
    this->m_AutoTeamJoin = g_engfuncs.pfnCVarGetPointer("mp_auto_join_team");

    // humans_join_team
    this->m_HumansJoinTeam = g_engfuncs.pfnCVarGetPointer("humans_join_team");

    // bot_join_team
    this->m_BotJoinTeam = g_engfuncs.pfnCVarGetPointer("bot_join_team");

    // Log Tag
    //
    // Padrão: "PUG"
    this->m_Tag = this->Register("pug_tag", "PUG");

    // Prefixo do comando via say (Jogador)
    //
    // Padrão: "."
    this->m_CmdPrefixPlayer = this->Register("pug_cmd_prefix_player", ".");

    // Prefixo do comando via say (Admin)
    //
    // Padrão: "!"
    this->m_CmdPrefixAdmin = this->Register("pug_cmd_prefix_admin", "!");

    // Mínimo de jogadores por time
    //
    // Padrão: "5"
    this->m_PlayersMin = this->Register("pug_players_min", "5");

    // Máximo de jogadores por time em jogo
    //
    // Padrão: "5"
    this->m_PlayersMax = this->Register("pug_players_max", "5");

    // Total de rounds por período
    //
    // Padrão: "15"
    this->m_Rounds = this->Register("pug_rounds", "15");

    // Total de rounds em overtime
    //
    // Padrão: "3"
    this->m_RoundsOT = this->Register("pug_rounds_ot", "3");

    // Tipo do overtime
    //
    // 0 Votação
    // 1 Jogar overtime
    // 2 Permitir empate
    // 3 Morte súbita
    //
    // Padrão: "0"
    this->m_OvertimeType = this->Register("pug_ot_type", "0");

    // Modo do sistema ready
    //
    // 0 Automático
    // 1 Jogadores usam o sistema ready
    //
    // Padrão: "1"
    this->m_ReadyType = this->Register("pug_ready_type", "1");

    // Lista do sistema ready
    //
    // 0 Exibir lista completa de Prontos / Aquecendo
    // 1 Quantidade de jogadores Prontos / Aquecendo
    //
    // Padrão: "0"
    this->m_ReadyList = this->Register("pug_ready_list", "0");

    // Tempo em segundos para iniciar a partida no modo aquecimento
    // Contagem a partir do momento em que todos jogadores estiverem no jogo
    //
    // Padrão: "60.0"
    this->m_ReadyTimeLimit = this->Register("pug_ready_time_limit", "60.0");

    // Modo de times
    //
    // 0 Votação
    // 1 Capitães
    // 2 Misturar Times
    // 3 Nenhum
    // 4 Balancear Skill
    // 5 Trocar Times
    // 6 Round Faca
    //
    // Padrão: "0"
    this->m_TeamType = this->Register("pug_team_type", "0");

    // Opções no menu de votação da escolha dos times
    //
    // a Capitães
    // b Misturar Times
    // c Nenhum
    // d Balancear Skill
    // e Trocar Times
    // f Round Faca
    //
    // Padrão: "abcdef"
    this->m_TeamOption = this->Register("pug_team_option", "abcdef");

    // Restringir armas
    // Cada posição é um item (Ver enum ItemID para as posições)
    //
    // 0 Inativo
    // 1 Ativo
    //
    // Padrão: "000000000000000000000000000000000000000"
    this->m_RestrictItem = this->Register("pug_restrict_item", "000000000000000000000000000000000000000");

    // O Tempo limite das votações do pug
    //
    // Padrão: "15.0"
    this->m_VoteDelay = this->Register("pug_vote_delay", "15.0");

    // O mapa atual terá uma escolha de mapa
    // Ps. Não utilizado nas confirações
    //
    // 0 Inativo
    // 1 Ativo
    //
    // Padrão: "1"
    this->m_VoteMap = this->Register("pug_vote_map", "0");

    // O último mapa do servidor
    // Ps. Não utilizado nas confirações
    //
    // Padrão: ""
    this->m_LastMap = this->Register("pug_last_map", STRING(gpGlobals->mapname));

    // Forçar o round faca no mapa atual
    //
    // 0 Inativo
    // 1 Ativo
    //
    // Padrão: "0"
    this->m_KnifeRound = this->Register("pug_knife_round", "0");

    // Questionar fim da partida ao desconectar um jogador
    // Quando não houver espectadores
    //
    // 0 Inativo
    // 1 Ativo
    //
    // Padrão: "1"
    this->m_VoteEnd  = this->Register("pug_vote_end", "1");

    // Tipo do texto dos placares na tela
    //
    // 0 Por extenso
    // 1 Mostrar Placares TRs e CTs
    //
    // Padrão: "0"
    this->m_ScoreText = this->Register("pug_score_text", "0");

    // Ativar modo Deathmatch
    // Se inativo, será o modo aquecimento comum
    //
    // 0 Inativo
    // 1 Ativo
    //
    // Padrão: "1"
    this->m_DM_Enable = this->Register("pug_dm_enable", "1");

    // Ocultar kill feed de outros jogadores
    //
    // 0 Inativo
    // 1 Ativo
    //
    // Padrão: "0"
    this->m_DM_HideKillFeed = this->Register("pug_dm_hide_kill_feed", "0");

    // Indicador de acertos na tela
    //
    // 0 Inativo
    // 1 Ativo
    //
    // Padrão: "1"
    this->m_DM_HitIndicator = this->Register("pug_dm_hit_indicator", "1");

    // Somente aceita acertos na cabeça
    //
    // 0 Inativo
    // 1 Ativo
    //
    // Padrão: "0"
    this->m_DM_HSOnlyMode = this->Register("pug_dm_hs_mode", "0");

    // Exibir Taxa de mortes / frags e taxa de HS no hud
    //
    // 0 Inativo
    // 1 Ativo
    //
    // Padrão: "1"
    this->m_DM_HudKDRatio = this->Register("pug_dm_hud_kd_ratio", "1");

    // Pisca a tela ao matar outro jogador
    //
    // 0 Inativo
    // 1 Ativo
    // 2 Somente no HS
    //
    // Padrão: "2"
    this->m_DM_KillFade = this->Register("pug_dm_kill_fade", "2");

    // Recuperar vida após frag
    //
    // 0 Inativo
    //
    // Padrão: "15"
    this->m_DM_KillHP = this->Register("pug_dm_kill_hp", "15");

    // Recuperar vida após frag (HS)
    //
    // 0 Inativo
    //
    // Padrão: "40"
    this->m_DM_KillHPHS = this->Register("pug_dm_kill_hp_hs", "40");

    // Recuperar armadura após frag
    //
    // 0 Inativo
    // 1 Ativo
    // 2 Somente no HS
    //
    // Padrão: "1"
    this->m_DM_KillRepairArmor = this->Register("pug_dm_kill_repair_armor ", "1");

    // Exibir HP recuperado após frag
    //
    // 0 Inativo
    // 1 Ativo
    //
    // Padrão: "1"
    this->m_DM_KillHealedMsg = this->Register("pug_dm_kill_hp_msg ", "1");

    // Ativar som ao realizar frag
    //
    // 0 Inativo
    // 1 Ativo
    // 2 Somente no HS
    //
    // Padrão: "2"
    this->m_DM_KillSound = this->Register("pug_dm_kill_sound", "2");

    // Exibe frags no lugar do dinheiro
    //
    // 0 Inativo
    // 1 Ativo
    //
    // Padrão: "1"
    this->m_DM_MoneyFrag = this->Register("pug_dm_money_frag", "0");
}

cvar_t *CPugCvar::Register(const char *pszName, const char *pszValue)
{
    cvar_t *pPointer = g_engfuncs.pfnCVarGetPointer(pszName);

    if (!pPointer)
    {
        this->m_Data[pszName].name = pszName;

        this->m_Data[pszName].string = strdup(pszValue);

        this->m_Data[pszName].flags = (FCVAR_SERVER | FCVAR_PROTECTED | FCVAR_SPONLY | FCVAR_UNLOGGED);

        g_engfuncs.pfnCVarRegister(&this->m_Data[pszName]);

        pPointer = g_engfuncs.pfnCVarGetPointer(this->m_Data[pszName].name);

        if (pPointer)
        {
            g_engfuncs.pfnCvar_DirectSet(pPointer, pszValue);
        }
    }

    return pPointer;
}