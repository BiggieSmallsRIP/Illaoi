#include "../SDK/PluginSDK.h"
#include "../SDK/EventArgs.h"
#include "../SDK/EventHandler.h"
#include <limits>

PLUGIN_API const char PLUGIN_PRINT_NAME[32] = "Illaoi";
PLUGIN_API const char PLUGIN_PRINT_AUTHOR[32] = "BiggieSmalls";
PLUGIN_API ChampionId PLUGIN_TARGET_CHAMP = ChampionId::Illaoi;

namespace Menu
{
	IMenu* MenuInstance = nullptr;
	
	namespace Combo
	{
		IMenuElement* Enabled = nullptr;
		IMenuElement* UseQ = nullptr;
		IMenuElement* UseW = nullptr;
		IMenuElement* UseWaa = nullptr;
		IMenuElement* UseE = nullptr;
		IMenuElement* UseR = nullptr;
		IMenuElement* Runits = nullptr;
		IMenuElement* Rlabel = nullptr;
	}

	namespace Harass
	{
		IMenuElement* Enabled = nullptr;
		IMenuElement* UseQ = nullptr;
		IMenuElement* UseW = nullptr;
		IMenuElement* UseWaa = nullptr;
		IMenuElement* UseE = nullptr;
		IMenuElement* TowerHarass = nullptr;
		IMenuElement* MinMana = nullptr;
	}
	namespace LaneClear
	{
		IMenuElement* Enabled = nullptr;
		IMenuElement* UseQ = nullptr;
		IMenuElement* UseW = nullptr;
		IMenuElement* MinMana = nullptr;
		IMenuElement* MinMinions = nullptr;
	}
	namespace Killsteal
	{
		IMenuElement* Enabled = nullptr;
		IMenuElement* UseQ = nullptr;
		IMenuElement* UseW = nullptr;
	}

	namespace Misc
	{
		IMenuElement* AutoR = nullptr;
		IMenuElement* Edash = nullptr;
	}

	namespace Drawings
	{
		IMenuElement* Toggle = nullptr;
		IMenuElement* DrawQRange = nullptr;
		IMenuElement* DrawERange = nullptr;
		IMenuElement* DrawRRange = nullptr;
	}

	namespace Colors
	{
		IMenuElement* QColor = nullptr;
		IMenuElement* RColor = nullptr;
		IMenuElement* EColor = nullptr;
	}
}

namespace Spells
{
	std::shared_ptr<ISpell> Q = nullptr;
	std::shared_ptr<ISpell> W = nullptr;
	std::shared_ptr<ISpell> E = nullptr;
	std::shared_ptr<ISpell> R = nullptr;
}

//number of  enemies in Ult Radius
int CountEnemiesInRangeR(const Vector& Position, const float Range)
{
	auto Enemies = g_ObjectManager->GetChampions(false);
	int Counter = 0;
	for (auto& Enemy : Enemies)
		if (Enemy->IsVisible() && Enemy->IsValidTarget() && Position.Distance(Enemy->Position()) <= Range)
			Counter++;

	auto OtherObjects = g_ObjectManager->GetOtherObjEnemy();
	for (auto OtherObject : OtherObjects)
	{
		if (OtherObject->HasBuff("illaoiespirit") && Position.Distance(OtherObject->Position()) <= Range)
			Counter++;
	}

	return Counter;
}

