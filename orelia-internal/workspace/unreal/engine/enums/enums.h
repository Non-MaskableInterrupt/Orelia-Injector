namespace sdk {
    enum class e_fort_damage_number_type : uint8_t {
        none = 0,
        player = 1,
        enemy = 2,
        building = 3,
        shield = 4,
        score = 5
    };

    enum class e_fort_elemental_damage_type : uint8_t {
        none = 0,
        fire = 1,
        ice = 2,
        lightning = 3,
        energy = 4,
        water = 5,
        physical = 6,
        nature = 7
    };

    enum class e_stat_category : uint8_t {
        none = 0,
        combat = 1,
        building = 2,
        utility = 3,
        max_categories = 4
    };

    enum class e_root_motion_finish_velocity_mode : uint8_t {
        keep_velocity = 0,
        set_velocity = 1,
        clamp_velocity = 2
    };

    enum class e_fort_item_type : uint8_t {
        world_item = 0,
        ammo = 1,
        badge = 2,
        backpack_pickup = 3,
        building_piece = 4,
        character_part = 5,
        consumable = 6,
        deco = 7,
        edit_tool = 8,
        ingredient = 9,
        item_cache = 10,
        food = 11,
        gadget = 12,
        athena_gadget = 13,
        homebase_gadget = 14,
        spy_tech_perk = 15,
        hero_ability = 16,
        mission_item = 17,
        trap = 18,
        multi_item = 19,
        weapon = 20,
        weapon_melee = 21,
        weapon_ranged = 22,
        weapon_harvest = 23,
        weapon_creative_phone = 24,
        weapon_mod = 25,
        world_resource = 26,
        creative_user_prefab = 27,
        creative_playset = 28,
        vehicle = 29,
        npc = 30,
        player_augment = 31,
        account_item = 32,
        account_resource = 33,
        collected_resource = 34,
        alteration = 35,
        card_pack = 36,
        currency = 37,
        hero = 38,
        schematic = 39,
        worker = 40,
        team_perk = 41,
        player_tech = 42,
        token = 43,
        daily_reward_schedule_token = 44,
        code_token = 45,
        stat = 46,
        buff = 47,
        buff_credit = 48,
        quest = 49,
        accolades = 50,
        friend_chest = 51,
        medal = 52,
        repeatable_dailies_card = 53,
        challenge_bundle = 54,
        challenge_bundle_schedule = 55,
        challenge_bundle_completion_token = 56,
        gameplay_modifier = 57,
        outpost = 58,
        homebase_node = 59,
        defender = 60,
        conversion_control = 61,
        deployable_base_cloud_save = 62,
        consumable_account_item = 63,
        quota = 64,
        expedition = 65,
        homebase_banner_icon = 66,
        homebase_banner_color = 67,
        athena_sky_dive_contrail = 68,
        personal_vehicle = 69,
        athena_glider = 70,
        athena_pickaxe = 71,
        athena_backpack = 72,
        cosmetic_shoes = 73,
        athena_character = 74,
        athena_dance = 75,
        athena_loading_screen = 76,
        athena_battle_bus = 77,
        athena_vehicle_cosmetic = 78,
        athena_item_wrap = 79,
        athena_music_pack = 80,
        athena_pet_cosmetic = 81,
        athena_season_treasure = 82,
        athena_season = 83,
        athena_reward_graph = 84,
        athena_ext_resource = 85,
        event_description = 86,
        athena_event_token = 87,
        event_purchase_tracker = 88,
        cosmetic_variant_token = 89,
        campaign_hero_loadout = 90,
        playset = 91,
        preroll_data = 92,
        creative_plot = 93,
        player_survey_token = 94,
        cosmetic_locker = 95,
        banner_token = 96,
        rested_xp_booster_token = 97,
        reward_event_graph_purchase_token = 98,
        hardcore_modifier = 99,
        event_dependent_item = 100,
        item_access_token = 101,
        stw_accolade_reward = 102,
        campsite = 103,
        victory_crown = 104,
        reality_sapling = 105,
        apparel = 106,
        apparel_layout = 107,
        player_augments_persistence = 108,
        sparks_aura = 109,
        sparks_guitar = 110,
        sparks_bass = 111,
        sparks_keyboard = 112,
        sparks_microphone = 113,
        sparks_drums = 114,
        sparks_spotlight_anim = 115,
        sparks_song = 116,
        juno_building_set = 117,
        juno_building_prop = 118,
        special_item = 119,
        emote = 120,
        stack = 121,
        collection_book_page = 122,
        bga_consumable_wrapper = 123,
        gift_box = 124,
        gift_box_unlock = 125,
        playset_prop = 126,
        reg_cosmetic_def = 127,
        profile = 128,
        max = 129
    };

    enum class e_root_motion_mode : uint8_t {
        constant_force = 0,
        move_to_target = 1,
        move_to_actor = 2
    };

    enum class e_vehicle_movement_mode : uint8_t {
        on_ground = 0,
        in_air = 1,
        wipe_out = 2,
        max_count = 3
    };

    enum class e_creative_team_color : uint8_t {
        none = 0,
        white = 1,
        red = 2,
        orange = 3,
        yellow = 4,
        green = 5,
        teal = 6,
        blue = 7,
        purple = 8,
        custom_1 = 9,
        custom_2 = 10,
        custom_3 = 11,
        custom_4 = 12,
        custom_5 = 13,
        custom_6 = 14,
        custom_7 = 15,
        custom_8 = 16,
        custom_9 = 17
    };

    enum class e_color_blind_mode : uint8_t {
        off = 0,
        deuteranope = 1,
        protanope = 2,
        tritanope = 3
    };

    enum class e_fort_day_of_week : uint8_t {
        none = 0,
        monday = 1,
        tuesday = 2,
        wednesday = 4,
        thursday = 8,
        friday = 16,
        saturday = 32,
        sunday = 64,
        all = 255
    };

    enum class trace_type_query : uint8_t {
        trace_type_query_1 = 0,
        trace_type_query_2 = 1,
        trace_type_query_3 = 2,
        trace_type_query_4 = 3,
        trace_type_query_5 = 4,
        trace_type_query_6 = 5,
        trace_type_query_7 = 6,
        trace_type_query_8 = 7,
        trace_type_query_9 = 8,
        trace_type_query_10 = 9,
        trace_type_query_11 = 10,
        trace_type_query_12 = 11,
        trace_type_query_13 = 12,
        trace_type_query_14 = 13,
        trace_type_query_15 = 14,
        trace_type_query_16 = 15,
        trace_type_query_17 = 16,
        trace_type_query_18 = 17,
        trace_type_query_19 = 18,
        trace_type_query_20 = 19,
        trace_type_query_21 = 20,
        trace_type_query_22 = 21,
        trace_type_query_23 = 22,
        trace_type_query_24 = 23,
        trace_type_query_25 = 24,
        trace_type_query_26 = 25,
        trace_type_query_27 = 26,
        trace_type_query_28 = 27,
        trace_type_query_29 = 28,
        trace_type_query_30 = 29,
        trace_type_query_31 = 30,
        trace_type_query_32 = 31,
        trace_type_query_max = 32,
        e_trace_type_query_max = 33
    };

    enum class draw_debug_trace : uint8_t {
        none = 0,
        for_one_frame = 1,
        for_duration = 2,
        persistent = 3,
        e_draw_debug_trace_max = 4
    };

    enum class e_psc_pool_method : uint8_t {
        none,
        auto_release,
        manual_release
    };

    enum e_net_role : std::uint8_t {
        e_net_role_none,
        e_net_role_simulated_proxy,
        e_net_role_autonomous_proxy,
        e_net_role_authority
    };

    enum e_net_dormancy : std::uint8_t {
        e_net_dormancy_never,
        e_net_dormancy_awake,
        e_net_dormancy_dormant_all,
        e_net_dormancy_dormant_partial,
        e_net_dormancy_initial
    };

    enum e_internal_object_flags : std::uint32_t {
        none = 0x00000000,
        reachable_in_cluster = 0x00800000,
        cluster_root = 0x01000000,
        native = 0x02000000,
        async = 0x04000000,
        async_loading = 0x08000000,
        unreachable = 0x10000000,
        pending_kill = 0x20000000,
        root_set = 0x40000000,
        garbage_collection_keep_flags = 0x80000000,
        all_flags = 0xFF800000
    };

    enum class e_collision_response : uint8_t {
        ignore = 0,
        overlap = 1,
        block = 2
    };

    enum class e_collision_channel : uint8_t {
        world_static = 0,
        world_dynamic = 1,
        pawn = 2,
        visibility = 3,
        camera = 4,
        physics_body = 5,
        vehicle = 6,
        destructible = 7,
        engine_trace_1 = 8,
        engine_trace_2 = 9,
        engine_trace_3 = 10,
        engine_trace_4 = 11,
        engine_trace_5 = 12,
        engine_trace_6 = 13,
        game_trace_1 = 14,
        game_trace_2 = 15,
        game_trace_3 = 16,
        game_trace_4 = 17,
        game_trace_5 = 18,
        game_trace_6 = 19,
        game_trace_7 = 20,
        game_trace_8 = 21,
        game_trace_9 = 22,
        game_trace_10 = 23,
        game_trace_11 = 24,
        game_trace_12 = 25,
        game_trace_13 = 26,
        game_trace_14 = 27,
        game_trace_15 = 28,
        game_trace_16 = 29,
        game_trace_17 = 30,
        game_trace_18 = 31,
        overlap_all_deprecated = 32
    };

    enum class e_weapon_core_animation : std::uint8_t {
        melee = 0,
        pistol = 1,
        shotgun = 2,
        paper_blueprint = 3,
        rifle = 4,
        melee_one_hand = 5,
        machine_pistol = 6,
        rocket_launcher = 7,
        grenade_launcher = 8,
        going_commando = 9,
        assault_rifle = 10,
        tactical_shotgun = 11,
        sniper_rifle = 12,
        trap_placement = 13,
        shoulder_launcher = 14,
        ability_deco_tool = 15,
        crossbow = 16,
        c4 = 17,
        remote_control = 18,
        dual_wield = 19,
        ar_bullpup = 20,
        ar_forward_grip = 21,
        med_pack_paddles = 22,
        smg_p90 = 23,
        ar_drum_gun = 24,
        consumable_small = 25,
        consumable_large = 26,
        balloon = 27,
        mounted_turret = 28,
        creative_tool = 29,
        explosive_bow = 30,
        ashton_indigo = 31,
        ashton_chicago = 32,
        melee_dual_wield = 33,
        unarmed = 34,
        max = 35
    };

    enum class e_fort_rarity : std::uint8_t {
        common = 0,
        uncommon = 1,
        rare = 2,
        epic = 3,
        legendary = 4,
        mythic = 5,
        transcendent = 6,
        unattainable = 7,
        num_rarity_values = 8
    };

    enum class e_blend_mode : std::uint8_t {
        opaque = 0,
        masked = 1,
        translucent = 2,
        additive = 3,
        modulate = 4,
        alpha_composite = 5,
        alpha_holdout = 6,
        translucent_colored_transmittance = 7,
        max = 8,
        translucent_grey_transmittance = 2,
        colored_transmittance_only = 4
    };

    enum class e_object_flags : std::uint32_t {
        no_flags = 0x000000,
        public_flag = 0x000001,
        standalone = 0x000002,
        mark_as_native = 0x000004,
        transactional = 0x000008,
        class_default_object = 0x000010,
        archetype_object = 0x000020,
        transient = 0x000040,
        mark_as_root_set = 0x000080,
        tag_garbage_temp = 0x000100,
        need_initialization = 0x000200,
        need_load = 0x000400,
        keep_for_cooker = 0x000800,
        need_post_load = 0x001000,
        need_post_load_subobjects = 0x002000,
        newer_version_exists = 0x004000,
        begin_destroyed = 0x008000,
        finish_destroyed = 0x010000,
        being_regenerated = 0x020000,
        default_sub_object = 0x040000,
        was_loaded = 0x080000,
        text_export_transient = 0x100000,
        load_completed = 0x200000,
        inheritable_component_template = 0x400000,
        duplicate_transient = 0x800000,
        strong_ref_on_frame = 0x1000000,
        non_pie_duplicate_transient = 0x2000000,
        dynamic = 0x4000000,
        will_be_loaded = 0x8000000
    };
}