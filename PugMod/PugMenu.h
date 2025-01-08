#pragma once

constexpr std::array<const char*, 3> g_Menu_Sound =
{
    "spk common/menu1", // 0 -> Next / Next
    "spk common/menu2", // 1 -> Select Normal
    "spk common/menu3", // 2 -> Select Disabled
};

enum E_MENU
{
    DM_OPTIONS,
    DM_EQUIP,
    DM_WEAPONS,
    ME_VOTE_MAP,
    ME_VOTE_TEAM,
    CP_LEADER_MENU,
    ME_VOTE_SWAP_TEAM
};

typedef struct S_MENU_ITEM
{
    int Info;
    std::string Text;
    bool Disabled;
    int Extra;
} P_MENU_ITEM, *LP_MENU_ITEM;

class CPugMenu
{
public:
    void Clear();
    void Create(std::string Title, bool Exit, int HandleType);
    void AddItem(int Info, std::string Text, bool Disabled, int Extra);
    void AddItemFormat(int Info, bool Disabled, int Extra, const char *Format, ...);
    void Show(CBasePlayer *Player);
    void Hide(CBasePlayer *Player);
    bool Handle(CBasePlayer *Player, int Key);
    void Display(CBasePlayer *Player, int Page);
    void ShowMenu(CBasePlayer *Player, int Slots, int Time, std::string Text);
    void ReplaceAll(std::string &String, const std::string &From, const std::string &To);

private:
    std::string m_Text = "";
    std::vector<P_MENU_ITEM> m_Data = {};
    int m_Page = 0;
    bool m_Exit = false;
    int m_MaxPageOption = 7;
    int m_Handle = 0;
};

extern std::array<CPugMenu, MAX_CLIENTS + 1> gPugMenu;