// Combo Logic
void ComboLogic()
{
	if (Menu::Combo::Enabled->GetBool() && Menu::Combo::UseE->GetBool() && Spells::E->IsReady() && !g_LocalPlayer->HasBuff("IllaoiR"))
	{
		auto TargetE = g_Common->GetTarget(Spells::E->Range(), DamageType::Physical);
		if (TargetE && TargetE->IsValidTarget() && TargetE->IsInRange(875.f))
			Spells::E->Cast(TargetE, HitChance::High);
	}

	//ult in combo
	if (Menu::Combo::UseR->GetBool() && Spells::R->IsReady())
	{
		const auto Enemies = g_ObjectManager->GetChampions(false);

		for (auto Enemy : Enemies)

		if (Enemy->HealthPercent() <= 70 && CountEnemiesInRangeR(g_LocalPlayer->Position(), Spells::R->Range()) >= Menu::Combo::Runits->GetInt())
			Spells::R->Cast();
	}

	if (Menu::Combo::Enabled->GetBool() && Menu::Combo::UseW->GetBool() && Spells::W->IsReady() && !Menu::Combo::UseWaa->GetBool())
	{
		auto TargetW = g_Common->GetTarget(Spells::W->Range(), DamageType::Physical);
		if (TargetW && TargetW->IsValidTarget() && TargetW->Distance(g_LocalPlayer) >= 190.f)

			Spells::W->Cast(TargetW);
	}

	auto TargetQ = g_Common->GetTarget(Spells::Q->Range(), DamageType::Physical);
	auto OtherObjects = g_ObjectManager->GetOtherObjEnemy();
	auto target = g_Common->GetTarget(275.f, DamageType::Physical);
	for (auto OtherObject : OtherObjects)
	{
		if (OtherObject->HasBuff("illaoiespirit") && !target)
		{
			if (Menu::Combo::UseQ->GetBool() && Spells::Q->IsReady())
				if (!TargetQ && OtherObject->IsInRange(Spells::Q->Range()))
					Spells::Q->Cast(OtherObject->Position());

			auto OrbwalkerTarget = g_Orbwalker->GetTarget();
			if (!g_Orbwalker->GetTarget())
				g_Orbwalker->SetOrbwalkingTarget(OtherObject);
		}
	}

	if (Menu::Combo::Enabled->GetBool() && Menu::Combo::UseQ->GetBool() && Spells::Q->IsReady())
	{
		if (TargetQ && TargetQ->IsValidTarget() && g_LocalPlayer->Distance(TargetQ) <= 740 && (!g_LocalPlayer->HasBuff("IllaoiR") && Spells::W->IsReady()))
			Spells::Q->Cast(TargetQ, HitChance::High);
	}
}

// Harass Logic
void HarassLogic()
{
	if (Menu::Harass::TowerHarass->GetBool() && g_LocalPlayer->IsUnderMyEnemyTurret())
		return;

	if (g_LocalPlayer->ManaPercent() < Menu::Harass::MinMana->GetInt())
		return;

	if (Menu::Harass::Enabled->GetBool() && Menu::Harass::UseE->GetBool()  && Spells::E->IsReady())
	{
		auto TargetE = g_Common->GetTarget(Spells::E->Range(), DamageType::Physical);
		if (TargetE && TargetE->IsValidTarget() && TargetE->IsInRange(875.f))
			Spells::E->Cast(TargetE, HitChance::High);
	}

	if (Menu::Harass::Enabled->GetBool() && Menu::Harass::UseW->GetBool() &&  Spells::W->IsReady())
	{
		auto TargetW = g_Common->GetTarget(Spells::W->Range(), DamageType::Physical);;
		if (TargetW && TargetW->IsValidTarget())
			Spells::W->Cast(TargetW);
	}

	auto TargetQ = g_Common->GetTarget(Spells::Q->Range(), DamageType::Physical);
	auto OtherObjects = g_ObjectManager->GetOtherObjEnemy();
	auto target = g_Common->GetTarget(275.f, DamageType::Physical);
	for (auto OtherObject : OtherObjects)
	{
		if (OtherObject->HasBuff("illaoiespirit") && !target)
		{
			if (Menu::Combo::UseQ->GetBool() && Spells::Q->IsReady())
				if (!TargetQ && OtherObject->IsInRange(Spells::Q->Range()))
					Spells::Q->Cast(OtherObject->Position());

			auto OrbwalkerTarget = g_Orbwalker->GetTarget();
			if (!g_Orbwalker->GetTarget())
				g_Orbwalker->SetOrbwalkingTarget(OtherObject);
		}
	}

	if (Menu::Harass::Enabled->GetBool() && Menu::Harass::UseQ->GetBool() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo) && Spells::Q->IsReady())
	{
		if (TargetQ && TargetQ->IsValidTarget())
			Spells::Q->Cast(TargetQ, HitChance::High);
	}
}

