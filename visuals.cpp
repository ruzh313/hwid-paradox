struct Vec2 {
    float x, y;

    Vec2() : x(0), y(0) {}
    Vec2(float x, float y) : x(x), y(y) {}
};

// FadeState struct for managing the fade effect
struct FadeState {
    float alpha;
    std::chrono::steady_clock::time_point last_update;
    float fade_duration;
    bool is_fading_in;

    FadeState() : alpha(0.0f), fade_duration(0.5f), is_fading_in(true) {
        last_update = std::chrono::steady_clock::now();
    }

    void start_fade_in() {
        is_fading_in = true;
        last_update = std::chrono::steady_clock::now();
    }

    void start_fade_out() {
        is_fading_in = false;
        last_update = std::chrono::steady_clock::now();
    }

    void update() {
        auto now = std::chrono::steady_clock::now();
        float delta_time = std::chrono::duration<float>(now - last_update).count();
        last_update = now;

        if (is_fading_in) {
            alpha += delta_time / fade_duration;
            if (alpha > 1.0f) {
                alpha = 1.0f;
            }
        }
        else {
            alpha -= delta_time / fade_duration;
            if (alpha < 0.0f) {
                alpha = 0.0f;
            }
        }
    }

    int get_alpha() const {
        return static_cast<int>(alpha * 255.0f);
    }

    bool is_visible() const {
        return alpha > 0.0f;
    }
};

// Map to track the fade states of each indicator
std::map<std::string, FadeState> indicator_fades;

// Variables to track dragging state
bool dragging_keybindlist = false;
Vec2 keybindlist_offset = { 0, 0 };
Vec2 keybindlist_drag_offset = { 0, 0 };

// Function to update keybindlist position if dragging
void update_keybindlist_drag() {
    bool pressed = g_input.GetKeyPress(VK_LBUTTON);
    bool down = g_input.GetKeyState(VK_LBUTTON);

    if (dragging_keybindlist && !down) {
        dragging_keybindlist = false;
    }

    if (dragging_keybindlist && down) {
        keybindlist_offset.x = g_input.m_mouse.x - keybindlist_drag_offset.x;
        keybindlist_offset.y = g_input.m_mouse.y - keybindlist_drag_offset.y;
    }

    Rect keybindlist_rect = { keybindlist_offset.x, keybindlist_offset.y, 150, 20 };

    if (pressed && g_input.IsCursorInRect(keybindlist_rect)) {
        dragging_keybindlist = true;
        keybindlist_drag_offset.x = g_input.m_mouse.x - keybindlist_offset.x;
        keybindlist_drag_offset.y = g_input.m_mouse.y - keybindlist_offset.y;
    }
}

void Visuals::keybindlist() {
    if (!g_menu.main.debug.keybindlist.get()) {
        return;
    }

    bool keybindlist_enabled = g_menu.main.debug.keybindlist.get();
    std::set<std::string> current_indicators;

    Color color = g_gui.m_color;
    struct Indicator_t { std::string text; std::string mode; };
    std::vector<Indicator_t> indicators{};

    if (g_visuals.m_thirdperson) {
        Indicator_t ind{};
        ind.text = XOR("Thirdperson");
        ind.mode = XOR("on");
        indicators.push_back(ind);
        current_indicators.insert(ind.text);
    }

    if (g_aimbot.m_force_body) {
        Indicator_t ind{};
        ind.text = XOR("Force Body Aim");
        ind.mode = XOR("on");
        indicators.push_back(ind);
        current_indicators.insert(ind.text);
    }

    if (g_hvh.m_left) {
        Indicator_t ind{};
        ind.text = XOR("Yaw Base");
        ind.mode = XOR("left");
        indicators.push_back(ind);
        current_indicators.insert(ind.text);
    }

    if (g_hvh.m_right) {
        Indicator_t ind{};
        ind.text = XOR("Yaw Base");
        ind.mode = XOR("right");
        indicators.push_back(ind);
        current_indicators.insert(ind.text);
    }

    if (g_hvh.m_back) {
        Indicator_t ind{};
        ind.text = XOR("Yaw Base");
        ind.mode = XOR("back");
        indicators.push_back(ind);
        current_indicators.insert(ind.text);
    }

    if (g_hvh.m_forward) {
        Indicator_t ind{};
        ind.text = XOR("Yaw Base");
        ind.mode = XOR("forward");
        indicators.push_back(ind);
        current_indicators.insert(ind.text);
    }

    if (g_input.GetKeyState(g_menu.main.aimbot.override.get())) {
        Indicator_t ind{};
        ind.text = XOR("Resolver Override");
        ind.mode = XOR("hold");
        indicators.push_back(ind);
        current_indicators.insert(ind.text);
    }

    if (g_aimbot.m_damage_toggle) {
        Indicator_t ind{};
        ind.text = XOR("Damage Override");
        ind.mode = tfm::format(XOR("%i"), g_menu.main.aimbot.override_dmg_value.get());
        indicators.push_back(ind);
        current_indicators.insert(ind.text);
    }

    if (g_aimbot.m_fake_latency) {
        Indicator_t ind{};
        ind.text = XOR("Ping Spike");
        ind.mode = tfm::format(XOR("%i"), g_menu.main.misc.fake_latency_amt.get());
        indicators.push_back(ind);
        current_indicators.insert(ind.text);
    }

    if (g_input.GetKeyState(g_menu.main.misc.fakewalk.get())) {
        Indicator_t ind{};
        ind.text = XOR("Fake Walk");
        ind.mode = XOR("hold");
        indicators.push_back(ind);
        current_indicators.insert(ind.text);
    }

    if (indicators.empty()) {
        return;
    }

    update_keybindlist_drag();

    float x_offset = keybindlist_offset.x;
    float y_offset = keybindlist_offset.y;
    int radius = 5;
    int border_thickness = 1;
    int padding = 5;
    int rect_x = 10 + x_offset;
    int rect_y = 10 + y_offset;
    int rect_width = 150;
    int rect_height = 20;

    int indicator_bg_height = (indicators.size() * 15) + padding;
    float target_keybindlist_height = rect_height + indicator_bg_height;

    static float current_keybindlist_height = 20.0f;
    float animation_speed = 0.2f;

    current_keybindlist_height += (target_keybindlist_height - current_keybindlist_height) * animation_speed;

    render::round_rect(rect_x - border_thickness, rect_y - border_thickness,
        rect_width + 2 * border_thickness, current_keybindlist_height + 2 * border_thickness,
        radius + border_thickness, { 255, 255, 255, 60 });

    render::round_rect(rect_x, rect_y, rect_width, current_keybindlist_height,
        radius, { 23, 23, 23, 255 });

    render::hud.string(rect_x + rect_width / 2, rect_y + rect_height / 5,
        colors::white, XOR("Hotkeys"), render::ALIGN_CENTER);

    int indicator_bg_x = rect_x;
    int indicator_bg_y = rect_y + rect_height;

    render::round_rect(indicator_bg_x, indicator_bg_y, rect_width, indicator_bg_height,
        radius, { 0, 0, 0, 35 });

    for (size_t i{ }; i < indicators.size(); ++i) {
        auto& indicator = indicators[i];




        render::hud.string(rect_x + padding, indicator_bg_y + padding + (i * 15),
            { 255, 255, 255, 255 }, indicator.text, render::ALIGN_LEFT);
        render::hud.string(rect_x + rect_width - padding, indicator_bg_y + padding + (i * 15),
            { 255, 255, 255, 255 }, indicator.mode, render::ALIGN_RIGHT);
    }
}