// killsteal
void KillstealLogic()
{
	const auto Enemies = g_ObjectManager->GetChampions(false);
	for (auto Enemy : Enemies)
	{
		if (Menu::Killsteal::UseW->GetBool() && Spells::W->IsReady() && Enemy->IsInRange(Spells::W->Range()))
		{
			auto WDamage = g_Common->GetSpellDamage(g_LocalPlayer, Enemy, SpellSlot::W, false) + g_LocalPlayer->AutoAttackDamage(Enemy, true);
			if (Enemy->IsValidTarget() && WDamage >= Enemy->RealHealth(true, false))
				Spells::W->Cast(Enemy);
		}

		if (Menu::Killsteal::UseQ->GetBool() && Spells::Q->IsReady() && Enemy->IsInRange(Spells::Q->Range()))
		{
			auto QDamage = g_Common->GetSpellDamage(g_LocalPlayer, Enemy, SpellSlot::Q, false);
			if (Enemy->IsValidTarget() && QDamage >= Enemy->RealHealth(true, false))
				Spells::Q->Cast(Enemy);
		}
	}
}

// Lane Clear Logic
void LaneCLearLogic()
{
	if (g_LocalPlayer->ManaPercent() < Menu::LaneClear::MinMana->GetInt())
		return;

	auto MinMinions = Menu::LaneClear::MinMinions->GetInt();
	if (!MinMinions)
		return;

	auto Target = g_Orbwalker->GetTarget();
	{
		if (Target && (Target->IsMinion() || Target->IsMonster()) && Spells::Q->Range());

		if (Spells::Q->IsReady() && Menu::LaneClear::UseQ->GetBool() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear))
			Spells::Q->CastOnBestFarmPosition(MinMinions);
	}

	if (Target && (Target->IsMinion() || Target->IsMonster()));
	{
		if (Spells::W->IsReady() && !Spells::Q->IsReady() && Menu::LaneClear::UseW->GetBool() && Spells::W->Range() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear))

			Spells::W->Cast();
	}

	// Jungle Clear Logic
	{
		auto Monster = g_Orbwalker->GetTarget();
		{
			if (Monster && Monster->IsMonster() && Menu::LaneClear::UseQ->GetBool() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear) && Spells::Q->IsReady())
				Spells::Q->Cast(Monster, HitChance::High);
		}
		{
			if (Monster && Monster->IsMonster() && Spells::W->IsReady() && Menu::LaneClear::UseW->GetBool() && Spells::W->Range() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear))

				Spells::W->Cast();
		}
	}
}

void OnGameUpdate()
{
	if (Menu::Combo::Enabled->GetBool() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo))
		ComboLogic();

	if (Menu::Harass::Enabled->GetBool() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeHarass))
		HarassLogic();

	if (Menu::LaneClear::Enabled->GetBool() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeLaneClear))
		LaneCLearLogic();

	if (Menu::Killsteal::Enabled->GetBool())
		KillstealLogic();

	// E on Dash
	if (Menu::Misc::Edash->GetBool() && Spells::E->IsReady())
	{
		const auto Enemies = g_ObjectManager->GetChampions(false);
		for (auto Enemy : Enemies)
		{
			if (Enemy->IsDashing())
			{
				const auto DashData = Enemy->GetDashData();
				if (g_LocalPlayer->Distance(DashData.EndPosition) < 630.f)
					Spells::E->Cast(Enemy, HitChance::High);
			}
		}
	}
	// auto ult
	if (Spells::R->IsReady())
	{
		if (CountEnemiesInRangeR(g_LocalPlayer->Position(), Spells::R->Range()) >= Menu::Misc::AutoR->GetInt())
			Spells::R->Cast();
	}
}
// W after AA
void OnAfterAttack(IGameObject* target)
{
	{
		if (Menu::Combo::Enabled && Menu::Combo::UseWaa->GetBool() && Spells::W->IsReady())

		{
			const auto OrbwalkerTarget = g_Orbwalker->GetTarget();
			if (OrbwalkerTarget && OrbwalkerTarget->IsAIHero() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeCombo))

			{
				Spells::W->Cast();
				g_Orbwalker->ResetAA();
			}
		}
	}
	{
		if (Menu::Harass::Enabled && Menu::Harass::UseWaa->GetBool() && Spells::W->IsReady())
		{
			const auto OrbwalkerTarget = g_Orbwalker->GetTarget();
			if (OrbwalkerTarget && OrbwalkerTarget->IsAIHero() && g_Orbwalker->IsModeActive(eOrbwalkingMode::kModeHarass))
			{
				Spells::W->Cast();
				g_Orbwalker->ResetAA();
			}
		}
	}
}

// Drawings
void OnHudDraw()
{
	if (!Menu::Drawings::Toggle->GetBool() || g_LocalPlayer->IsDead())
		return;

	const auto PlayerPosition = g_LocalPlayer->Position();
	const auto CirclesWidth = 1.5f;

	if (Menu::Drawings::DrawQRange->GetBool() && !Spells::Q->CooldownTime())
		g_Drawing->AddCircle(PlayerPosition, Spells::Q->Range(), Menu::Colors::QColor->GetColor(), CirclesWidth);

	if (Menu::Drawings::DrawERange->GetBool() && !Spells::E->CooldownTime())
		g_Drawing->AddCircle(PlayerPosition, Spells::E->Range(), Menu::Colors::EColor->GetColor(), CirclesWidth);

	if (Menu::Drawings::DrawRRange->GetBool() && !Spells::R->CooldownTime())
		g_Drawing->AddCircle(PlayerPosition, Spells::R->Range(), Menu::Colors::RColor->GetColor(), CirclesWidth);
}

PLUGIN_API bool OnLoadSDK(IPluginsSDK* plugin_sdk)
{
	DECLARE_GLOBALS(plugin_sdk);

	if (g_LocalPlayer->ChampionId() != ChampionId::Illaoi)
		return false;

	using namespace Menu;
	using namespace Spells;

	MenuInstance = g_Menu->CreateMenu("Illaoi", "Illaoi by BiggieSmalls");

	const auto ComboSubMenu = MenuInstance->AddSubMenu("Combo", "combo_menu");
	Menu::Combo::Enabled = ComboSubMenu->AddCheckBox("Enable Combo", "enable_combo", true);
	Menu::Combo::UseQ = ComboSubMenu->AddCheckBox("Use Q", "combo_use_q", true);
	Menu::Combo::UseW = ComboSubMenu->AddCheckBox("Use W", "combo_use_w", false);
	Menu::Combo::UseWaa = ComboSubMenu->AddCheckBox("Use W only after AA", "combo_use_waa", true);
	Menu::Combo::UseE = ComboSubMenu->AddCheckBox("Use E", "combo_use_e", true);
	Menu::Combo::UseR = ComboSubMenu->AddCheckBox("Use R", "r_use", false);
	Menu::Combo::Runits = ComboSubMenu->AddSlider("Min Enemies to Ult in Combo", "r_min_units", 3, 0, 5);
	Menu::Combo::Rlabel = ComboSubMenu->AddLabel("^Counts E Vessel as Champions", "combo_label", true);

	const auto HarassSubMenu = MenuInstance->AddSubMenu("Harass", "harass_menu");
	Menu::Harass::Enabled = HarassSubMenu->AddCheckBox("Enable Harass", "enable_harass", true);
	Menu::Harass::UseQ = HarassSubMenu->AddCheckBox("Use Q", "harass_use_q", false);
	Menu::Harass::UseW = HarassSubMenu->AddCheckBox("Use W", "harass_use_w", false);
	Menu::Harass::UseWaa = HarassSubMenu->AddCheckBox("Use W only after AA", "harass_use_waa", true);
	Menu::Harass::UseE = HarassSubMenu->AddCheckBox("Use E", "harass_use_e", true);
	Menu::Harass::TowerHarass = HarassSubMenu->AddCheckBox("Undertower Harass", "tower_harass", true);
	Menu::Harass::MinMana = HarassSubMenu->AddSlider("Min Mana", "min_mana_harass", 50, 0, 100, true);

	const auto LaneClearSubMenu = MenuInstance->AddSubMenu("Lane Clear", "laneclear_menu");
	Menu::LaneClear::Enabled = LaneClearSubMenu->AddCheckBox("Enable Lane Clear", "enable_laneclear", true);
	Menu::LaneClear::UseQ = LaneClearSubMenu->AddCheckBox("Use Q", "laneclear_use_q", false);
	Menu::LaneClear::UseW = LaneClearSubMenu->AddCheckBox("Use W", "laneclear_use_w", false);
	Menu::LaneClear::MinMana = LaneClearSubMenu->AddSlider("Min Mana", "min_mana_laneclear", 50, 0, 100, true);
	Menu::LaneClear::MinMinions = LaneClearSubMenu->AddSlider("Min minions", "lane_clear_min_minions", 4, 0, 9);

	const auto KSSubMenu = MenuInstance->AddSubMenu("KS", "ks_menu");
	Menu::Killsteal::Enabled = KSSubMenu->AddCheckBox("Enable Killsteal", "enable_ks", true);
	Menu::Killsteal::UseQ = KSSubMenu->AddCheckBox("Use Q", "q_ks", true);
	Menu::Killsteal::UseW = KSSubMenu->AddCheckBox("Use W", "w_ks", true);

	const auto MiscSubMenu = MenuInstance->AddSubMenu("Misc", "misc_menu");
	Menu::Misc::Edash = MiscSubMenu->AddCheckBox("E on dashing enemies", "e_dash", true);
	Menu::Misc::AutoR = MiscSubMenu->AddSlider("Auto Ult on Enemies", "auto_r", 3, 0, 5);

	const auto DrawingsSubMenu = MenuInstance->AddSubMenu("Drawings", "drawings_menu");
	Drawings::Toggle = DrawingsSubMenu->AddCheckBox("Enable Drawings", "drawings_toggle", true);
	Drawings::DrawQRange = DrawingsSubMenu->AddCheckBox("Draw Q Range", "draw_q", true);
	Drawings::DrawRRange = DrawingsSubMenu->AddCheckBox("Draw R Range", "draw_r", true);
	Drawings::DrawERange = DrawingsSubMenu->AddCheckBox("Draw E Range", "draw_e", true);

	const auto ColorsSubMenu = DrawingsSubMenu->AddSubMenu("Colors", "color_menu");
	Colors::QColor = ColorsSubMenu->AddColorPicker("Q Range", "color_q_range", 23, 175, 255, 180);
	Colors::RColor = ColorsSubMenu->AddColorPicker("R Range", "color_r_range", 200, 200, 200, 180);
	Colors::EColor = ColorsSubMenu->AddColorPicker("E Range", "color_e_range", 210, 210, 210, 180);

	Spells::Q = g_Common->AddSpell(SpellSlot::Q, 800.f);
	Spells::W = g_Common->AddSpell(SpellSlot::W, 415.f);
	Spells::E = g_Common->AddSpell(SpellSlot::E, 850.f);
	Spells::R = g_Common->AddSpell(SpellSlot::R, 400.f);

	//prediction and customs
	Spells::Q->SetSkillshot(1.0f, 95.f, FLT_MAX, kCollidesWithNothing, kSkillshotLine);
	Spells::E->SetSkillshot(0.3f, 50.f, 1900.f, kCollidesWithMinions && kCollidesWithYasuoWall, kSkillshotLine);
	Spells::R->SetSkillshot(0.75f, 400.f, FLT_MAX, kCollidesWithNothing, kSkillshotCircle);

	//EventHandler<Events::OnProcessSpellCast>::AddEventHandler(OnProcessSpell);
	//EventHandler<Events::OnBuff>::AddEventHandler(OnBuffChange);
	EventHandler<Events::GameUpdate>::AddEventHandler(OnGameUpdate);
	EventHandler<Events::OnAfterAttackOrbwalker>::AddEventHandler(OnAfterAttack);
	EventHandler<Events::OnHudDraw>::AddEventHandler(OnHudDraw);

	//EventHandler<Events::OnBeforeAttackOrbwalker>::AddEventHandler(OnBeforeAttack);

	g_Common->ChatPrint("<font color='#FFC300'>Illaoi Loaded!</font>");
	g_Common->Log("Illaoi plugin loaded.");

	return true;
}

PLUGIN_API void OnUnloadSDK()
{
	Menu::MenuInstance->Remove();

	//EventHandler<Events::OnBeforeAttackOrbwalker>::RemoveEventHandler(OnBeforeAttack);
	EventHandler<Events::GameUpdate>::RemoveEventHandler(OnGameUpdate);
	//EventHandler<Events::OnProcessSpellCast>::RemoveEventHandler(OnProcessSpell);
	EventHandler<Events::GameUpdate>::RemoveEventHandler(OnGameUpdate);
	EventHandler<Events::OnAfterAttackOrbwalker>::RemoveEventHandler(OnAfterAttack);
	EventHandler<Events::OnHudDraw>::RemoveEventHandler(OnHudDraw);
	//EventHandler<Events::OnBuff>::RemoveEventHandler(OnBuffChange);

	g_Common->ChatPrint("<font color='#00BFFF'>Illaoi Unloaded.</font>");
}